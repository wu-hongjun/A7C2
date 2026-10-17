"""Bounded Sony service transport. All device access stays in this module."""
import struct
import time
import core
import usb.core
import usb.util
import usb.backend.libusb1
import libusb_package
from pmca.usb.driver.generic import GenericUsbDriver,MscBbbDriver
from pmca.usb.driver.generic.libusb import UsbBackend
from pmca.usb.sony import SonySenserAuthDevice,SonySenserDevice,SonySenserCamera,SonyExtCmdCamera,SonyMscExtCmdDevice
from pmca.usb import MscDevice

class Bounded(UsbBackend):
    def __init__(self,device,budget=180):
        super().__init__(device);self.budget=budget;self.reset()
    def reset(self):self.deadline=time.monotonic()+self.budget
    def timeout(self,t=None):
        left=int((self.deadline-time.monotonic())*1000)
        if left<=0:raise TimeoutError('相機操作超出有界等待時間')
        return max(1,min(left,t or self.budget*1000))
    def read(self,ep,length,timeout=None):return super().read(ep,length,self.timeout(timeout))
    def write(self,ep,data):self.dev.default_timeout=self.timeout();return super().write(ep,data)

def get_device(pid):
    backend=usb.backend.libusb1.get_backend(find_library=libusb_package.find_library)
    if backend is None:raise RuntimeError('找不到 libusb 程式庫')
    found=list(usb.core.find(find_all=True,idVendor=0x054c,idProduct=pid,backend=backend))
    if len(found)!=1:raise RuntimeError(f'需要且只能連接一台目標相機（USB {pid:04x}）；請確認大量儲存模式及驅動')
    if not list(found[0]):raise RuntimeError('無法讀取 USB 配置')
    return found[0]

def identity():
    dev=get_device(0x0e8a)
    try:
        driver=MscBbbDriver(Bounded(dev,20))
        if MscDevice(driver).getDeviceInfo().manufacturer.strip()!='Sony':raise RuntimeError('不是 Sony 裝置')
        info=SonyExtCmdCamera(SonyMscExtCmdDevice(driver)).getCameraInfo()
        if info.modelName!=core.MODEL:raise RuntimeError('只支援 '+core.MODEL)
        return {'model':info.modelName,'serial_sha256':core.sha(info.serial.encode())}
    finally:usb.util.dispose_resources(dev)

class Session:
    def __init__(self,expected=None):self.expected=expected;self.healthy=True
    def __enter__(self):
        self.identity=identity()
        if self.expected and self.identity!=self.expected:raise RuntimeError('目前機身與備份機身不同')
        dev=get_device(0x0e8a)
        auth=None
        try:
            auth=SonySenserAuthDevice(GenericUsbDriver(Bounded(dev,20)))
            auth.start();auth.authenticate()
        except BaseException:
            if auth is not None:auth.stop()
            raise
        finally:usb.util.dispose_resources(dev)
        deadline=time.monotonic()+15
        while True:
            try:self.dev=get_device(0x0336);break
            except RuntimeError:
                if time.monotonic()>=deadline:raise RuntimeError('服務身份未出現；若是第一次，請依文件安裝0336驅動')
                time.sleep(.5)
        time.sleep(2)
        self.backend=Bounded(self.dev)
        self.auth=SonySenserAuthDevice(GenericUsbDriver(self.backend))
        try:
            self.auth.start();self.auth.authenticate()
            self.channel=SonySenserDevice(self.auth.driver)
            self.camera=SonySenserCamera(self.channel)
            if len(self.camera.readHasp())!=88:raise RuntimeError('ProductInfo 回應不符')
        except BaseException:
            try:self.auth.stop()
            finally:usb.util.dispose_resources(self.dev)
            raise
        return self
    def __exit__(self,*args):
        try:
            if self.healthy:self.auth.stop()
        finally:usb.util.dispose_resources(self.dev)
    def call(self,method,*args):
        if not self.healthy:raise RuntimeError('會話結果未知，停止後續命令')
        self.backend.reset()
        try:return method(*args)
        except Exception:self.healthy=False;raise
    def jiritsu(self,cmd,data=b'',missing=False):
        if cmd not in [15,16,17,18]:raise ValueError('不允許此 Jiritsu 命令')
        response,out=self.call(self.channel.sendSenserPacket,0x40,struct.pack('<HH',0x1001,cmd)+data)
        if response or len(out)<4:raise RuntimeError('Jiritsu 回應格式錯誤')
        status=struct.unpack_from('<I',out)[0]
        if missing and status==0x10000006:return None
        if status:raise RuntimeError(f'Jiritsu 狀態 {status:#x}')
        return out[4:]
    def select(self,path):
        if not path.startswith('/tmp/jiritsu/') or '\0' in path or len(path)>510:raise ValueError('路徑不符')
        self.jiritsu(15,b'\0'+path.encode('ascii')+b'\0')
    def read(self,path,missing=False):self.select(path);return self.jiritsu(17,b'\0\xff\0',missing)
    def temp(self,name,data=None,remove=False):
        if name not in core.NATIVE_PATHS:raise ValueError('不是指定臨時檔')
        path='/tmp/jiritsu/'+name;self.select(path)
        if remove:self.jiritsu(18,b'\0\xff')
        elif data:
            if len(data)>65536:raise ValueError('臨時檔過大')
            self.jiritsu(16,b'\0\xff\0'+data)
        else:raise ValueError('不允許空檔寫入')
    def cp_read(self,key):
        if key not in core.CP_KEYS:raise ValueError('不允許此 CP 單元')
        return self.call(self.camera._sendAdjustControlPacket,0x603,0x40,struct.pack('<II',key,1))
    def cp_write(self,key,data):
        if key not in core.CP_KEYS or len(data)!={core.CP_KEYS[0]:8192,core.CP_KEYS[1]:28672}[key]:raise ValueError('CP 寫入目標或長度不符')
        return self.call(self.camera._sendAdjustControlPacket,0x603,0x41,struct.pack('<II',key,1)+data)
    def ap_write(self,key,data):
        if key not in core.AP_KEYS or len(data)!=1:raise ValueError('AP 綁定目標或長度不符')
        return self.call(self.camera.writeBackup,key,data)

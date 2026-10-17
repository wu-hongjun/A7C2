"""A7C II 2.01 preview installer. Per-body backup, explicit phases, no retries."""
import argparse
from datetime import datetime
import json
from pathlib import Path
import shutil
import struct
import core

def event(message):print(message,flush=True)

class Job:
    def __init__(self,workspace):
        self.root=Path(workspace).resolve();self.state_path=self.root/'state.json'
    def save(self,phase,**values):
        self.state.update(values);self.state['phase']=phase
        core.save_json(self.state_path,self.state)
    def audit(self,step,**values):
        row={'time':datetime.now().isoformat(),'step':step,**values}
        self.state.setdefault('journal',[]).append(row);core.save_json(self.state_path,self.state)
    def load(self):
        core.assets();self.state=core.read_json(self.state_path)
        if self.state.get('format')!=1 or self.state.get('model')!=core.MODEL:raise ValueError('不是有效機身工作目錄')
        for name,expected in self.state['backup_files'].items():
            for parent in [self.root/'original',Path(self.state['backup_copy'])]:
                if core.sha((parent/name).read_bytes())!=expected:raise ValueError('備份副本雜湊不符：'+name)
        core.backup_records((self.root/'original/backup.private.bin').read_bytes())
        return self
    def module(self,session,name,mode=None):
        blob=(core.BASE/'assets'/f'{name}.so').read_bytes()
        result={'installer':'a7c2.install.result','probe':'a7c2.probe.result',
                'wbi':'a7c2.wbi.result','runtime':'a7c2.runtime.result'}[name]
        request={'installer':('a7c2.install.request',b'A7INS001'),
                 'wbi':('a7c2.wbi.request',b'A7WBI001')}.get(name)
        names=['libJit.so','a7c2-preview-staged.so',result]
        if name=='runtime':names.append('a7c2.probe.result')
        if request:names.append(request[0])
        for item in names:
            if session.read('/tmp/jiritsu/'+item,True) is not None:
                raise RuntimeError('存在先前臨時檔，先保存並確認結果，請勿直接重試：'+item)
        if session.read('/tmp/jiritsu/../../etc/ld.so.preload',True):raise RuntimeError('發現既有預載配置，停止')
        created=[]
        try:
            created.append('a7c2-preview-staged.so');session.temp(created[-1],blob)
            if session.read('/tmp/jiritsu/'+created[-1])!=blob:raise RuntimeError('臨時模組讀回不符')
            if request:
                contents=struct.pack('<8sII',request[1],mode,0)
                created.append(request[0]);session.temp(request[0],contents)
                if session.read('/tmp/jiritsu/'+request[0])!=contents:raise RuntimeError('執行模式讀回不符')
            self.audit('native_about_to_run',module=name,mode=mode)
            created.extend(['libJit.so',result])
            if name=='runtime':created.append('a7c2.probe.result')
            session.temp('libJit.so',blob)
            raw=session.read('/tmp/jiritsu/'+result)
            dest=self.root/'evidence'/f'{datetime.now():%Y%m%d-%H%M%S-%f}-{name}.bin'
            dest.parent.mkdir(exist_ok=True);dest.write_bytes(raw)
            parsed=core.parse_native(name,raw,mode)
            self.audit('native_verified',module=name,mode=mode,result=parsed)
            return parsed
        finally:
            for item in reversed(list(dict.fromkeys(created))):
                if not session.healthy:
                    self.audit('temporary_cleanup_pending',name=item);continue
                if session.read('/tmp/jiritsu/'+item,True) is not None:session.temp(item,remove=True)
                if session.read('/tmp/jiritsu/'+item,True) is not None:raise RuntimeError('臨時檔清理未完成')
    def prepare(self,second):
        from transport import Session
        core.assets()
        if self.root.exists() and any(self.root.iterdir()):raise ValueError('請選擇新的空白工作目錄，保留舊紀錄')
        second=Path(second).resolve()
        if second==self.root or self.root in second.parents or second in self.root.parents:raise ValueError('第二份備份請使用獨立目錄，不能互相包含')
        if second.exists() and any(second.iterdir()):raise ValueError('第二份備份目錄必須為空')
        self.root.mkdir(parents=True,exist_ok=True);second.mkdir(parents=True,exist_ok=True)
        (self.root/'original').mkdir();(self.root/'candidate').mkdir()
        self.state={'format':1,'model':core.MODEL,'version':core.VERSION,'phase':'preparing',
                    'backup_copy':str(second),'backup_files':{},'journal':[]}
        self.save('preparing')
        with Session() as session:
            self.state['identity']=session.identity
            event('已辨識 A7C II，正在備份原始程式及這台相機的設定。')
            backup=session.call(session.camera.getBackupPresetData,True);records=core.backup_records(backup)
            originals={}
            for chip,name,path in [(1,core.PROGRAMS[1],'/cp/cpapp-b.bin'),(0,core.PROGRAMS[0],'/usr/lib/appFw.so')]:
                event('讀取完整 '+name)
                data=session.read('/tmp/jiritsu/../..'+path)
                if core.sha(data)!=core.ORIGINAL[chip]:raise ValueError('程式不是支援的原版 2.01；拒絕繼續')
                originals[name+'.bin']=data
                (self.root/'candidate'/name).write_bytes(core.patch_program(data,chip))
            originals['backup.private.bin']=backup
            for key in core.CP_KEYS:
                data=session.cp_read(key)
                if data!=records[1][key].data:raise ValueError('原生色彩讀回與機身備份不符')
                originals[f'unit-{key:08x}.bin']=data
            for key in core.AP_KEYS:
                if session.call(session.camera.readBackup,key)!=b'\0' or records[0][key].data!=b'\0':
                    raise ValueError('保留槽已使用；此預覽版拒絕覆蓋')
            merged=core.merge_colors(originals['unit-02a40001.bin'],originals['unit-02a4000d.bin'])
            for key,data in zip(core.CP_KEYS,merged):(self.root/'candidate'/f'unit-{key:08x}.bin').write_bytes(data)
            for name,data in originals.items():
                for parent in [self.root/'original',second]:
                    target=parent/name;target.write_bytes(data)
                    if core.sha(target.read_bytes())!=core.sha(data):raise ValueError('備份寫回驗證不符')
                self.state['backup_files'][name]=core.sha(data)
            self.save('backups_verified')
            self.module(session,'probe')
            self.module(session,'installer',0)
            self.module(session,'wbi',0)
        self.save('prepared')
        event('備份、候選與實機只校驗完成。尚未改寫韌體或保存拍攝設定。')
    def units(self):
        originals={k:(self.root/'original'/f'unit-{k:08x}.bin').read_bytes() for k in core.CP_KEYS}
        merged=core.merge_colors(originals[core.CP_KEYS[0]],originals[core.CP_KEYS[1]])
        return originals,dict(zip(core.CP_KEYS,merged))
    def files(self,session,expected):
        for chip,name,path in [(1,core.PROGRAMS[1],'/cp/cpapp-b.bin'),(0,core.PROGRAMS[0],'/usr/lib/appFw.so')]:
            event('完整讀回 '+name)
            data=session.read('/tmp/jiritsu/../..'+path)
            folder=self.root/'readback';folder.mkdir(exist_ok=True)
            stamp=f'{datetime.now():%Y%m%d-%H%M%S-%f}-{name}'
            (folder/stamp).write_bytes(data)
            if core.sha(data)!=expected[chip]:raise ValueError('程式完整讀回不符；停止後續操作')
        self.audit('host_full_file_readback_verified',hashes=expected)
    def settings(self,session,restore=False):
        originals,merged=self.units();target=originals if restore else merged
        desired=dict(zip(core.AP_KEYS,[b'\0',b'\0'] if restore else [b'\x0a',b'\x0b']))
        before_raw=session.call(session.camera.getBackupPresetData,True);before=core.backup_records(before_raw)
        folder=self.root/'evidence';folder.mkdir(exist_ok=True)
        (folder/f'{datetime.now():%Y%m%d-%H%M%S-%f}-settings-before.private.bin').write_bytes(before_raw)
        current_cp={k:session.cp_read(k) for k in core.CP_KEYS}
        current_ap={k:session.call(session.camera.readBackup,k) for k in core.AP_KEYS}
        for key in core.CP_KEYS:
            accepted=[originals[key],merged[key]] if restore else [originals[key]]
            if current_cp[key] not in accepted or before[1][key].data!=current_cp[key]:raise ValueError('CP 設定狀態未知，拒絕寫入')
        for i,key in enumerate(core.AP_KEYS):
            accepted=[b'\0',bytes([10+i])] if restore else [b'\0']
            if current_ap[key] not in accepted or before[0][key].data!=current_ap[key]:raise ValueError('AP 設定狀態未知，拒絕寫入')
        for key in core.CP_KEYS:
            if current_cp[key]==target[key]:continue
            self.audit('about_to_write_cp',key=hex(key));session.cp_write(key,target[key])
            if session.cp_read(key)!=target[key]:raise ValueError('CP 讀回失敗')
        if any(current_cp[k]!=target[k] for k in core.CP_KEYS):
            self.audit('about_to_save_cp');session.call(session.camera.saveBackup,0x02a4)
        for key in core.AP_KEYS:
            if current_ap[key]==desired[key]:continue
            self.audit('about_to_write_ap',key=hex(key));session.ap_write(key,desired[key])
            if session.call(session.camera.readBackup,key)!=desired[key]:raise ValueError('AP 綁定讀回失敗')
        if any(current_ap[k]!=desired[k] for k in core.AP_KEYS):
            self.audit('about_to_save_ap');session.call(session.camera.saveBackup,0x02cf)
        after_raw=session.call(session.camera.getBackupPresetData,True)
        stored_raw=session.call(session.camera.getBackupPresetData,False)
        (folder/f'{datetime.now():%Y%m%d-%H%M%S-%f}-settings-after.private.bin').write_bytes(after_raw)
        (folder/f'{datetime.now():%Y%m%d-%H%M%S-%f}-settings-stored.private.bin').write_bytes(stored_raw)
        expected={(0,k):v for k,v in desired.items()};expected.update({(1,k):v for k,v in target.items()})
        preservation=core.verify_preservation(before,core.backup_records(after_raw),expected)
        stored=core.backup_records(stored_raw)
        for (chip,key),value in expected.items():
            if stored[chip][key].data!=value:raise ValueError('儲存後讀回不符')
        self.audit('settings_storage_verified',restore=restore,**preservation)
    def install(self,battery):
        from transport import Session
        self.load()
        if self.state['phase']!='prepared':raise ValueError('需先完成準備；狀態未知時不允許重跑')
        if not 50<=battery<=100:raise ValueError('請確認電量至少50%，並保持連線')
        with Session(self.state['identity']) as session:
            # Recheck every per-body target before any program-file write.
            originals,_=self.units();current=core.backup_records(session.call(session.camera.getBackupPresetData,True))
            if any(current[1][k].data!=v for k,v in originals.items()) or any(current[0][k].data!=b'\0' for k in core.AP_KEYS):
                raise ValueError('準備後機身目標設定已變；請勿覆蓋')
            self.module(session,'wbi',0)
            self.save('writing_programs');event('正在寫入固定窗口；請勿拔線或關機。')
            self.module(session,'installer',1)
            self.save('programs_written');self.files(session,core.FINAL)
            self.save('writing_settings');self.settings(session)
            self.save('settings_verified');event('程式及保存設定已驗證，正在處理舊開機快照。')
            self.module(session,'wbi',1)
        self.save('installed_pending_cold_start')
        event('安裝及讀回完成。拔線、正常關機，等存取燈熄滅後取出電池10秒，再裝回開機。')
        event('請分別重設FL2/FL3、拍新照片及回放；目前是實驗預覽版。')
    def verify(self):
        from transport import Session
        self.load()
        with Session(self.state['identity']) as session:
            self.files(session,core.FINAL)
            current=core.backup_records(session.call(session.camera.getBackupPresetData,True))
            _,new=self.units()
            if any(current[1][k].data!=v for k,v in new.items()):raise ValueError('重開後 CP 參數不符')
            if any(current[0][k].data!=bytes([10+i]) for i,k in enumerate(core.AP_KEYS)):raise ValueError('重開後 AP 綁定不符')
            runtime=self.module(session,'runtime')
            candidate=core.patch_program((self.root/'original/appFw.so.bin').read_bytes(),0)
            for span in runtime['spans']:
                expected=candidate[span['offset']:span['offset']+span['length']].hex()
                if span['memory']!=expected or span['disk']!=expected:
                    raise ValueError('執行期仍有舊資料；請確認已關機並取出電池後再開機')
        self.audit('post_restart_files_and_targets_verified')
        event('磁碟程式與保存參數符合預期；選單、照片及回放仍需實際檢查。')
    def restore(self,battery):
        from transport import Session
        self.load()
        if not 50<=battery<=100:raise ValueError('回復前需確認電量至少50%')
        with Session(self.state['identity']) as session:
            # Native full-candidate check occurs inside restore worker before
            # writes. Independent readback here also rejects partial images.
            self.files(session,core.FINAL);self.module(session,'wbi',0)
            self.save('restoring_settings');self.settings(session,True)
            self.save('restoring_programs');self.module(session,'installer',2)
            self.files(session,core.ORIGINAL);self.module(session,'wbi',1)
        self.save('restored_pending_cold_start')
        event('原檔與四項目標設定已回復並讀回。正常關機後取出電池10秒再開機。')

def main():
    p=argparse.ArgumentParser(description='A7C II 2.01 FL2/FL3 實驗預覽版')
    p.add_argument('command',choices=['prepare','install','verify','restore','check-package'])
    p.add_argument('--workspace',type=Path);p.add_argument('--backup-copy',type=Path)
    p.add_argument('--battery-percent',type=int,default=-1)
    args=p.parse_args()
    if args.command=='check-package':core.assets();event('套件資產完整；此檢查不連線相機。');return
    if not args.workspace:p.error('必須指定機身工作目錄')
    job=Job(args.workspace)
    try:
        if args.command=='prepare':
            if not args.backup_copy:p.error('準備時需指定第二份備份目錄')
            job.prepare(args.backup_copy)
        elif args.command=='install':job.install(args.battery_percent)
        elif args.command=='verify':job.verify()
        else:job.restore(args.battery_percent)
    except Exception as exc:
        if hasattr(job,'state'):
            job.state['last_error']=str(exc);job.state['failed_during']=job.state['phase']
            job.save('needs_review')
        event('已停止：'+str(exc));event('不要盲目重跑寫入或重新開機；保留工作目錄供確認。')
        import traceback
        diagnostic=job.root/'evidence';diagnostic.mkdir(parents=True,exist_ok=True)
        (diagnostic/f'{datetime.now():%Y%m%d-%H%M%S-%f}-error.txt').write_text(traceback.format_exc(),encoding='utf-8')
        raise SystemExit(1)
if __name__=='__main__':main()

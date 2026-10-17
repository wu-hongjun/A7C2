"""Back up one A7C II without uploading modules or writing camera storage."""
import argparse
import os
from pathlib import Path
import core


class BackupReader:
    """Expose only the read operations needed for programs/settings backups."""
    def __init__(self,expected,session_factory=None):
        self.expected=expected;self.factory=session_factory

    def __enter__(self):
        if self.factory is None:
            from transport import Session
            self.factory=Session
        self.session=self.factory(self.expected)
        self.session.__enter__()
        return self

    def __exit__(self,*args):return self.session.__exit__(*args)
    def backup(self,memory):
        if type(memory) is not bool:raise ValueError('Memory/storage selector must be boolean')
        return self.session.call(self.session.camera.getBackupPresetData,memory)
    def program(self,name):
        paths={'appFw.so':'/tmp/jiritsu/../../usr/lib/appFw.so',
               'cpapp-b.bin':'/tmp/jiritsu/../../cp/cpapp-b.bin'}
        if name not in paths:raise ValueError('Not an approved program read')
        return self.session.read(paths[name])
    def cp_unit(self,key):
        if key not in core.CP_KEYS:raise ValueError('Not an approved CP unit read')
        return self.session.cp_read(key)
    def ap_binding(self,key):
        if key not in core.AP_KEYS:raise ValueError('Not an approved AP binding read')
        return self.session.call(self.session.camera.readBackup,key)


def normal_info():
    """The same model/version query used by upstream PMCA info, no update data."""
    from transport import get_device,Bounded,MscBbbDriver,SonyExtCmdCamera,SonyMscExtCmdDevice
    from pmca.usb.sony import SonyUpdaterCamera
    import usb.util
    device=get_device(0x0e8a)
    try:
        driver=MscBbbDriver(Bounded(device,20));ext=SonyMscExtCmdDevice(driver)
        info=SonyExtCmdCamera(ext).getCameraInfo()
        if info.modelName!=core.MODEL:raise ValueError('Unexpected camera model')
        query=SonyUpdaterCamera(ext);query.init();version,_=query.getFirmwareVersion()
        return {'model':info.modelName,'serial_sha256':core.sha(info.serial.encode()),
                'firmware_version':version}
    finally:usb.util.dispose_resources(device)


class Copies:
    def __init__(self,workspace,second,identity):
        self.root=Path(workspace).resolve();self.second=Path(second).resolve()
        if self.root==self.second or self.root in self.second.parents or self.second in self.root.parents:
            raise ValueError('Backup directories must be separate, not nested')
        for folder in [self.root,self.second]:
            if folder.exists() and any(folder.iterdir()):raise ValueError('Use new empty backup directories')
            folder.mkdir(parents=True,exist_ok=True,mode=0o700);folder.chmod(0o700)
        self.manifest={'format':1,'phase':'backup_in_progress','identity':identity,'files':{},
                       'installation_prepared':False,'native_modules_uploaded':0,
                       'program_writes':0,'settings_writes':0,'wbi_writes':0,
                       'copies_on_distinct_filesystems':self.root.stat().st_dev!=self.second.stat().st_dev}
        self.save()

    def save(self):
        for folder in [self.root,self.second]:
            core.save_json(folder/'backup-manifest.private.json',self.manifest)
            (folder/'backup-manifest.private.json').chmod(0o600)

    def put(self,name,data):
        if Path(name).name!=name:raise ValueError('Flat backup filename required')
        digest=core.sha(data)
        for folder in [self.root,self.second]:
            path=folder/name
            with path.open('xb') as file:
                path.chmod(0o600);file.write(data);file.flush();os.fsync(file.fileno())
            if core.sha(path.read_bytes())!=digest:raise ValueError('Backup readback hash mismatch')
        self.manifest['files'][name]={'bytes':len(data),'sha256':digest}
        self.save()

    def verify(self):
        for name,row in self.manifest['files'].items():
            for folder in [self.root,self.second]:
                path=folder/name
                if path.stat().st_size!=row['bytes'] or core.sha(path.read_bytes())!=row['sha256']:
                    raise ValueError('Backup copy verification failed: '+name)


def backup(workspace,second,identity=None,session_factory=None):
    info=normal_info() if identity is None else identity
    if info['firmware_version']!=core.VERSION:
        raise ValueError('Service backup path validated only for firmware '+core.VERSION)
    copies=Copies(workspace,second,info)
    expected={k:info[k] for k in ['model','serial_sha256']}
    try:
        with BackupReader(expected,session_factory) as reader:
            memory=reader.backup(True)
            copies.put('settings-memory.private.bin',memory)
            records=core.backup_records(memory)
            storage=reader.backup(False)
            copies.put('settings-storage.private.bin',storage)
            stored=core.backup_records(storage)
            for name in ['cpapp-b.bin','appFw.so']:
                print('Reading complete '+name,flush=True)
                copies.put(name+'.bin',reader.program(name))
            for key in core.CP_KEYS:
                data=reader.cp_unit(key);copies.put(f'unit-{key:08x}.bin',data)
                if data!=records[1][key].data:raise ValueError('Native CP unit differs from memory backup')
            for key in core.AP_KEYS:
                data=reader.ap_binding(key);copies.put(f'binding-{key:08x}.bin',data)
                if data!=records[0][key].data:raise ValueError('Native AP binding differs from memory backup')
            copies.manifest['property_counts']={'memory':[len(r) for r in records],
                                                 'storage':[len(r) for r in stored]}
        copies.verify()
        hashes=[copies.manifest['files'][name+'.bin']['sha256'] for name in core.PROGRAMS]
        copies.manifest['programs_match_official_2_01']=hashes==core.ORIGINAL
        copies.manifest['phase']='backups_verified_no_installation'
        copies.save()
        print('Two complete backup copies verified; no installation performed.',flush=True)
    except BaseException as error:
        copies.manifest['phase']='backup_incomplete_needs_review'
        copies.manifest['last_error']=str(error);copies.save();raise
    return copies


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace',type=Path,required=True)
    parser.add_argument('--backup-copy',type=Path,required=True)
    args=parser.parse_args()
    backup(args.workspace,args.backup_copy)


if __name__=='__main__':main()

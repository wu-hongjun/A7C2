// Decompiled pseudocode, not original source.
// REA Evidence: ev_a27f826c523675144a169d606dc772f1ca9cc73020777aa667d063feb55d04af
// Source record: research/evidence/rea/sony-fsys-completion/02-procedure_pseudo_code.json

void _ZN15InfraFsysThread8CallbackEjP14FsysOsalHeaderPvj
               (long param_1,undefined4 param_2,undefined4 *param_3,undefined8 param_4,
               undefined4 param_5)

{
  int iVar1;

  fsysDepTrace_Printf(0x10,"[Thread] Callback (rcv_id = 0x%08X) :",param_2);
  fsysDepTrace_Printf(0x10,"[Thread]  - msg_id      : [0x%08X]",*param_3);
  fsysDepTrace_Printf(0x10,"[Thread]  - version     : [0x%08X]",*(undefined2 *)(param_3 + 1));
  fsysDepTrace_Printf(0x10,"[Thread]  - size        : [0x%08X]",*(undefined2 *)((long)param_3 + 6));
  fsysDepTrace_Printf(0x10,"[Thread]  - err_code    : [0x%08X]",param_3[2]);
  _ZN3MWF12EventWrapper9SetSendIdEj(*(undefined8 *)(param_1 + 8),param_2);
  iVar1 = _ZN3MWF12EventWrapper4SendEPKviPvi
                    (*(undefined8 *)(param_1 + 8),param_4,param_5,param_3,0x18);
  if (iVar1 != 0) {
    fsysDepTrace_Printf(0x11,"[Thread] Send Error(l.%d) [0x%08X]",0x1fe,iVar1);
    fsysDepLog_IfsysError(0x4c,0x1ff,iVar1);
    return;
  }
  fsysDepLog_IfsysSendMsg(*param_3,param_2,param_3[2]);
  return;
}

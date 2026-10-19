// Decompiled pseudocode, not original source.
// REA Evidence: ev_b05ad69d1b8a87531b0ba54b08dc0db39a759a1e5930ee41abdbc41cb9935930
// Source record: research/evidence/rea/sony-fsys-completion/01-procedure_pseudo_code.json

void _ZN9InfraFsys13CallbackPointEPviS0_j
               (uint *param_1,uint param_2,long param_3,undefined4 param_4)

{
  uint uVar1;
  long *plVar2;
  undefined8 uVar3;
  long lVar4;
  undefined1 auStack_48 [16];
  undefined1 auStack_38 [16];
  undefined1 auStack_28 [16];
  undefined1 auStack_18 [24];

  lVar4 = *(long *)(param_3 + 8);
  fsysDepLog_IfsysCallbackIn(*param_1,param_4);
  fsysDepTrace_Printf(0x10,"CallbackPoint :");
  fsysDepTrace_Printf(0x10," - msg_id      : [0x%08X]",*param_1);
  fsysDepTrace_Printf(0x10," - version     : [0x%08X]",(short)param_1[1]);
  fsysDepTrace_Printf(0x10," - size        : [0x%08X]",*(undefined2 *)((long)param_1 + 6));
  fsysDepTrace_Printf(0x10," - err_code    : [0x%08X]",param_1[2]);
  if ((int)param_2 < 0x18) {
    fsysDepTrace_Printf(0x11," Illegal Size [0x%08X]\n",*param_1);
    uVar3 = 0xbe;
  }
  else {
    param_2 = param_2 - 0x18;
    uVar1 = *param_1;
    if (param_2 < *(ushort *)((long)param_1 + 6)) {
      fsysDepTrace_Printf(0x11," Illegal Payload Size [0x%08X]\n");
      uVar3 = 200;
    }
    else {
      if ((uVar1 >> 0x11 & 1) != 0) {
        _ZN3MWF6ObjMsgC1Ejj(auStack_48,0xffff,0x8000003);
        _ZN3MWF17ParamStd_TemplateIjLNS_5Param4TypeE8EEC1Ej(auStack_38,uVar1 & 0xffff);
        _ZN3MWF6ObjMsg8AddParamEjRKNS_5ParamE(auStack_48,0x10002,auStack_38);
        _ZN3MWF17ParamStd_TemplateIjLNS_5Param4TypeE8EED1Ev(auStack_38);
        _ZN3MWF17ParamStd_TemplateIjLNS_5Param4TypeE8EEC1Ej(auStack_28,param_4);
        _ZN3MWF6ObjMsg8AddParamEjRKNS_5ParamE(auStack_48,0x10003,auStack_28);
        _ZN3MWF17ParamStd_TemplateIjLNS_5Param4TypeE8EED1Ev(auStack_28);
        _ZN3MWF6StructC1EPKvii
                  (auStack_18,param_1 + 6,*(undefined2 *)((long)param_1 + 6),(short)param_1[1]);
        _ZN3MWF6ObjMsg8AddParamEjRKNS_5ParamE(auStack_48,0x10004,auStack_18);
        _ZN3MWF6StructD1Ev(auStack_18);
        plVar2 = *(long **)(lVar4 + 0x18);
        (**(code **)(*plVar2 + 0x18))(plVar2,auStack_48);
        _ZN3MWF6ObjMsgD1Ev(auStack_48);
        goto LAB_001268d8;
      }
      if ((uVar1 >> 0x10 & 1) != 0) {
        uVar1 = _ZN15InfraFsysThread15DispatchCommandEjjjPv
                          (lVar4,uVar1 & 0xffff,(short)param_1[1],*(ushort *)((long)param_1 + 6),
                           param_1 + 6);
        param_1[2] = uVar1;
        goto LAB_001268d8;
      }
      fsysDepTrace_Printf(0x11," Illegal msg_id[0x%08X]\n");
      uVar3 = 0xe3;
      param_2 = *param_1;
    }
  }
  fsysDepLog_IfsysError(0x46,uVar3,param_2);
LAB_001268d8:
  fsysDepLog_IfsysCallbackOut(*param_1,param_4,param_1[2]);
  return;
}

// Decompiled pseudocode, not original source.
// REA Evidence: ev_295177da68a680848e3b744bdc5d88610f1a59d2651fdfe24d8124bc1c083da0
// Source record: research/evidence/rea/dataflow-encoder-envelope/02-procedure_pseudo_code.json

undefined8 _ZN16DataflowInfraMsg12SetGlobalMsgEPvt(long param_1,void *param_2,ushort param_3)

{
  undefined8 uVar1;
  
  if (*(int *)(param_1 + 8) == 0) {
    if (param_3 < 0x180) {
      *(ushort *)(param_1 + 0xc) = param_3;
      *(undefined4 *)(param_1 + 8) = 2;
      memcpy((void *)(param_1 + 0x18),param_2,(ulong)param_3);
      uVar1 = 0;
    }
    else {
      _ZN16DataflowInfraLog14Seterr_generalEjji(0x2047534d,0x77,0xffffffe4);
      uVar1 = 0xffffffe4;
    }
  }
  else {
    _ZN16DataflowInfraLog14Seterr_generalEjji(0x2047534d,0x71,0xffffffe3);
    uVar1 = 0xffffffe3;
  }
  return uVar1;
}


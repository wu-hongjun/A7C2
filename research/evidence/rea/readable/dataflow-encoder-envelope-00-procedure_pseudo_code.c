// Decompiled pseudocode, not original source.
// REA Evidence: ev_dadc5fd91565148f5318bed47ed290123ad51c4ebde8d608f15a72472d89cc77
// Source record: research/evidence/rea/dataflow-encoder-envelope/00-procedure_pseudo_code.json

undefined8 _ZN16DataflowInfraMsg14SetDataflowMsgEPvt(long param_1,void *param_2,ushort param_3)

{
  int iVar1;
  ushort uVar2;
  bool bVar3;
  undefined8 uVar4;
  
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 2) {
    _ZN16DataflowInfraLog14Seterr_generalEjji(0x2047534d,0x3b,0xffffffe2);
    uVar4 = 0xffffffe2;
  }
  else if ((iVar1 == 1) && (*(int *)(param_1 + 0x24) != 0x18)) {
    _ZN16DataflowInfraLog14Seterr_generalEjji(0x2047534d,0x43,0xffffffe3);
    uVar4 = 0xffffffe3;
  }
  else {
    bVar3 = *(ushort *)((long)param_2 + 4) != param_3;
    if ((!bVar3 && 6 < param_3) && (bVar3 || param_3 != 7)) {
      if (iVar1 == 0) {
        _ZN16DataflowInfraMsg18MakeDataflowHeaderEv(param_1);
        *(undefined4 *)(param_1 + 8) = 1;
      }
      uVar2 = *(ushort *)((long)param_2 + 4);
      if ((uint)*(ushort *)(param_1 + 0x22) + (uint)uVar2 < 0x180) {
        *(ushort *)(param_1 + 0x22) = param_3 + *(ushort *)(param_1 + 0x22);
        *(short *)(param_1 + 0x20) = *(short *)(param_1 + 0x20) + 1;
        memcpy((void *)(param_1 + 0x18 + (ulong)*(uint *)(param_1 + 0x28)),param_2,(ulong)uVar2);
        *(uint *)(param_1 + 0x28) = *(int *)(param_1 + 0x28) + (uint)*(ushort *)((long)param_2 + 4);
        uVar4 = 0;
      }
      else {
        _ZN16DataflowInfraLog14Seterr_generalEjji(0x2047534d,0x56,0xffffffe4);
        uVar4 = 0xffffffe4;
      }
    }
    else {
      _ZN16DataflowInfraLog14Seterr_generalEjji(0x2047534d,0x4a,0xffffffe0);
      uVar4 = 0xffffffe0;
    }
  }
  return uVar4;
}


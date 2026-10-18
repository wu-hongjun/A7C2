// Decompiled pseudocode, not original source.
// REA Evidence: ev_69d142f44b1028a6eac4edfd1475251ad93e671cec5f00214ada223a3610a303
// Source record: research/evidence/rea/dataflow-encoder-envelope/01-procedure_pseudo_code.json

undefined8 _ZN18DataflowInfraMsgEx14SetDataflowMsgEPvt(long param_1,void *param_2,ushort param_3)

{
  int iVar1;
  ushort uVar2;
  bool bVar3;
  undefined8 uVar4;
  long lVar5;
  long lVar6;
  
  lVar6 = *(long *)(param_1 + 0x18);
  if (lVar6 == 0) {
    _ZN16DataflowInfraLog14Seterr_generalEjji(0x5847534d,0x42,0xffffffec);
    uVar4 = 0xffffffec;
  }
  else {
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 == 2) {
      _ZN16DataflowInfraLog14Seterr_generalEjji(0x5847534d,0x4b,0xffffffe2);
      uVar4 = 0xffffffe2;
    }
    else if ((iVar1 == 1) && (*(int *)(lVar6 + 0xc) != 0x18)) {
      _ZN16DataflowInfraLog14Seterr_generalEjji(0x5847534d,0x53,0xffffffe3);
      uVar4 = 0xffffffe3;
    }
    else {
      bVar3 = *(ushort *)((long)param_2 + 4) != param_3;
      if ((!bVar3 && 6 < param_3) && (bVar3 || param_3 != 7)) {
        if (iVar1 == 0) {
          _ZN18DataflowInfraMsgEx18MakeDataflowHeaderEv(param_1);
          *(undefined4 *)(param_1 + 8) = 1;
        }
        uVar2 = *(ushort *)((long)param_2 + 4);
        if ((uint)*(ushort *)(lVar6 + 10) + (uint)uVar2 < 0x1e0) {
          lVar5 = *(long *)(param_1 + 0x18);
          *(short *)(lVar6 + 8) = *(short *)(lVar6 + 8) + 1;
          *(ushort *)(lVar6 + 10) = param_3 + *(ushort *)(lVar6 + 10);
          memcpy((void *)(lVar5 + (ulong)*(uint *)(lVar6 + 0x10)),param_2,(ulong)uVar2);
          *(uint *)(lVar6 + 0x10) = *(int *)(lVar6 + 0x10) + (uint)*(ushort *)((long)param_2 + 4);
          uVar4 = 0;
        }
        else {
          _ZN16DataflowInfraLog14Seterr_generalEjji(0x5847534d,0x66,0xffffffe4);
          uVar4 = 0xffffffe4;
        }
      }
      else {
        _ZN16DataflowInfraLog14Seterr_generalEjji(0x5847534d,0x5a,0xffffffe0);
        uVar4 = 0xffffffe0;
      }
    }
  }
  return uVar4;
}


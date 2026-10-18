// Decompiled pseudocode, not original source.
// REA Evidence: ev_c6de653616d300206cab68346cd78c7d7f84b1e571dbb09f77bc645f634b0f4f
// Source record: research/evidence/rea/sdyc-physical-map/01-procedure_pseudo_code.json

int _ZN12AvbbcPhycMemD2Ev(undefined8 *param_1)

{
  int iVar1;
  int *piVar2;
  
  *param_1 = 0x2b2a70;
  iVar1 = munmap((void *)param_1[3],(ulong)*(uint *)(param_1 + 5));
  if (iVar1 != 0) {
    piVar2 = __errno_location();
    _ZN17AvbbcBaseClassLog7SetLog0Ejjjjt
              (0x201e0a8,*(undefined4 *)(param_1 + 6),*piVar2,*(undefined4 *)(param_1 + 1),0x5c);
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz(0x55,6,3,0x5d,"[DummyMPR]Memory UnMapping Error!!");
    }
  }
  if (*(int *)(param_1 + 1) != -1) {
    iVar1 = close(*(int *)(param_1 + 1));
    return iVar1;
  }
  return -1;
}


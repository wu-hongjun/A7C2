// Decompiled pseudocode, not original source.
// REA Evidence: ev_858a6d761c484d9fcdf9cecff1174bdc635e4473e6bd95d0c0c46efbd93788e1
// Source record: research/evidence/rea/libmpr-still/02-procedure_pseudo_code.json

undefined8
_ZN3mpr24SetStillRecParamInternalERKNS_13stillRecParamEbb(void *param_1,char param_2,char param_3)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 local_8;
  undefined2 local_6;
  undefined1 local_4;
  undefined1 local_3;
  undefined1 local_2;
  undefined1 local_1;
  
  pthread_rwlock_wrlock((pthread_rwlock_t *)&DAT_0048f6d0);
  cVar1 = _ZN3mpr28CheckStillRecParamDifferenceEPKNS_13stillRecParamE(param_1);
  if (cVar1 == '\0' && param_3 == '\0') {
    local_3 = 0;
    local_4 = 4;
    local_2 = 0;
    local_8 = 0x21c0;
    local_1 = 0;
    local_6 = 0x101;
    FUN_001f29f4(&local_8,8);
    _ZN3mpr6thread6rwLock6unlockEv(&DAT_0048f6c8);
  }
  else {
    memcpy(&DAT_0048f418,param_1,0x2a8);
    if (*(long *)((long)param_1 + 0x1e0) != 0) {
      strncpy(&DAT_00481a38,DAT_0048f5f8,0xcfff);
      DAT_0048f5f8 = &DAT_00481a38;
    }
    if (*(long *)((long)param_1 + 0x248) != 0) {
      strncpy(&DAT_0048ea38,DAT_0048f660,0x7ff);
      DAT_0048f660 = &DAT_0048ea38;
    }
    iVar4 = _ZN3mpr8msidconv9GetSlotIDEj(&_ZN3mpr10g_msidconvE,DAT_0048f4e8);
    iVar5 = _ZN3mpr8msidconv9GetSlotIDEj(&_ZN3mpr10g_msidconvE,DAT_0048f4ec);
    if (((iVar4 == -1) ||
        (((iVar2 = FUN_00234978(iVar4), iVar5 == -1 || (iVar3 = FUN_00234978(iVar5), iVar3 == 0)) &&
         (iVar2 == 0)))) &&
       ((cVar1 = _ZN3mpr11IsFifoEmptyEv(), cVar1 != '\0' && (DAT_0048f6c0 != '\0')))) {
      if (iVar4 != -1) {
        FUN_0022aa98(iVar4,1);
      }
      if (iVar5 != -1) {
        FUN_0022aa98(iVar5,1);
      }
      DAT_0048f6c0 = '\0';
    }
    _ZN3mpr6thread6rwLock6unlockEv(&DAT_0048f6c8);
    if (param_3 != '\0') {
      if (iVar4 != -1) {
        FUN_0022ab40(iVar4);
      }
      if (iVar5 != -1) {
        FUN_0022ab40(iVar5);
      }
    }
    if (param_2 != '\0') {
      _ZN3mpr8capidtbl14RemakeCapIdTblEPN9DefStruct9LocalTimeE(0);
    }
    FUN_0022fe1c(0);
  }
  return 0;
}


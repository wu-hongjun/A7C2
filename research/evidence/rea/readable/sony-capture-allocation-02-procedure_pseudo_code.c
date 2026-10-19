// Decompiled pseudocode, not original source.
// REA Evidence: ev_abb833323aa7226c9b285505a616b076590324754e3424dbac4637b8f2a471d6
// Source record: research/evidence/rea/sony-capture-allocation/02-procedure_pseudo_code.json

char * _ZN3mpr11GetFilePathEPNS_18recordStillParam_tEbj(long param_1,char param_2,ulong param_3)

{
  char *__string;
  int iVar1;
  size_t sVar2;
  char *pcVar3;
  long lVar4;
  long lVar5;

  if (param_2 == '\0') {
    iVar1 = *(int *)(param_1 + (param_3 & 0xffffffff) * 0x588 + 500);
    if (iVar1 == 2) {
      pcVar3 = "ARW";
    }
    else if (iVar1 == 1) {
      pcVar3 = "JPG";
    }
    else {
      pcVar3 = "HIF";
      if (iVar1 != 4) {
        pcVar3 = "XXX";
      }
    }
    lVar4 = 0;
    lVar5 = (param_3 & 0xffffffff) * 0x588 + 0x23c;
    do {
      __string = (char *)(param_1 + lVar5 + lVar4);
      sVar2 = strnlen(__string,0x105);
      if ((0xb < (uint)sVar2) &&
         (iVar1 = strncmp(pcVar3,(char *)(param_1 + (ulong)((uint)sVar2 - 4) + lVar4 + lVar5),3),
         iVar1 != 0)) {
        return __string;
      }
      lVar4 = lVar4 + 0x106;
    } while (lVar4 != 0x418);
    pcVar3 = (char *)0x0;
  }
  else {
    if (*(char *)(param_1 + 0x676) == '\0') {
      if (*(char *)(param_1 + 0xbfe) == '\0') {
        return (char *)0x0;
      }
      lVar4 = 1;
    }
    else {
      lVar4 = 0;
    }
    pcVar3 = (char *)(param_1 + lVar4 * 0x588 + 0x676);
  }
  return pcVar3;
}

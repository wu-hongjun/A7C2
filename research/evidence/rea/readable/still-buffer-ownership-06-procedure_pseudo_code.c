// Decompiled pseudocode, not original source.
// REA Evidence: ev_495cd0de50051fdea1066175f2f4efcf5dd1b3390633cc8f48b95bdf7e076030
// Source record: research/evidence/rea/still-buffer-ownership/06-procedure_pseudo_code.json

void _ZN3mpr11stillrecord17ReleaseStillCacheEiPNS_18recordStillParam_tE(int param_1,long param_2)

{
  long lVar1;
  char *pcVar2;
  ulong uVar3;
  undefined2 local_10;
  undefined2 local_e;
  undefined1 local_c;
  undefined1 local_b;
  undefined1 local_a;
  undefined1 local_9;
  undefined4 local_8;
  
  if (*(int *)(param_2 + 0xc) != 3) {
    return;
  }
  lVar1 = param_2 + (long)param_1 * 0x588;
  if (*(int *)(lVar1 + 0x20c) == -1) {
    local_c = 3;
    local_b = 0;
    local_10 = 0x2120;
    local_a = 0;
    local_9 = 0;
    local_e = 0x9b01;
    local_8 = osal_thread_id();
    FUN_001df8d4(&local_10,0xc);
  }
  else {
    uVar3 = 0;
    pcVar2 = (char *)(param_2 + (long)param_1 * 0x588 + 0x23c);
    do {
      if (((*pcVar2 != '\0') && (*(char *)(param_2 + (long)param_1 * 0x588 + uVar3 + 0x238) != '\0')
          ) && (*(char *)(lVar1 + 0x674) != '\0')) {
        FUN_002356c0(pcVar2,uVar3 & 0xffffffff);
      }
      uVar3 = uVar3 + 1;
      pcVar2 = pcVar2 + 0x106;
    } while (uVar3 != 4);
  }
  return;
}


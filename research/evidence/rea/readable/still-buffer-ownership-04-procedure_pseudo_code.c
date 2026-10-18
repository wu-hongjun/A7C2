// Decompiled pseudocode, not original source.
// REA Evidence: ev_2974f7138f8ab1f3259edc0ea78be8adb8e21ac99d93ef009a786c8bef20eac9
// Source record: research/evidence/rea/still-buffer-ownership/04-procedure_pseudo_code.json

void _ZN3mpr11stillrecord19FreeStillInfoMemoryEibPNS_18recordStillParam_tEPN5still8fin_if_tEbb
               (int param_1,char param_2,long param_3,long param_4,char param_5,char param_6)

{
  long lVar1;
  uint uVar2;
  ulong uVar3;
  undefined2 local_30;
  undefined2 local_2e;
  undefined1 local_2c;
  undefined1 local_2b;
  undefined1 local_2a;
  undefined1 local_29;
  undefined4 local_28;
  long local_20 [4];
  
  param_3 = param_3 + (long)param_1 * 0x588;
  local_20[0] = 0;
  local_20[1] = 0;
  local_20[2] = 0;
  local_20[3] = 0;
  if (*(int *)(param_3 + 0x20c) == -1) {
    local_2b = 0;
    local_2a = 0;
    local_2c = 3;
    local_29 = 0;
    local_30 = 0x2120;
    local_2e = 0x8501;
    local_28 = osal_thread_id();
    FUN_001df8d4(&local_30,0xc);
  }
  else if (param_2 == '\0') {
    uVar2 = *(uint *)(param_3 + 500);
    lVar1 = param_4 + 0xc0;
    if (uVar2 == 1) {
      local_20[2] = lVar1;
      local_20[0] = param_4;
      local_20[1] = param_4 + 0x30;
      uVar2 = 3;
      if (param_5 != '\0') {
        uVar2 = 4;
        local_20[3] = param_4 + 0x120;
      }
    }
    else if (uVar2 == 2) {
      local_20[0] = param_4;
      if (param_6 == '\0') {
        local_20[1] = lVar1;
      }
      else {
        if (param_5 == '\0') {
          local_20[1] = param_4 + 0xf0;
        }
        else {
          local_20[1] = param_4 + 0x120;
        }
        uVar2 = 3;
        local_20[2] = lVar1;
      }
    }
    else {
      if (uVar2 != 4) {
                    /* WARNING: Subroutine does not return */
        __assert_fail("0","integRecordStill.cpp",0xd92,
                      "void mpr::stillrecord::FreeStillInfoMemory(int, bool, mpr::recordStillParam_t*, still::fin_if_t*, bool, bool)"
                     );
      }
      local_20[3] = lVar1;
      local_20[0] = param_4;
      local_20[1] = param_4 + 0x60;
      local_20[2] = param_4 + 0x90;
    }
    uVar3 = 0;
    local_20[0] = param_4;
    do {
      if (((int *)local_20[uVar3] != (int *)0x0) && (*(int *)local_20[uVar3] != 0)) {
        FUN_003b24f0();
      }
      uVar3 = uVar3 + 1;
    } while (uVar2 != uVar3);
  }
  return;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_8534186136884122e8bd5b57489c2b1f946a79e8b3e5164c9cff495cf7e6a359
// Source record: research/evidence/rea/libmpr-extra/02-procedure_pseudo_code.json

void FUN_0027e3ac(undefined8 param_1,undefined8 param_2,uint param_3,long param_4)

{
  char *pcVar1;
  long lVar2;
  undefined2 local_48;
  undefined2 local_46;
  undefined1 local_44;
  undefined1 local_43;
  undefined1 local_42;
  undefined1 local_41;
  char local_40 [8];
  ulong local_38;
  undefined8 local_30;
  undefined8 local_28;
  undefined8 local_20;
  undefined8 local_18;
  undefined8 local_10;
  undefined8 uStack_8;
  
  local_38 = 0;
  builtin_strncpy(local_40,"SONYBCUB",8);
  local_30 = 0;
  local_28 = 0;
  local_20 = 0;
  local_18 = 0;
  local_10 = 0;
  uStack_8 = 0;
  FUN_0027e368();
  lVar2 = FUN_0027daec();
  local_38 = lVar2 << 0x20 | 1;
  pcVar1 = (char *)(param_4 + (ulong)param_3);
  local_28 = 0;
  local_18 = 0;
  local_10 = 0;
  local_30 = 0x3000000010000;
  local_43 = 0;
  local_20 = 0x1100000000000000;
  local_42 = 0;
  *(char (*) [8])pcVar1 = local_40;
  *(ulong *)(pcVar1 + 8) = local_38;
  local_41 = 0;
  pcVar1[0x10] = '\0';
  pcVar1[0x11] = '\0';
  pcVar1[0x12] = '\x01';
  pcVar1[0x13] = '\0';
  pcVar1[0x14] = '\0';
  pcVar1[0x15] = '\0';
  pcVar1[0x16] = '\x03';
  pcVar1[0x17] = '\0';
  pcVar1[0x18] = '\0';
  pcVar1[0x19] = '\0';
  pcVar1[0x1a] = '\0';
  pcVar1[0x1b] = '\0';
  pcVar1[0x1c] = '\0';
  pcVar1[0x1d] = '\0';
  pcVar1[0x1e] = '\0';
  pcVar1[0x1f] = '\0';
  pcVar1[0x20] = '\0';
  pcVar1[0x21] = '\0';
  pcVar1[0x22] = '\0';
  pcVar1[0x23] = '\0';
  pcVar1[0x24] = '\0';
  pcVar1[0x25] = '\0';
  pcVar1[0x26] = '\0';
  pcVar1[0x27] = '\x11';
  pcVar1[0x28] = '\0';
  pcVar1[0x29] = '\0';
  pcVar1[0x2a] = '\0';
  pcVar1[0x2b] = '\0';
  pcVar1[0x2c] = '\0';
  pcVar1[0x2d] = '\0';
  pcVar1[0x2e] = '\0';
  pcVar1[0x2f] = '\0';
  pcVar1[0x30] = '\0';
  pcVar1[0x31] = '\0';
  pcVar1[0x32] = '\0';
  pcVar1[0x33] = '\0';
  pcVar1[0x34] = '\0';
  pcVar1[0x35] = '\0';
  pcVar1[0x36] = '\0';
  pcVar1[0x37] = '\0';
  *(undefined8 *)(pcVar1 + 0x38) = uStack_8;
  local_44 = 4;
  local_48 = 0x2304;
  local_46 = 0x20;
  FUN_0027db18(&local_48,8);
  return;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_6bbace4c374e9ee25126761fa24e43f574778f9017ee50de98a3ecf0d516a358
// Source record: research/evidence/rea/hq-lut/03-procedure_pseudo_code.json

void FUN_037f9d00(undefined4 *param_1,int param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined1 auStack_120 [24];
  undefined1 auStack_108 [24];
  undefined1 auStack_f0 [24];
  undefined1 auStack_d8 [24];
  undefined1 auStack_c0 [24];
  undefined1 auStack_a8 [24];
  undefined1 auStack_90 [24];
  undefined1 auStack_78 [24];
  undefined1 auStack_60 [24];
  undefined1 auStack_48 [24];
  undefined1 auStack_30 [24];
  undefined1 auStack_18 [24];
  
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  *(undefined8 *)(param_1 + 0xc) = 0;
  *(undefined8 *)(param_1 + 0xe) = 0;
  *(undefined8 *)(param_1 + 0x10) = 0;
  *(undefined8 *)(param_1 + 0x12) = 0;
  *(undefined8 *)(param_1 + 0x14) = 0;
  *(undefined8 *)(param_1 + 0x16) = 0;
  if (param_2 - 4U < 2) {
    param_1[1] = 5;
    *param_1 = 0x100;
    FUN_037f94e8(auStack_108,2,auStack_30);
    FUN_037f9ca0(auStack_120,0x100,auStack_108,auStack_18);
    FUN_037f9594(param_1 + 0xc,auStack_120);
    FUN_037f6778(auStack_120);
    puVar2 = auStack_a8;
    FUN_037f7580(auStack_108);
    FUN_037f94e8(puVar2,2,auStack_60);
    puVar3 = auStack_c0;
    FUN_037f9ca0(puVar3,0x3c,puVar2,auStack_48);
    puVar1 = auStack_d8;
    FUN_037f9b78(puVar1,5,puVar3,auStack_30);
    FUN_037f9adc(auStack_f0,0x14,puVar1,auStack_18);
    FUN_037f9680(param_1 + 0x12,auStack_f0);
    FUN_037f95fc(auStack_f0);
  }
  else {
    if (param_2 != 6) {
      return;
    }
    puVar3 = auStack_30;
    *param_1 = 0x100;
    param_1[1] = 6;
    FUN_037f94e8(auStack_78,2,puVar3);
    puVar2 = auStack_18;
    FUN_037f9ca0(auStack_90,0x100,auStack_78,puVar2);
    FUN_037f9594(param_1 + 0xc,auStack_90);
    FUN_037f6778(auStack_90);
    FUN_037f7580(auStack_78);
    FUN_037f94e8(puVar2,2,auStack_c0);
    FUN_037f9ca0(puVar3,0x3c,puVar2,auStack_a8);
    puVar1 = auStack_48;
    FUN_037f9b78(puVar1,5,puVar3,auStack_90);
    FUN_037f9adc(auStack_60,0x14,puVar1,auStack_78);
    FUN_037f9680(param_1 + 0x12,auStack_60);
    FUN_037f95fc(auStack_60);
  }
  FUN_037f67b4(puVar1);
  FUN_037f6778(puVar3);
  FUN_037f7580(puVar2);
  param_1[6] = 1;
  param_1[7] = 4;
  param_1[9] = 1;
  param_1[8] = 10;
  param_1[0xb] = 1;
  param_1[10] = 0x19;
  return;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_4fcdff2ccac82926b7d3dd61c04a9d8b81ccb8c63da315024359b3f4e9274630
// Source record: research/evidence/rea/hq-lut/08-procedure_pseudo_code.json

void FUN_037f8990(long param_1,undefined8 *param_2,long param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  int iVar4;
  uint uVar5;
  undefined1 uVar6;
  undefined4 *puVar7;
  undefined4 local_20;
  uint uStack_1c;
  long local_18;
  undefined8 local_10;
  long local_8;
  
  if (*(int *)(param_1 + 0x48) - 4U < 3) {
    puVar3 = (undefined4 *)param_2[1];
    for (puVar7 = (undefined4 *)*param_2; puVar3 != puVar7; puVar7 = puVar7 + 0x3c) {
      iVar4 = puVar7[10];
      uVar5 = puVar7[0x15];
      local_20 = puVar7[0x14];
      uVar6 = *(undefined1 *)((long)puVar7 + 0x5b);
      uVar1 = *puVar7;
      uVar2 = puVar7[1];
      local_18 = 0;
      local_10 = 0;
      local_8 = 0;
      uStack_1c = uVar5;
      local_18 = _Znwm(8);
      local_8 = local_18 + 8;
      local_10 = FUN_037f7888(&local_20,&local_18,local_18);
      if (*(long *)(param_1 + 0x20) == *(long *)(param_1 + 0x28)) {
        FUN_037f7744(param_1 + 0x18,&local_18);
      }
      else {
        FUN_037f7514(*(long *)(param_1 + 0x20),&local_18);
        *(long *)(param_1 + 0x20) = *(long *)(param_1 + 0x20) + 0x18;
      }
      FUN_037f7580(&local_18);
      if ((iVar4 == 2) && (uVar5 < 2)) {
        FUN_037f8578(param_1,puVar7 + 0x36,param_3 + 0xf0,param_3 + 0x108,uVar1,uVar2,uVar6);
      }
    }
    return;
  }
  return;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_b8f34bf016b037487997ad8e1b6f62ba862c9e176f82dc5d9a44a0922da7a3d3
// Source record: research/evidence/rea/hq-lut/02-procedure_pseudo_code.json

undefined8 FUN_037f7c34(undefined8 param_1,long param_2)

{
  long lVar1;
  long lVar2;
  long lVar3;
  undefined1 auStack_68 [4];
  undefined4 local_64;
  undefined1 auStack_60 [24];
  long local_48;
  long local_40;
  undefined8 local_38;
  long local_30;
  long local_28;
  long local_20;
  undefined8 local_18;
  undefined8 local_10;
  undefined8 local_8;
  
  if (*(int *)(param_2 + 0x48) - 4U < 3) {
    lVar3 = (long)*(int *)(param_2 + 0x58);
    local_64 = 0;
    FUN_037f7658(auStack_60,1,&local_64,auStack_68);
    local_48 = 0;
    local_40 = 0;
    local_38 = 0;
    FUN_037f76b8(&local_48,lVar3);
    lVar2 = local_48;
    for (; lVar3 != 0; lVar3 = lVar3 + -1) {
      if (lVar2 != 0) {
        FUN_037f66a4(lVar2,auStack_60);
      }
      lVar2 = lVar2 + 0x18;
    }
    local_30 = 0;
    local_28 = 0;
    local_20 = 0;
    local_40 = lVar2;
    local_30 = _Znwm(0x18);
    lVar2 = local_30 + 0x18;
    local_28 = local_30;
    local_20 = lVar2;
    FUN_037f6810(local_30,&local_48);
    lVar1 = local_20;
    lVar3 = local_30;
    local_18 = *(undefined8 *)(param_2 + 0x30);
    local_28 = 0;
    local_10 = *(undefined8 *)(param_2 + 0x38);
    *(long *)(param_2 + 0x38) = lVar2;
    local_8 = *(undefined8 *)(param_2 + 0x40);
    local_30 = 0;
    *(long *)(param_2 + 0x30) = lVar3;
    local_20 = 0;
    *(long *)(param_2 + 0x40) = lVar1;
    FUN_037f67b4(&local_18);
    FUN_037f67b4(&local_30);
    FUN_037f6778(&local_48);
    FUN_037f7580(auStack_60);
    FUN_037f78c8(**(undefined8 **)(param_2 + 0x30),&DAT_052a0590,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0xa8,&DAT_052a0598,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0x18,&DAT_052a04d8,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0x30,&DAT_052a04e0,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0x90,&DAT_052a04e8,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0x60,&DAT_052a04f0,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0x78,&DAT_052a04f8,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0xc0,&DAT_052a0500,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0x48,&DAT_052a0508,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0xf0,&DAT_052a0510,1);
    FUN_037f78c8(**(long **)(param_2 + 0x30) + 0xd8,&DAT_052a0518,1);
  }
  FUN_037f7b38(param_1,param_2 + 0x30);
  return param_1;
}


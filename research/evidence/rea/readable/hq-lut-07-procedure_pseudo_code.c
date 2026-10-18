// Decompiled pseudocode, not original source.
// REA Evidence: ev_4494cd5e027c7d2daf1c2e1c351b746add3e1bdc02013fde3a1de06b0eadc4dd
// Source record: research/evidence/rea/hq-lut/07-procedure_pseudo_code.json

void FUN_03802404(long *param_1,undefined8 param_2,undefined8 param_3,long *param_4,
                 undefined8 param_5,int param_6)

{
  long lVar1;
  long lVar2;
  long lVar3;
  long lVar4;
  long lVar5;
  undefined1 auStack_80 [8];
  undefined1 auStack_78 [8];
  undefined2 *local_70;
  undefined2 *puStack_68;
  undefined2 *local_60;
  long local_58;
  long local_50;
  long local_48;
  undefined1 *local_40;
  undefined1 *local_38;
  undefined1 *local_30;
  long local_20;
  long local_18;
  long local_10;
  
  if (param_6 - 4U < 3) {
    local_70 = (undefined2 *)0x0;
    puStack_68 = (undefined2 *)0x0;
    local_60 = (undefined2 *)0x0;
    local_70 = (undefined2 *)_Znwm(2);
    puStack_68 = local_70 + 1;
    *local_70 = 0;
    local_58 = 0;
    local_50 = 0;
    local_48 = 0;
    local_60 = puStack_68;
    lVar1 = _Znwm(48000);
    lVar4 = lVar1 + 48000;
    local_58 = lVar1;
    local_50 = lVar1;
    local_48 = lVar4;
    do {
      if (lVar1 != 0) {
        FUN_031558c4(lVar1,&local_70);
      }
      lVar1 = lVar1 + 0x18;
    } while (lVar4 != lVar1);
    local_40 = (undefined1 *)0x0;
    local_38 = (undefined1 *)0x0;
    local_30 = (undefined1 *)0x0;
    local_50 = lVar4;
    local_40 = (undefined1 *)_Znwm(0x240);
    local_30 = local_40 + 0x240;
    local_38 = local_40;
    lVar1 = FUN_037fd7c8(local_40,0x18,&local_58);
    local_20 = *param_4;
    local_38 = (undefined1 *)0x0;
    local_18 = param_4[1];
    param_4[1] = lVar1;
    local_10 = param_4[2];
    param_4[2] = (long)local_30;
    *param_4 = (long)local_40;
    local_30 = (undefined1 *)0x0;
    local_40 = (undefined1 *)0x0;
    FUN_037f045c(&local_20);
    FUN_037f045c(&local_40);
    FUN_037f03cc(&local_58);
    if (local_70 != (undefined2 *)0x0) {
      _ZdlPv();
    }
    FUN_037edff8(&local_40,3,auStack_80);
    FUN_037eef7c(&local_20,8,&local_40,auStack_78);
    FUN_037eeed0(&local_58,2000,&local_20,&local_70);
    FUN_037edb7c(&local_20);
    FUN_037edb6c(&local_40);
    FUN_03800cc0(&local_40,&DAT_04da2010);
    lVar1 = 0;
    do {
      lVar4 = 0;
      do {
        lVar5 = 0;
        lVar3 = (ulong)(uint)((int)lVar1 * 2 + (int)lVar4) * 0x18;
        do {
          FUN_037fef88(&local_20,
                       (int)((*(float *)(*(long *)(*(long *)(*param_1 + lVar5) + lVar4 * 0x18) +
                                        lVar1) + 5e-07) * 8192.0 + 0.5),4);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEOS4_(&local_40,&local_20);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_20);
          lVar2 = lVar5 * 2;
          lVar5 = lVar5 + 0x18;
          **(undefined1 **)(*(long *)(*param_4 + lVar3) + lVar2) = local_40[2];
          *(undefined1 *)(*(long *)(*(long *)(*param_4 + lVar3) + lVar2) + 1) = local_40[3];
          **(undefined1 **)(*(long *)(*param_4 + lVar3) + lVar2 + 0x18) = *local_40;
          *(undefined1 *)(*(long *)(*(long *)(*param_4 + lVar3) + lVar2 + 0x18) + 1) = local_40[1];
        } while (lVar5 != 24000);
        lVar4 = lVar4 + 1;
      } while (lVar4 != 8);
      lVar1 = lVar1 + 4;
    } while (lVar1 != 0xc);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_40);
    FUN_037edbd8(&local_58);
    return;
  }
  return;
}


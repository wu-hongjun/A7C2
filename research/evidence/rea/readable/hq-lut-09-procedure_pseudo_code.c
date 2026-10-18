// Decompiled pseudocode, not original source.
// REA Evidence: ev_55fcabaa6f0c7d793f5ddaaeb86a74e48e66914b8f3002f1b9523f024696c2b7
// Source record: research/evidence/rea/hq-lut/09-procedure_pseudo_code.json

void FUN_037f8578(undefined8 param_1,long *param_2,ulong *param_3,long *param_4,int param_5,
                 int param_6,int param_7)

{
  long lVar1;
  long lVar2;
  undefined8 uVar3;
  ulong *puVar4;
  int *piVar5;
  long lVar6;
  ulong uVar7;
  long lVar8;
  long lVar9;
  ulong uVar10;
  ulong uVar11;
  ulong uVar12;
  long local_78;
  long local_70;
  undefined8 local_68;
  long local_60;
  long local_58;
  undefined8 local_50;
  long local_48;
  long local_40;
  undefined8 local_38;
  ulong local_30;
  ulong local_28;
  ulong local_20;
  ulong local_18;
  undefined8 local_10;
  ulong local_8;
  
  lVar9 = (long)param_6;
  uVar10 = (ulong)param_5;
  local_78 = 0;
  local_70 = 0;
  local_68 = 0;
  FUN_037edfa8(&local_78,lVar9);
  for (lVar2 = 0; lVar9 != lVar2; lVar2 = lVar2 + 1) {
    *(undefined4 *)(local_78 + lVar2 * 4) = 0;
  }
  local_70 = local_78 + lVar9 * 4;
  local_60 = 0;
  local_58 = 0;
  local_50 = 0;
  FUN_037eddf4(&local_60,uVar10);
  lVar2 = local_60;
  for (uVar11 = uVar10; uVar11 != 0; uVar11 = uVar11 - 1) {
    if (lVar2 != 0) {
      FUN_037eeab4(lVar2,&local_78);
    }
    lVar2 = lVar2 + 0x18;
  }
  local_48 = 0;
  local_40 = 0;
  local_38 = 0;
  local_58 = lVar2;
  FUN_037ede50(&local_48,uVar10);
  lVar2 = local_48;
  for (uVar11 = uVar10; uVar11 != 0; uVar11 = uVar11 - 1) {
    if (lVar2 != 0) {
      FUN_037eeafc(lVar2,&local_60);
    }
    lVar2 = lVar2 + 0x18;
  }
  uVar11 = 0;
  local_30 = 0;
  local_28 = 0;
  local_20 = 0;
  local_40 = lVar2;
  if (uVar10 != 0) {
    if (0xaaaaaaaaaaaaaaa < uVar10) {
      uVar3 = _ZSt17__throw_bad_allocv();
      FUN_037edbd8(&local_48);
      FUN_037edb7c(&local_60);
      FUN_037edb6c(&local_78);
                    /* WARNING: Subroutine does not return */
      _Unwind_Resume(uVar3);
    }
    uVar11 = _Znwm((long)param_5 * 0x18);
  }
  local_20 = uVar11 + (long)param_5 * 0x18;
  local_30 = uVar11;
  local_28 = uVar11;
  uVar11 = FUN_037f844c(uVar11,uVar10,&local_48);
  local_18 = *param_3;
  local_28 = 0;
  local_10 = param_3[1];
  param_3[1] = uVar11;
  local_8 = param_3[2];
  param_3[2] = local_20;
  *param_3 = local_30;
  local_20 = 0;
  local_30 = 0;
  FUN_037edc34(&local_18);
  FUN_037edc34();
  FUN_037edbd8(&local_48);
  FUN_037edb7c(&local_60);
  FUN_037edb6c(&local_78);
  for (lVar2 = 0; (int)lVar2 < param_5; lVar2 = lVar2 + 1) {
    uVar11 = 0;
    lVar9 = lVar2 * 0x18;
    do {
      uVar12 = 0;
      do {
        lVar8 = *(long *)(*(long *)(*param_3 + uVar12 * 0x18) + uVar11 * 0x18);
        lVar1 = lVar8 + lVar9;
        lVar6 = *(long *)(lVar8 + lVar9);
        puVar4 = *(ulong **)(*(long *)(*(long *)(*param_2 + uVar12 * 0x18) + uVar11 * 0x18) + lVar9)
        ;
        local_18 = *puVar4;
        local_10 = CONCAT44(local_10._4_4_,(int)puVar4[1]);
        if ((ulong)(*(long *)(lVar1 + 0x10) - lVar6) < 0xc) {
          lVar6 = _Znwm(0xc);
          FUN_037ee990(&local_18,(long)&local_10 + 4,lVar6);
          if (*(long *)(lVar8 + lVar9) != 0) {
            _ZdlPv();
          }
          *(long *)(lVar8 + lVar9) = lVar6;
          *(long *)(lVar1 + 8) = lVar6 + 0xc;
          *(long *)(lVar1 + 0x10) = lVar6 + 0xc;
        }
        else {
          uVar7 = *(long *)(lVar1 + 8) - lVar6;
          puVar4 = &local_18;
          if (uVar7 < 0xc) {
            puVar4 = (ulong *)((long)&local_18 + uVar7);
            FUN_037ee990(&local_18,puVar4);
            lVar6 = *(long *)(lVar1 + 8);
          }
          uVar3 = FUN_037ee990(puVar4,(long)&local_10 + 4,lVar6);
          *(undefined8 *)(lVar1 + 8) = uVar3;
        }
        uVar12 = uVar12 + 1;
      } while (uVar10 != uVar12);
      uVar11 = uVar11 + 1;
    } while (uVar10 != uVar11);
  }
  local_30 = local_30 & 0xffffffff00000000;
  FUN_037f7658(&local_18,5,&local_30,&local_48);
  FUN_037f7590(param_4,&local_18);
  FUN_037f7580(&local_18);
  piVar5 = (int *)*param_4;
  *piVar5 = param_5;
  piVar5[1] = param_6;
  piVar5[4] = param_7;
  return;
}


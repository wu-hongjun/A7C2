// Decompiled pseudocode, not original source.
// REA Evidence: ev_e550f03e68df80982f83223e8554c0500c5eb9d4e11e208b99e8bdcbde131b49
// Source record: research/evidence/rea/still-sink-finalizers/02-procedure_pseudo_code.json

undefined8 FUN_0036a424(long *param_1,undefined8 *param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  long lVar5;
  long *plVar6;
  ulong uVar7;
  long *plVar8;
  long lVar9;
  long lVar10;
  undefined8 uVar11;
  undefined8 *puVar12;
  long lVar13;
  undefined8 local_168;
  long local_160;
  uint local_158;
  undefined4 local_154;
  ulong local_150;
  void *local_148;
  undefined8 local_140;
  undefined8 uStack_138;
  undefined8 local_130;
  undefined8 uStack_128;
  undefined8 local_120;
  undefined1 local_118;
  undefined1 uStack_117;
  undefined2 uStack_116;
  undefined1 uStack_114;
  undefined1 uStack_113;
  undefined1 uStack_112;
  undefined1 uStack_111;
  undefined2 uStack_110;
  undefined2 uStack_10e;
  undefined8 local_10c;
  int local_104;
  undefined8 local_100;
  undefined1 local_f8;
  undefined8 local_f0;
  int local_e8;
  undefined8 local_b0;
  undefined8 local_a8;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined8 local_98;
  undefined8 local_90;
  undefined4 local_88;
  undefined4 local_84;
  undefined8 local_80;
  undefined8 local_78;
  undefined4 local_70;
  undefined4 local_6c;
  undefined8 local_68;
  undefined8 local_60;
  undefined4 local_58;
  undefined4 local_54;
  undefined8 local_50;
  undefined8 local_48;
  undefined4 local_40;
  undefined4 local_3c;
  undefined8 local_38;
  undefined8 local_30;
  uint local_28;
  undefined4 local_24;
  undefined8 local_20;
  undefined8 local_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  
  lVar10 = param_1[0x291];
  if (lVar10 != 0) {
    FUN_0035ba94(lVar10);
    _ZdlPvm(lVar10,0x3d8);
  }
  lVar10 = _Znwm(0x3d8);
  FUN_0035ba60(lVar10,0);
  param_1[0x291] = lVar10;
  memset(&local_b0,0,0xb0);
  local_a8 = param_2[0x2b];
  local_b0 = param_2[0x2c];
  local_a0 = *(undefined4 *)(param_2 + 0x2a);
  local_9c = *(undefined4 *)(param_2 + 0x29);
  local_78 = param_2[0x43];
  local_80 = param_2[0x44];
  local_70 = *(undefined4 *)(param_2 + 0x42);
  local_6c = *(undefined4 *)(param_2 + 0x41);
  local_90 = param_2[0x37];
  local_98 = param_2[0x38];
  local_88 = *(undefined4 *)(param_2 + 0x36);
  local_84 = *(undefined4 *)(param_2 + 0x35);
  local_60 = param_2[0x31];
  local_68 = param_2[0x32];
  local_58 = *(undefined4 *)(param_2 + 0x30);
  local_54 = *(undefined4 *)(param_2 + 0x2f);
  local_30 = param_2[0x49];
  local_3c = *(undefined4 *)(param_2 + 0x4d);
  local_38 = param_2[0x4a];
  local_28 = *(uint *)(param_2 + 0x48);
  local_18 = param_2[0x1f];
  local_24 = *(undefined4 *)(param_2 + 0x47);
  local_20 = param_2[0x20];
  local_50 = param_2[0x50];
  local_10 = *(undefined4 *)(param_2 + 0x1e);
  local_40 = *(undefined4 *)(param_2 + 0x4e);
  uVar11 = param_2[0x4f];
  local_118 = (undefined1)local_40;
  uStack_117 = (undefined1)((uint)local_40 >> 8);
  uStack_116 = (undefined2)((uint)local_40 >> 0x10);
  local_c = *(undefined4 *)(param_2 + 0x1d);
  local_48 = uVar11;
  if ((long *)param_1[0x2aa] != (long *)0x0) {
    (**(code **)(*(long *)param_1[0x2aa] + 8))();
  }
  lVar10 = _Znwm(0x1d060);
  FUN_003798a0();
  param_1[0x2aa] = lVar10;
  FUN_00379c6c(lVar10,0,0,&local_118);
  FUN_0037a978(param_1[0x2aa],uVar11,CONCAT22(uStack_116,CONCAT11(uStack_117,local_118)),
               param_2[0x80],*(undefined4 *)(param_2 + 0x7f),0,0,
               *(undefined1 *)((long)param_2 + 0x44c),param_2 + 0x8b,(long)param_2 + 0x485);
  lVar13 = param_2[0x49];
  local_40 = CONCAT22(uStack_116,CONCAT11(uStack_117,local_118));
  lVar10 = param_2[0x4a];
  if ((long *)param_1[0x2a9] != (long *)0x0) {
    (**(code **)(*(long *)param_1[0x2a9] + 8))();
  }
  lVar5 = _Znwm(0x1848);
  FUN_00376c98(lVar5,lVar13,0,10,lVar13 - lVar10,*(undefined4 *)(param_2 + 0x48));
  param_1[0x2a9] = lVar5;
  iVar2 = FUN_0037963c(lVar5,param_2);
  lVar10 = param_2[0x55];
  uVar1 = iVar2 + 0xfffU & 0xfffff000;
  if (lVar10 == 0) {
    if (*(long *)(param_1[0x2a9] + 0x50) != 0) {
      FUN_001c7808(*(long *)(param_1[0x2a9] + 0x50),0x2044,0,0);
      FUN_001c7808(*(undefined8 *)(param_1[0x2a9] + 0x50),0x2044,1,0);
    }
  }
  else {
    lVar5 = param_2[0x56];
    iVar2 = uVar1 - 10;
    lVar13 = _Znwm(0x28);
    FUN_0035aecc(lVar13,1,lVar10,4,iVar2,lVar10 - lVar5);
    param_1[0x2ab] = lVar13;
    iVar3 = FUN_0035aff8(lVar13,param_2);
    if (*(long *)(param_1[0x2a9] + 0x50) != 0) {
      FUN_001c7808(*(long *)(param_1[0x2a9] + 0x50),0x2044,0,iVar2);
      FUN_001c7808(*(undefined8 *)(param_1[0x2a9] + 0x50),0x2044,1,iVar3 + 0xfffU & 0xfffff000);
      uVar4 = FUN_0036b944(*(undefined8 *)(param_1[0x2a9] + 0x50),0x2044);
      uStack_110 = (undefined2)uVar4;
      uStack_10e = (undefined2)((uint)uVar4 >> 0x10);
      local_10c = (ulong)local_10c._4_4_ << 0x20;
      uStack_113 = 0;
      uStack_114 = 2;
      uStack_112 = 0;
      local_118 = 0x61;
      uStack_117 = 0x22;
      uStack_111 = 0;
      uStack_116 = 0x2016;
      FUN_00369e78(&local_118,0x10);
    }
  }
  local_118 = 0xff;
  uStack_117 = 0;
  uStack_116 = 0;
  local_150 = local_150 & 0xffffffffffffff00;
  local_28 = uVar1;
  (**(code **)(*param_1 + 0x78))(param_1,param_2,2,&local_150,&local_118);
  if ((char)local_150 != '\0') {
    iVar2 = CONCAT22(uStack_116,CONCAT11(uStack_117,local_118));
    if (iVar2 == 0) {
      lVar10 = param_1[0x2a9];
      uVar11 = 1;
    }
    else {
      if (iVar2 != 1) goto LAB_0036a778;
      uVar11 = 2;
      lVar10 = param_1[0x2a9];
    }
    FUN_00367c28(*(undefined8 *)(lVar10 + 0x38),0x213,0,uVar11);
  }
LAB_0036a778:
  local_10 = 0x1000;
  if (*(char *)(param_2 + 0x10d) != '\0') {
    local_10 = 0x4000;
  }
  local_8 = FUN_0036a034(param_1,*(undefined4 *)*param_2);
  local_160 = param_2[0x55];
  local_158 = 0;
  if (local_160 != 0) {
    local_158 = *(int *)(param_2 + 0x54) + 0xfffU & 0xfffff000;
  }
  local_168 = param_2[0x56];
  local_154 = *(undefined4 *)(param_2 + 0x53);
  FUN_0035c3ac(param_1[0x291],&local_b0,&local_168);
  local_148 = (void *)0x0;
  local_140 = 0;
  uStack_138 = 0;
  local_150 = 7;
  local_130 = 0;
  uStack_128 = 0;
  local_120 = 0;
  local_148 = calloc(7,0x18);
  if (local_148 != (void *)0x0) {
    local_118 = *(undefined1 *)(param_2 + 0x81);
    uVar11 = param_2[0x83];
    uStack_116 = (undefined2)uVar11;
    uStack_114 = (undefined1)((ulong)uVar11 >> 0x10);
    uStack_113 = (undefined1)((ulong)uVar11 >> 0x18);
    uStack_112 = (undefined1)((ulong)uVar11 >> 0x20);
    uStack_111 = (undefined1)((ulong)uVar11 >> 0x28);
    uStack_110 = (undefined2)((ulong)uVar11 >> 0x30);
    uStack_10e = *(undefined2 *)(param_2 + 0x84);
    local_10c = param_2[0x85];
    local_104 = CONCAT22(local_104._2_2_,*(undefined2 *)(param_2 + 0x86));
    local_100 = param_2[0x82];
    local_e8 = *(int *)(param_2 + 0x87);
    local_f8 = local_e8 != 0;
    local_f0 = param_2[0x88];
    _ZN7HEIFLib3FinEPN10HEIFCommon17AREA_MULTI_LIST_TEbPN3mpr8libcrypt10assetParamE
              (param_1[0x291],&local_150,*(undefined1 *)(param_2 + 0x10d),&local_118);
    if ((uint)local_150 < 8) {
      *(uint *)(param_1 + 0x23e) = (uint)local_150;
      plVar8 = param_1 + 0x23f;
      do {
        plVar6 = plVar8 + 6;
        FUN_00369fa0(plVar8);
        plVar8 = plVar6;
      } while (param_1 + 0x275 != plVar6);
      puVar12 = &local_140;
      uVar7 = 0;
      for (lVar10 = 0; (uint)lVar10 < (uint)local_150; lVar10 = lVar10 + 1) {
        switch(*(undefined4 *)puVar12) {
        case 0:
          plVar8 = param_2 + 0x29;
          plVar6 = param_1 + uVar7 * 6 + 0x23f;
          break;
        case 1:
          plVar6 = param_1 + uVar7 * 6 + 0x23f;
          lVar13 = param_2[0x36];
          *plVar6 = param_2[0x35];
          param_1[uVar7 * 6 + 0x240] = lVar13;
          lVar13 = param_2[0x38];
          param_1[uVar7 * 6 + 0x241] = param_2[0x37];
          param_1[uVar7 * 6 + 0x242] = lVar13;
          lVar13 = param_2[0x39];
          lVar5 = param_2[0x3a];
          goto LAB_0036a9a4;
        case 2:
          plVar6 = param_1 + uVar7 * 6 + 0x23f;
          plVar8 = param_2 + 0x41;
          break;
        case 3:
          plVar8 = param_2 + 0x2f;
          plVar6 = param_1 + uVar7 * 6 + 0x23f;
          break;
        case 4:
          uStack_113 = 0;
          uStack_114 = 2;
          uStack_112 = 0;
          uStack_116 = 0x2110;
          uStack_111 = 0;
          local_118 = 0x61;
          uStack_117 = 0x22;
          uStack_110 = 0;
          uStack_10e = 0;
          local_10c = local_10c & 0xffffffff00000000;
          FUN_00369e78(&local_118,0x10);
          lVar13 = param_2[0x48];
          param_1[uVar7 * 6 + 0x23f] = param_2[0x47];
          param_1[uVar7 * 6 + 0x240] = lVar13;
          lVar13 = param_2[0x4a];
          param_1[uVar7 * 6 + 0x241] = param_2[0x49];
          param_1[uVar7 * 6 + 0x242] = lVar13;
          lVar13 = param_2[0x4c];
          param_1[uVar7 * 6 + 0x243] = param_2[0x4b];
          param_1[uVar7 * 6 + 0x244] = lVar13;
          if (param_2[0x55] == 0) goto switchD_0036a978_default;
          lVar13 = lVar10 * 0x18;
          lVar9 = param_1[uVar7 * 6 + 0x241];
          uVar11 = *(undefined8 *)((long)local_148 + uVar7 * 0x18 + 8);
          uVar4 = *(undefined4 *)((long)local_148 + lVar13 + 0x10);
          lVar5 = *(long *)((long)local_148 + lVar13 + 8);
          param_1[uVar7 * 6 + 0x242] = *(long *)((long)local_148 + lVar13);
          iVar2 = *(int *)((long)param_1 + uVar7 * 0x30 + 0x11fc);
          *(undefined4 *)(param_1 + uVar7 * 6 + 0x240) = uVar4;
          param_1[uVar7 * 6 + 0x241] = lVar5;
          local_104 = (iVar2 - (int)lVar9) + (int)uVar11;
          uStack_110 = (undefined2)uVar7;
          uStack_10e = (undefined2)(uVar7 >> 0x10);
          *(int *)((long)param_1 + uVar7 * 0x30 + 0x11fc) = local_104;
          local_10c = CONCAT44(uVar4,(int)lVar5);
          uStack_114 = 1;
          local_118 = 0x61;
          uStack_117 = 0x22;
          uStack_116 = 0xef13;
          uStack_113 = 0;
          uStack_112 = 0;
          uStack_111 = 0;
          FUN_00369e78(&local_118);
          uVar7 = (ulong)((int)uVar7 + 1);
          lVar13 = param_2[0x54];
          param_1[uVar7 * 6 + 0x23f] = param_2[0x53];
          param_1[uVar7 * 6 + 0x240] = lVar13;
          lVar13 = param_2[0x56];
          param_1[uVar7 * 6 + 0x241] = param_2[0x55];
          param_1[uVar7 * 6 + 0x242] = lVar13;
          lVar13 = param_2[0x58];
          param_1[uVar7 * 6 + 0x243] = param_2[0x57];
          param_1[uVar7 * 6 + 0x244] = lVar13;
          iVar2 = *(int *)(param_2 + 0x54);
          *(undefined4 *)((long)param_1 + uVar7 * 0x30 + 0x11fc) = 0;
          *(uint *)(param_1 + uVar7 * 6 + 0x240) = iVar2 + 0xfffU & 0xfffff000;
          *(int *)(param_1 + 0x23e) = (int)param_1[0x23e] + 1;
          goto LAB_0036ab38;
        case 5:
          plVar8 = param_2 + 0x4d;
          plVar6 = param_1 + uVar7 * 6 + 0x23f;
          break;
        case 6:
          plVar8 = param_2 + 0x1d;
          plVar6 = param_1 + uVar7 * 6 + 0x23f;
          break;
        default:
          goto switchD_0036a978_default;
        }
        lVar13 = plVar8[1];
        *plVar6 = *plVar8;
        plVar6[1] = lVar13;
        lVar13 = plVar8[3];
        plVar6[2] = plVar8[2];
        plVar6[3] = lVar13;
        lVar13 = plVar8[4];
        lVar5 = plVar8[5];
LAB_0036a9a4:
        plVar6[4] = lVar13;
        plVar6[5] = lVar5;
switchD_0036a978_default:
        lVar13 = lVar10 * 0x18;
        lVar9 = param_1[uVar7 * 6 + 0x241];
        lVar5 = *(long *)((long)local_148 + lVar13 + 8);
        param_1[uVar7 * 6 + 0x242] = *(long *)((long)local_148 + lVar13);
        uVar4 = *(undefined4 *)((long)local_148 + lVar13 + 0x10);
        param_1[uVar7 * 6 + 0x241] = lVar5;
        *(undefined4 *)(param_1 + uVar7 * 6 + 0x240) = uVar4;
        *(int *)((long)param_1 + uVar7 * 0x30 + 0x11fc) =
             (*(int *)((long)param_1 + uVar7 * 0x30 + 0x11fc) - (int)lVar9) + (int)lVar5;
LAB_0036ab38:
        uStack_113 = 0;
        uStack_112 = 0;
        uStack_114 = 1;
        uStack_111 = 0;
        puVar12 = (undefined8 *)((long)puVar12 + 4);
        local_118 = 0x61;
        uStack_117 = 0x22;
        uStack_116 = 0xef12;
        uStack_110 = (undefined2)uVar7;
        uStack_10e = (undefined2)(uVar7 >> 0x10);
        local_104 = *(undefined4 *)((long)param_1 + uVar7 * 0x30 + 0x11fc);
        local_10c = CONCAT44((int)param_1[uVar7 * 6 + 0x240],(int)param_1[uVar7 * 6 + 0x241]);
        FUN_00369e78(&local_118,0x18);
        uVar7 = (ulong)((int)uVar7 + 1);
      }
      FUN_00369fa0(param_1 + (ulong)*(uint *)(param_1 + 0x23e) * 6 + 0x23f);
      goto LAB_0036a824;
    }
  }
  *(undefined4 *)(param_1 + 0x23e) = 0;
LAB_0036a824:
  if (local_148 != (void *)0x0) {
    free(local_148);
  }
  return 0;
}


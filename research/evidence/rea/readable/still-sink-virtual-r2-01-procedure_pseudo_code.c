// Decompiled pseudocode, not original source.
// REA Evidence: ev_d5cdb362f54685506aad20e8c691edc6a5972abec51143a30192567045381b18
// Source record: research/evidence/rea/still-sink-virtual-r2/01-procedure_pseudo_code.json

undefined8 FUN_0036ac20(long param_1,long param_2,long param_3)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined8 uVar4;
  void *pvVar5;
  void *pvVar6;
  undefined8 *puVar7;
  long lVar8;
  long lVar9;
  undefined8 uVar10;
  uint uVar11;
  long lVar12;
  ulong uVar13;
  uint *puVar14;
  long lVar15;
  undefined8 local_3c8;
  undefined8 local_3c0;
  ulong local_3b8;
  ulong local_3b0;
  undefined8 local_3a8;
  undefined8 local_3a0;
  undefined8 local_398;
  void *local_390;
  undefined8 local_388;
  undefined8 local_380;
  undefined8 uStack_378;
  void *local_370;
  void *local_368;
  uint local_360 [2];
  undefined8 local_358;
  void *local_350;
  undefined8 local_348;
  undefined8 local_340;
  undefined8 local_330;
  undefined8 local_328;
  undefined4 local_320;
  undefined8 local_318;
  undefined8 local_310;
  undefined4 local_308;
  undefined8 local_300;
  undefined8 local_2f8;
  undefined4 local_2f0;
  undefined8 local_2e8;
  undefined8 local_2e0;
  undefined4 local_2d8;
  undefined8 local_2d0;
  void *local_2c8;
  undefined4 local_2c0;
  long local_2b8;
  long local_2b0;
  uint local_2a8;
  undefined8 local_2a0;
  undefined8 local_298;
  undefined4 local_290;
  undefined4 local_288;
  undefined4 local_280;
  undefined1 local_27c;
  undefined1 local_27b;
  undefined1 local_27a;
  undefined1 local_279;
  undefined4 local_278;
  undefined4 local_274;
  int local_15c;
  int local_c4;
  
  local_27c = 2;
  local_280 = 0x26102261;
  local_27b = 0;
  local_27a = 0;
  local_279 = 0;
  local_278 = 0;
  local_274 = 0;
  FUN_00369e78(&local_280,0x10);
  memset(&local_280,0,0x27c);
  lVar12 = *(long *)(param_1 + 0x1488);
  if (lVar12 != 0) {
    FUN_0035ba94(lVar12);
    _ZdlPvm(lVar12,0x3d8);
  }
  uVar4 = _Znwm(0x3d8);
  FUN_0035ba60(uVar4,0);
  *(undefined8 *)(param_1 + 0x1488) = uVar4;
  memset(&local_330,0,0xb0);
  if (*(int *)(param_3 + 0x24) == 1) {
    local_328 = *(undefined8 *)(param_3 + 0x98);
    local_330 = *(undefined8 *)(param_3 + 0xa0);
    local_320 = *(undefined4 *)(param_3 + 0x90);
    local_2f8 = *(undefined8 *)(param_3 + 0x158);
    local_300 = *(undefined8 *)(param_3 + 0x160);
    local_2f0 = *(undefined4 *)(param_3 + 0x150);
    local_310 = *(undefined8 *)(param_3 + 0xf8);
    local_318 = *(undefined8 *)(param_3 + 0x100);
    local_308 = *(undefined4 *)(param_3 + 0xf0);
    local_2e0 = *(undefined8 *)(param_3 + 200);
    local_2e8 = *(undefined8 *)(param_3 + 0xd0);
    local_2d8 = *(undefined4 *)(param_3 + 0xc0);
  }
  else {
    if (*(int *)(param_3 + 0x24) != 2) {
      return 0xfffffffe;
    }
    if (*(int *)(param_3 + 0x20) == 1) {
      local_328 = *(undefined8 *)(param_3 + 0x98);
      local_330 = *(undefined8 *)(param_3 + 0xa0);
      local_320 = *(undefined4 *)(param_3 + 0x90);
      local_2f8 = *(undefined8 *)(param_3 + 0x158);
      local_300 = *(undefined8 *)(param_3 + 0x160);
      local_2f0 = *(undefined4 *)(param_3 + 0x150);
    }
    else {
      if (*(int *)(param_3 + 0x20) != 0) {
        return 0xffffffff;
      }
      local_328 = *(undefined8 *)(param_3 + 0x98);
      local_330 = *(undefined8 *)(param_3 + 0xa0);
      local_320 = *(undefined4 *)(param_3 + 0x90);
    }
  }
  local_2b0 = *(long *)(param_3 + 0x188);
  local_2b8 = *(long *)(param_3 + 400);
  local_2a8 = *(undefined4 *)(param_3 + 0x180);
  local_2c8 = *(void **)(param_3 + 0x1b8);
  local_2d0 = *(undefined8 *)(param_3 + 0x1c0);
  local_2c0 = *(undefined4 *)(param_3 + 0x1b0);
  local_298 = *(undefined8 *)(param_3 + 0x38);
  local_2a0 = *(undefined8 *)(param_3 + 0x40);
  local_290 = *(undefined4 *)(param_3 + 0x30);
  memcpy(local_2c8,*(void **)(param_2 + 0xbd8),(ulong)*(uint *)(param_2 + 0xbd0));
  local_2c0 = *(undefined4 *)(param_2 + 0xbd0);
  if (*(long **)(param_1 + 0x1548) != (long *)0x0) {
    (**(code **)(**(long **)(param_1 + 0x1548) + 8))();
  }
  lVar12 = local_2b0 - local_2b8;
  uVar4 = _Znwm(0x1848);
  FUN_00376c98(uVar4,local_2b0,0,10,lVar12,local_2a8);
  *(undefined8 *)(param_1 + 0x1548) = uVar4;
  iVar2 = FUN_003796b0(uVar4,*(undefined8 *)(param_2 + 0x1548),param_3);
  uVar1 = iVar2 + 0xfff;
  if ((int)uVar1 < 0) {
    return 0xfffffffd;
  }
  lVar12 = *(long *)(param_3 + 0x1e8);
  if (lVar12 == 0) {
    uVar11 = 0;
    lVar12 = *(long *)(*(long *)(param_1 + 0x1548) + 0x50);
    if (lVar12 != 0) {
      FUN_001c7808(lVar12,0x2044,0,0);
      lVar12 = *(long *)(param_1 + 0x1548);
      goto LAB_0036ae6c;
    }
  }
  else {
    lVar15 = *(long *)(param_3 + 0x1f0);
    iVar2 = (uVar1 & 0xfffff000) - 10;
    uVar4 = _Znwm(0x28);
    FUN_0035aecc(uVar4,1,lVar12,2,iVar2,lVar12 - lVar15);
    *(undefined8 *)(param_1 + 0x1558) = uVar4;
    iVar3 = FUN_0035b168(uVar4,param_3);
    uVar11 = iVar3 + 0xfffU & 0xfffff000;
    lVar12 = *(long *)(*(long *)(param_1 + 0x1548) + 0x50);
    if (lVar12 != 0) {
      FUN_001c7808(lVar12,0x2044,0,iVar2);
      lVar12 = *(long *)(param_1 + 0x1548);
LAB_0036ae6c:
      FUN_001c7808(*(undefined8 *)(lVar12 + 0x50),0x2044,1,uVar11);
    }
  }
  local_368 = (void *)(CONCAT44(uVar11,uVar1) & 0xfffffffffffff000);
  local_370 = (void *)0x224142261;
  local_2a8 = uVar1 & 0xfffff000;
  FUN_00369e78(&local_370,0x10);
  if (*(int *)(param_3 + 0x24) == 1) {
    local_370 = (void *)CONCAT44(local_370._4_4_,0xff);
    local_3b0 = local_3b0 & 0xffffffffffffff00;
    FUN_0036a3a8(param_1,param_3,2,&local_3b0,&local_370);
    if ((char)local_3b0 != '\0') {
      if ((uint)local_370 == 0) {
        local_c4 = 1;
        lVar12 = *(long *)(param_1 + 0x1548);
      }
      else {
        if ((uint)local_370 != 1) goto LAB_0036afd8;
        local_c4 = 2;
        lVar12 = *(long *)(param_1 + 0x1548);
      }
LAB_0036afbc:
      FUN_00367c28(*(undefined8 *)(lVar12 + 0x38),0x213,0,local_c4);
    }
  }
  else {
    if (*(int *)(param_3 + 0x24) != 2) {
      return 0xfffffffb;
    }
    FUN_0035d348(*(undefined8 *)(param_2 + 0x1488),1,&local_280);
    if ((*(int *)(param_3 + 0x20) != 0) && (local_c4 = local_15c, *(int *)(param_3 + 0x20) != 1)) {
      return 0xfffffffc;
    }
    if ((local_c4 == 1) || (local_c4 == 2)) {
      lVar12 = *(long *)(param_1 + 0x1548);
      goto LAB_0036afbc;
    }
  }
LAB_0036afd8:
  local_290 = 0x1000;
  if (*(char *)(param_3 + 0x44d) != '\0') {
    local_290 = 0x4000;
  }
  if (*(int *)(param_3 + 0x24) == 1) {
    local_288 = FUN_0036a034(param_1,*(undefined2 *)(param_3 + 0x318));
  }
  else {
    local_288 = local_280;
    if (*(int *)(param_3 + 0x24) != 2) {
      return 0xfffffffa;
    }
  }
  local_3b8 = 0;
  local_3c8 = 0;
  local_3c0 = 0;
  if (*(int *)(param_3 + 0x24) == 1) {
    local_3c0 = *(undefined8 *)(param_3 + 0x1e8);
    uVar4 = 0xfffffff7;
    local_3c8 = *(undefined8 *)(param_3 + 0x1f0);
    local_3b8 = (ulong)*(uint *)(param_3 + 0x1e0);
    iVar2 = FUN_0035c3ac(*(undefined8 *)(param_1 + 0x1488),&local_330,&local_3c8);
    goto joined_r0x0036b220;
  }
  if (*(int *)(param_3 + 0x24) != 2) {
    return 0xfffffff8;
  }
  local_3b0 = 0;
  local_3a8 = 0;
  local_3a0 = (void *)0x0;
  local_398 = 0;
  local_390 = (void *)0x0;
  local_388 = 0;
  local_380 = 0;
  uStack_378 = 0;
  pvVar5 = malloc(0x3f0);
  *(void **)(param_1 + 0x1538) = pvVar5;
  pvVar6 = malloc(0x3f0);
  iVar2 = *(int *)(param_3 + 0x20);
  *(void **)(param_1 + 0x1540) = pvVar6;
  if (iVar2 == 0) {
    local_388 = CONCAT44(local_388._4_4_,0x3f0);
    local_390 = pvVar6;
  }
  else if (iVar2 == 1) {
    local_398 = CONCAT44(local_398._4_4_,0x3f0);
    local_388 = CONCAT44(local_388._4_4_,0x3f0);
    local_3a0 = pvVar5;
    local_390 = pvVar6;
  }
  FUN_0035da30(*(undefined8 *)(param_2 + 0x1488),1,&local_3b0);
  if (*(int *)(param_3 + 0x20) == 0) {
    if (local_390 == (void *)0x0) {
      local_370 = (void *)0x0;
      local_368._0_4_ = 0;
    }
    else {
      local_370 = local_390;
      local_368._0_4_ = (undefined4)local_388;
    }
    local_358 = (ulong)local_358._4_4_ << 0x20;
LAB_0036b1dc:
    local_360[0] = 0;
    local_360[1] = 0;
    local_350 = (void *)0x0;
    local_348 = (ulong)local_348._4_4_ << 0x20;
  }
  else if (*(int *)(param_3 + 0x20) == 1) {
    if (local_3a0 == (void *)0x0) {
      local_370 = (void *)0x0;
      local_368._0_4_ = 0;
    }
    else {
      local_370 = local_3a0;
      local_368._0_4_ = (undefined4)local_398;
    }
    local_360[0] = 0;
    local_360[1] = 0;
    local_358 = (ulong)local_358._4_4_ << 0x20;
    if (local_390 == (void *)0x0) goto LAB_0036b1dc;
    local_350 = local_390;
    local_348 = CONCAT44(local_348._4_4_,(undefined4)local_388);
  }
  uVar4 = 0xfffffff9;
  iVar2 = FUN_0035c118(*(undefined8 *)(param_1 + 0x1488),&local_330,&local_370);
joined_r0x0036b220:
  if (iVar2 == 0) {
    local_368 = (void *)0x0;
    local_360[0] = 0;
    local_360[1] = 0;
    local_358 = 0;
    local_370 = (void *)0x7;
    local_350 = (void *)0x0;
    local_348 = 0;
    local_340 = 0;
    local_368 = calloc(7,0x18);
    if (local_368 == (void *)0x0) {
      uVar4 = 0xfffffff6;
      *(undefined4 *)(param_1 + 0x11f0) = 0;
    }
    else {
      uVar4 = 0xfffffff4;
      iVar2 = _ZN7HEIFLib3FinEPN10HEIFCommon17AREA_MULTI_LIST_TEbPN3mpr8libcrypt10assetParamE
                        (*(undefined8 *)(param_1 + 0x1488),&local_370,
                         *(undefined1 *)(param_3 + 0x44d),param_2 + 0x14a8);
      if (iVar2 == 0) {
        if ((uint)local_370 < 8) {
          *(uint *)(param_1 + 0x11f0) = (uint)local_370;
          lVar12 = param_1 + 0x11f8;
          do {
            lVar15 = lVar12 + 0x30;
            FUN_00369fa0(lVar12);
            lVar12 = lVar15;
          } while (param_1 + 0x13a8 != lVar15);
          puVar14 = local_360;
          uVar13 = 0;
          for (lVar12 = 0; (uint)lVar12 < (uint)local_370; lVar12 = lVar12 + 1) {
            if (*puVar14 < 7) {
              lVar15 = param_1 + uVar13 * 0x30;
              switch(*puVar14) {
              case 0:
                puVar7 = (undefined8 *)(param_3 + 0x88);
                break;
              case 1:
                puVar7 = (undefined8 *)(param_3 + 0xe8);
                break;
              case 2:
                puVar7 = (undefined8 *)(param_3 + 0x148);
                break;
              case 3:
                puVar7 = (undefined8 *)(param_3 + 0xb8);
                break;
              case 4:
                uVar4 = *(undefined8 *)(param_3 + 0x180);
                *(undefined8 *)(lVar15 + 0x11f8) = *(undefined8 *)(param_3 + 0x178);
                *(undefined8 *)(lVar15 + 0x1200) = uVar4;
                uVar4 = *(undefined8 *)(param_3 + 400);
                *(undefined8 *)(lVar15 + 0x1208) = *(undefined8 *)(param_3 + 0x188);
                *(undefined8 *)(lVar15 + 0x1210) = uVar4;
                uVar4 = *(undefined8 *)(param_3 + 0x1a0);
                *(undefined8 *)(lVar15 + 0x1218) = *(undefined8 *)(param_3 + 0x198);
                *(undefined8 *)(lVar15 + 0x1220) = uVar4;
                if (*(long *)(param_3 + 0x1e8) == 0) goto LAB_0036b320;
                lVar8 = lVar12 * 0x18;
                uVar10 = *(undefined8 *)(lVar15 + 0x1208);
                uVar4 = *(undefined8 *)((long)local_368 + uVar13 * 0x18 + 8);
                uVar13 = (ulong)((int)uVar13 + 1);
                *(undefined8 *)(lVar15 + 0x1210) = *(undefined8 *)((long)local_368 + lVar8);
                lVar9 = param_1 + uVar13 * 0x30;
                *(undefined8 *)(lVar15 + 0x1208) = *(undefined8 *)((long)local_368 + lVar8 + 8);
                *(undefined4 *)(lVar15 + 0x1200) = *(undefined4 *)((long)local_368 + lVar8 + 0x10);
                *(int *)(lVar15 + 0x11fc) = (*(int *)(lVar15 + 0x11fc) - (int)uVar10) + (int)uVar4;
                uVar4 = *(undefined8 *)(param_3 + 0x1e0);
                *(undefined8 *)(lVar9 + 0x11f8) = *(undefined8 *)(param_3 + 0x1d8);
                *(undefined8 *)(lVar9 + 0x1200) = uVar4;
                uVar4 = *(undefined8 *)(param_3 + 0x1f0);
                *(undefined8 *)(lVar9 + 0x1208) = *(undefined8 *)(param_3 + 0x1e8);
                *(undefined8 *)(lVar9 + 0x1210) = uVar4;
                uVar4 = *(undefined8 *)(param_3 + 0x200);
                *(undefined8 *)(lVar9 + 0x1218) = *(undefined8 *)(param_3 + 0x1f8);
                *(undefined8 *)(lVar9 + 0x1220) = uVar4;
                iVar2 = *(int *)(param_3 + 0x1e0);
                *(undefined4 *)(lVar9 + 0x11fc) = 0;
                *(uint *)(lVar9 + 0x1200) = iVar2 + 0xfffU & 0xfffff000;
                *(int *)(param_1 + 0x11f0) = *(int *)(param_1 + 0x11f0) + 1;
                goto LAB_0036b42c;
              case 5:
                puVar7 = (undefined8 *)(param_3 + 0x1a8);
                break;
              case 6:
                puVar7 = (undefined8 *)(param_3 + 0x28);
              }
              uVar4 = puVar7[1];
              *(undefined8 *)(lVar15 + 0x11f8) = *puVar7;
              *(undefined8 *)(lVar15 + 0x1200) = uVar4;
              uVar4 = puVar7[3];
              *(undefined8 *)(lVar15 + 0x1208) = puVar7[2];
              *(undefined8 *)(lVar15 + 0x1210) = uVar4;
              uVar4 = puVar7[5];
              *(undefined8 *)(lVar15 + 0x1218) = puVar7[4];
              *(undefined8 *)(lVar15 + 0x1220) = uVar4;
            }
LAB_0036b320:
            lVar9 = lVar12 * 0x18;
            lVar15 = param_1 + uVar13 * 0x30;
            uVar4 = *(undefined8 *)((long)local_368 + lVar9 + 8);
            *(undefined8 *)(lVar15 + 0x1210) = *(undefined8 *)((long)local_368 + lVar9);
            uVar10 = *(undefined8 *)(lVar15 + 0x1208);
            *(undefined4 *)(lVar15 + 0x1200) = *(undefined4 *)((long)local_368 + lVar9 + 0x10);
            *(undefined8 *)(lVar15 + 0x1208) = uVar4;
            *(int *)(lVar15 + 0x11fc) = (*(int *)(lVar15 + 0x11fc) - (int)uVar10) + (int)uVar4;
LAB_0036b42c:
            puVar14 = puVar14 + 1;
            local_3b0 = 0x1ef222261;
            lVar15 = param_1 + uVar13 * 0x30;
            local_3a8 = CONCAT44((int)*(undefined8 *)(lVar15 + 0x1208),(int)uVar13);
            uVar13 = (ulong)((int)uVar13 + 1);
            local_3a0 = (void *)CONCAT44(*(undefined4 *)(lVar15 + 0x11fc),
                                         *(undefined4 *)(lVar15 + 0x1200));
            FUN_00369e78(&local_3b0,0x18);
          }
          uVar4 = 0;
          FUN_00369fa0(param_1 + (ulong)*(uint *)(param_1 + 0x11f0) * 0x30 + 0x11f8);
        }
        else {
          uVar4 = 0xfffffff3;
          *(undefined4 *)(param_1 + 0x11f0) = 0;
        }
      }
    }
    if (local_368 != (void *)0x0) {
      free(local_368);
    }
  }
  return uVar4;
}


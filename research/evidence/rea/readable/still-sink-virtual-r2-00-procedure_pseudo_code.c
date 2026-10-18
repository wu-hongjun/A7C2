// Decompiled pseudocode, not original source.
// REA Evidence: ev_adabf6ef75f1327374412f4d3131125242884d5ecc6d00bae9cf3c86f2722686
// Source record: research/evidence/rea/still-sink-virtual-r2/00-procedure_pseudo_code.json

undefined8 FUN_0036f24c(long param_1,long param_2,long param_3)

{
  undefined8 *puVar1;
  int iVar2;
  uint uVar3;
  undefined8 uVar4;
  uint uVar5;
  char cVar6;
  bool bVar7;
  int iVar8;
  undefined4 uVar9;
  int iVar10;
  uint uVar11;
  long lVar12;
  undefined8 uVar13;
  undefined8 *puVar14;
  long *plVar15;
  size_t sVar16;
  undefined8 uVar17;
  long lVar18;
  int iVar19;
  long lVar20;
  ulong uVar21;
  undefined1 *__s;
  undefined8 *puVar22;
  int iVar23;
  long *plVar24;
  void *__src;
  undefined8 local_610;
  undefined8 uStack_608;
  undefined8 local_600;
  undefined8 uStack_5f8;
  undefined8 local_5f0;
  undefined8 uStack_5e8;
  undefined2 local_5d8;
  undefined2 local_5d6;
  undefined1 local_5d4;
  undefined1 local_5d3;
  undefined1 local_5d2;
  undefined1 local_5d1;
  undefined4 local_5d0;
  undefined8 *local_5c8;
  undefined8 local_5c0;
  undefined1 *local_5b8;
  undefined8 uStack_5b0;
  undefined8 local_5a8;
  undefined8 local_5a0;
  undefined8 local_598;
  undefined8 uStack_590;
  undefined8 local_588;
  undefined8 uStack_580;
  undefined8 local_578;
  undefined8 uStack_570;
  undefined8 local_568;
  undefined8 uStack_560;
  undefined8 local_558;
  undefined8 uStack_550;
  undefined8 local_548;
  undefined8 uStack_540;
  undefined8 local_538;
  undefined8 uStack_530;
  undefined8 local_528;
  undefined8 uStack_520;
  undefined8 local_518;
  undefined1 auStack_510 [144];
  undefined8 local_480 [2];
  int local_470 [116];
  ulong local_2a0;
  ulong local_298;
  undefined6 local_290;
  undefined2 uStack_28a;
  undefined2 local_288;
  undefined4 uStack_286;
  undefined2 uStack_282;
  undefined8 local_280;
  undefined8 uStack_278;
  undefined8 local_240;
  undefined4 local_238;
  undefined4 uStack_234;
  undefined4 *local_230;
  undefined8 local_228;
  undefined8 local_220;
  undefined8 uStack_218;
  undefined4 local_e8;
  undefined8 local_c0;
  undefined8 uStack_b8;
  undefined4 uStack_b4;
  undefined4 *local_b0;
  undefined8 local_a8;
  undefined8 local_a0;
  undefined8 uStack_98;
  
  __s = *(undefined1 **)(param_3 + 0x38);
  memset(__s,0,(ulong)*(uint *)(param_3 + 0x30));
  lVar12 = *(long *)(param_3 + 0x38) - *(long *)(param_3 + 0x40);
  uVar9 = *(undefined4 *)(param_3 + 0x30);
  *__s = 0xff;
  __s[1] = 0xd8;
  if (*(int *)(param_3 + 0x18) == 0) {
    bVar7 = false;
    iVar10 = 2;
    for (puVar22 = *(undefined8 **)(param_2 + 0x11d8); *(undefined8 **)(param_2 + 0x11e0) != puVar22
        ; puVar22 = puVar22 + 3) {
      if ((void *)*puVar22 != (void *)0x0) {
        if (((char *)puVar22[2] == (char *)0x0) && (*(int *)(puVar22 + 1) != 0)) {
          memcpy(__s + iVar10,(void *)*puVar22,(long)*(int *)(puVar22 + 1));
          iVar10 = iVar10 + *(int *)(puVar22 + 1);
        }
        else {
          iVar8 = strncmp((char *)puVar22[2],"APP1_EXIF",0x10);
          if (iVar8 == 0) {
            uVar13 = _Znwm(0x1848);
            FUN_00371b88(uVar13,__s,iVar10,10,lVar12,uVar9);
            *(undefined8 *)(param_1 + 0x1488) = uVar13;
            iVar8 = FUN_003796b0(uVar13,*(undefined8 *)(param_2 + 0x1488),param_3);
            if (iVar8 < 0) {
              return 0xffffffff;
            }
            iVar10 = iVar10 + iVar8;
            bVar7 = true;
          }
        }
      }
    }
  }
  else {
    if (*(int *)(param_3 + 0x18) != 1) {
      return 0xfffffffd;
    }
    bVar7 = false;
    iVar10 = 2;
    for (plVar24 = *(long **)(param_2 + 0x11d8); plVar24 != *(long **)(param_2 + 0x11e0);
        plVar24 = plVar24 + 3) {
      if ((((char *)plVar24[2] != (char *)0x0) && (*plVar24 != 0)) &&
         (iVar8 = strncmp((char *)plVar24[2],"APP_ARW",0x10), iVar8 == 0)) {
        lVar20 = *(long *)(*(long *)(param_2 + 0x1488) + 0x38);
        plVar15 = (long *)FUN_0036b944(lVar20,700);
        if (plVar15 == (long *)0x0) {
          uVar11 = 0;
          __src = (void *)0x0;
        }
        else {
          uVar11 = FUN_001c4d80(*(undefined8 *)(lVar20 + 0x28),*plVar15 + 4,0);
          __src = (void *)plVar15[1];
        }
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0xfe);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0x103);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0x14a);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0x201);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0x202);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),700);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0xffffc634);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0xffffc4a5);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0xffffff00);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0xffffff01);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x38),0xffffcd41);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x40),0xfe);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x40),0x14a);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x40),700);
        FUN_0036c114(*(undefined8 *)(*(long *)(param_2 + 0x1488) + 0x40),0xffffc634);
        uVar13 = _Znwm(0x1848);
        FUN_00371b88(uVar13,__s,iVar10,10,lVar12,uVar9);
        uVar17 = *(undefined8 *)(param_2 + 0x1488);
        *(undefined8 *)(param_1 + 0x1488) = uVar13;
        iVar8 = FUN_003796b0(uVar13,uVar17,param_3);
        if (iVar8 < 0) {
          return 0xfffffffe;
        }
        iVar10 = iVar10 + iVar8;
        FUN_0036d254(param_1,*(undefined8 *)(param_1 + 0x1488),0x2047,0xc);
        bVar7 = __src != (void *)0x0 && uVar11 != 0;
        if (__src != (void *)0x0 && uVar11 != 0) {
          local_290 = 0x61782f6d6f63;
          local_2a0._0_1_ = 'h';
          local_2a0._1_1_ = 't';
          local_2a0._2_1_ = 't';
          local_2a0._3_1_ = 'p';
          local_2a0._4_1_ = ':';
          local_2a0._5_1_ = '/';
          local_2a0._6_1_ = '/';
          local_2a0._7_1_ = 'n';
          local_298._0_1_ = 's';
          local_298._1_1_ = '.';
          local_298._2_1_ = 'a';
          local_298._3_1_ = 'd';
          local_298._4_1_ = 'o';
          local_298._5_1_ = 'b';
          local_298._6_1_ = 'e';
          local_298._7_1_ = '.';
          uStack_28a = 0x2f70;
          local_288 = 0x2e31;
          uStack_286 = 0x2f30;
          sVar16 = strlen((char *)&local_2a0);
          iVar8 = (int)sVar16 + 1;
          __s[iVar10] = 0xff;
          __s[(long)iVar10 + 1] = 0xe1;
          strncpy(__s + (long)iVar10 + 4,(char *)&local_2a0,(long)iVar8);
          memcpy(__s + (iVar10 + 4 + iVar8),__src,(ulong)uVar11);
          iVar8 = uVar11 + 2 + iVar8;
          __s[(long)iVar10 + 3] = (char)iVar8;
          __s[(long)iVar10 + 2] = (char)((uint)iVar8 >> 8);
          iVar10 = iVar10 + 2 + iVar8;
        }
        else {
          bVar7 = true;
        }
      }
    }
  }
  uVar13 = 0xfffffffc;
  if (bVar7) {
    uVar13 = _Znwm(0x90);
    FUN_003777ac(uVar13,__s,iVar10,lVar12,uVar9,1);
    *(undefined8 *)(param_1 + 0x14a0) = uVar13;
    iVar8 = FUN_003796b0(uVar13,0,param_3);
    iVar8 = iVar10 + iVar8;
    iVar23 = iVar8;
    if (*(char *)(param_3 + 0x44d) != '\0') {
      lVar12 = _Znwm(0x48);
      FUN_003525e8(lVar12,__s,iVar8);
      cVar6 = *(char *)(param_3 + 0x44e);
      *(long *)(param_1 + 0x14a8) = lVar12;
      if (cVar6 == '\0') {
        iVar23 = iVar8 + *(int *)(lVar12 + 0x18);
      }
      local_2a0 = *(ulong *)(param_3 + 0x28);
      local_298 = *(ulong *)(param_3 + 0x30);
      uVar13 = *(undefined8 *)(param_3 + 0x40);
      local_290 = (undefined6)*(undefined8 *)(param_3 + 0x38);
      uStack_28a = (undefined2)((ulong)*(undefined8 *)(param_3 + 0x38) >> 0x30);
      local_288 = (undefined2)uVar13;
      uStack_286 = (undefined4)((ulong)uVar13 >> 0x10);
      uStack_282 = (undefined2)((ulong)uVar13 >> 0x30);
      local_280 = *(undefined8 *)(param_3 + 0x48);
      uStack_278 = *(undefined8 *)(param_3 + 0x50);
      FUN_0036d120(param_1,&local_2a0);
    }
    puVar22 = (undefined8 *)(param_1 + 0x11f8);
    puVar14 = puVar22;
    do {
      *puVar14 = 0;
      puVar14[1] = 0;
      puVar1 = puVar14 + 6;
      puVar14[2] = 0;
      puVar14[3] = 0;
      puVar14[4] = 0;
      puVar14[5] = 0;
      puVar14 = puVar1;
    } while ((undefined8 *)(param_1 + 0x13a8) != puVar1);
    cVar6 = *(char *)(param_3 + 0x44e);
    uVar13 = *(undefined8 *)(param_3 + 0x30);
    *puVar22 = *(undefined8 *)(param_3 + 0x28);
    *(undefined8 *)(param_1 + 0x1200) = uVar13;
    uVar13 = *(undefined8 *)(param_3 + 0x40);
    *(undefined8 *)(param_1 + 0x1208) = *(undefined8 *)(param_3 + 0x38);
    *(undefined8 *)(param_1 + 0x1210) = uVar13;
    uVar13 = *(undefined8 *)(param_3 + 0x50);
    *(undefined8 *)(param_1 + 0x1218) = *(undefined8 *)(param_3 + 0x48);
    *(undefined8 *)(param_1 + 0x1220) = uVar13;
    *(int *)(param_1 + 0x1200) = iVar23;
    if (cVar6 == '\0') {
      lVar12 = 1;
      iVar23 = 0;
      *(undefined4 *)(param_1 + 0x13e0) = 0;
    }
    else {
      lVar12 = 2;
      *(undefined4 *)(param_3 + 0x240) = 0x181000;
      uVar13 = *(undefined8 *)(param_3 + 0x240);
      *(undefined8 *)(param_1 + 0x1228) = *(undefined8 *)(param_3 + 0x238);
      *(undefined8 *)(param_1 + 0x1230) = uVar13;
      uVar13 = *(undefined8 *)(param_3 + 0x250);
      *(undefined8 *)(param_1 + 0x1238) = *(undefined8 *)(param_3 + 0x248);
      *(undefined8 *)(param_1 + 0x1240) = uVar13;
      uVar13 = *(undefined8 *)(param_3 + 0x260);
      *(undefined8 *)(param_1 + 0x1248) = *(undefined8 *)(param_3 + 600);
      *(undefined8 *)(param_1 + 0x1250) = uVar13;
      *(int *)(param_1 + 0x13dc) = iVar23;
      *(undefined4 *)(param_1 + 0x13e0) = 0x181000;
    }
    uVar11 = (int)lVar12 + 1;
    if (*(long *)(param_3 + 0xf8) == 0) {
      lVar12 = param_1 + lVar12 * 0x30;
      uVar13 = *(undefined8 *)(param_3 + 0x90);
      *(undefined8 *)(lVar12 + 0x11f8) = *(undefined8 *)(param_3 + 0x88);
      *(undefined8 *)(lVar12 + 0x1200) = uVar13;
      uVar17 = *(undefined8 *)(param_3 + 0xa0);
      uVar13 = *(undefined8 *)(param_3 + 0xa8);
      uVar4 = *(undefined8 *)(param_3 + 0xb0);
      *(undefined8 *)(lVar12 + 0x1208) = *(undefined8 *)(param_3 + 0x98);
      *(undefined8 *)(lVar12 + 0x1210) = uVar17;
      *(undefined8 *)(lVar12 + 0x1218) = uVar13;
      *(undefined8 *)(lVar12 + 0x1220) = uVar4;
      FUN_00369fa0(param_1 + (ulong)uVar11 * 0x30 + 0x11f8);
      lVar12 = *(long *)(*(long *)(param_1 + 0x1488) + 0x50);
      *(uint *)(param_1 + 0x11f0) = uVar11;
      if (lVar12 != 0) {
        FUN_001c7808(lVar12,0x2044,0,0);
        FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x50),0x2044,1,0);
      }
    }
    else {
      uVar13 = *(undefined8 *)(param_3 + 0x90);
      lVar20 = param_1 + lVar12 * 0x30;
      *(undefined8 *)(lVar20 + 0x11f8) = *(undefined8 *)(param_3 + 0x88);
      *(undefined8 *)(lVar20 + 0x1200) = uVar13;
      uVar17 = *(undefined8 *)(param_3 + 0xa0);
      uVar13 = *(undefined8 *)(param_3 + 0xa8);
      uVar4 = *(undefined8 *)(param_3 + 0xb0);
      *(undefined8 *)(lVar20 + 0x1208) = *(undefined8 *)(param_3 + 0x98);
      *(undefined8 *)(lVar20 + 0x1210) = uVar17;
      *(undefined8 *)(lVar20 + 0x1218) = uVar13;
      *(undefined8 *)(lVar20 + 0x1220) = uVar4;
      uVar5 = *(uint *)(param_3 + 0x90);
      uVar3 = uVar5 + 0xfff & 0xfffff000;
      *(uint *)(lVar20 + 0x1200) = uVar3;
      memset((void *)(*(long *)(param_3 + 0x98) + (ulong)uVar5),0,(ulong)(uVar3 - uVar5));
      iVar19 = *(int *)(param_3 + 0x1e0);
      uVar21 = (ulong)uVar11;
      lVar20 = *(long *)(param_1 + 0x1488);
      if (iVar19 == 0) {
        lVar12 = *(long *)(lVar20 + 0x50);
        if (lVar12 != 0) {
          FUN_001c7808(lVar12,0x2044,0,0);
          FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x50),0x2044,1,0);
          uVar21 = (ulong)uVar11;
        }
      }
      else {
        lVar18 = param_1 + uVar21 * 0x30;
        uVar13 = *(undefined8 *)(param_3 + 0x1e0);
        *(undefined8 *)(lVar18 + 0x11f8) = *(undefined8 *)(param_3 + 0x1d8);
        *(undefined8 *)(lVar18 + 0x1200) = uVar13;
        uVar13 = *(undefined8 *)(param_3 + 0x1f0);
        *(undefined8 *)(lVar18 + 0x1208) = *(undefined8 *)(param_3 + 0x1e8);
        *(undefined8 *)(lVar18 + 0x1210) = uVar13;
        uVar13 = *(undefined8 *)(param_3 + 0x200);
        *(undefined8 *)(lVar18 + 0x1218) = *(undefined8 *)(param_3 + 0x1f8);
        *(undefined8 *)(lVar18 + 0x1220) = uVar13;
        *(uint *)(lVar18 + 0x1200) = iVar19 + 0xfffU & 0xfffff000;
        if (*(char *)(param_3 + 0x44e) == '\0') {
          iVar19 = -0xc;
        }
        else {
          iVar19 = *(int *)(param_1 + 0x1260) + -0xc;
        }
        iVar19 = *(int *)(param_1 + 0x1200) + *(int *)(param_1 + 0x1230) + iVar19;
        lVar20 = *(long *)(lVar20 + 0x50);
        if (lVar20 != 0) {
          FUN_001c7808(lVar20,0x2044,0,iVar19);
          FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x50),0x2044,1,
                       *(undefined4 *)(param_1 + (ulong)uVar11 * 0x30 + 0x1200));
        }
        uVar13 = _Znwm(0x28);
        FUN_0035aecc(uVar13,1,*(long *)(param_3 + 0x1e8),2,iVar19,
                     *(long *)(param_3 + 0x1e8) - *(long *)(param_3 + 0x1f0));
        *(undefined8 *)(param_1 + 0x14b0) = uVar13;
        FUN_0035b168(uVar13,param_3);
        uVar21 = (ulong)((int)lVar12 + 2);
      }
      lVar12 = param_1 + uVar21 * 0x30;
      uVar13 = *(undefined8 *)(param_3 + 0xf0);
      *(undefined8 *)(lVar12 + 0x11f8) = *(undefined8 *)(param_3 + 0xe8);
      *(undefined8 *)(lVar12 + 0x1200) = uVar13;
      uVar13 = *(undefined8 *)(param_3 + 0x100);
      *(undefined8 *)(lVar12 + 0x1208) = *(undefined8 *)(param_3 + 0xf8);
      *(undefined8 *)(lVar12 + 0x1210) = uVar13;
      uVar13 = *(undefined8 *)(param_3 + 0x110);
      *(undefined8 *)(lVar12 + 0x1218) = *(undefined8 *)(param_3 + 0x108);
      *(undefined8 *)(lVar12 + 0x1220) = uVar13;
      FUN_0036d1b0(param_1,0,*(undefined8 *)(param_3 + 0xf8));
      uVar11 = (int)uVar21 + 1;
      FUN_00369fa0(param_1 + (ulong)uVar11 * 0x30 + 0x11f8);
      *(uint *)(param_1 + 0x11f0) = uVar11;
    }
    iVar19 = 0;
    if (*(long *)(param_1 + 0x14a8) != 0) {
      if (*(char *)(param_3 + 0x44e) == '\0') {
        iVar19 = *(int *)(*(long *)(param_1 + 0x14a8) + 0x18);
      }
      else {
        iVar19 = *(int *)(param_1 + 0x1230);
      }
    }
    if (*(long *)(param_1 + 0x14a0) != 0) {
      iVar2 = *(int *)(param_1 + 0x1200) + *(int *)(param_1 + 0x1230);
      if (*(char *)(param_3 + 0x44e) == '\0') {
        iVar10 = (iVar2 - iVar10) + -8;
      }
      else {
        iVar10 = (iVar2 + *(int *)(param_1 + 0x1260) + -8) - iVar10;
      }
      FUN_00377864(*(long *)(param_1 + 0x14a0),iVar8 + *(int *)(param_3 + 0x90) + iVar19,
                   iVar10 + *(int *)(param_3 + 0x1e0),*(undefined4 *)(param_3 + 0xf0));
    }
    uVar13 = 0;
    if (*(long *)(param_1 + 0x14a8) != 0) {
      memset(local_480,0,0x1e0);
      memset(auStack_510,0,0x90);
      uVar11 = *(uint *)(param_1 + 0x11f0);
      local_5c8 = (undefined8 *)0x0;
      local_5c0 = 0;
      iVar10 = 0;
      local_5b8 = (undefined1 *)0x0;
      uStack_5b0 = 0;
      puVar14 = local_480;
      for (; puVar22 != (undefined8 *)(param_1 + (ulong)uVar11 * 0x30 + 0x11f8);
          puVar22 = puVar22 + 6) {
        uVar13 = puVar22[2];
        *(int *)(puVar14 + 2) = iVar10;
        *puVar14 = uVar13;
        puVar14[1] = puVar22[3];
        iVar8 = *(int *)(puVar22 + 1);
        *(int *)((long)puVar14 + 0x14) = iVar8;
        iVar10 = iVar10 + iVar8;
        *(undefined4 *)(puVar14 + 3) = *(undefined4 *)puVar22;
        puVar14 = puVar14 + 4;
      }
      if (*(char *)(param_3 + 0x44e) == '\0') {
        local_470[1] = local_470[1] - iVar19;
      }
      uVar9 = _ZN7libjpeg13GetExceptListEPN3mpr8libcrypt9cryptAreaEjPhPNS1_21EncryptOrExceptArea_tEPj
                        (param_1,local_480,(ulong)uVar11,__s,auStack_510,&uStack_5b0);
      if (*(char *)(param_3 + 0x44e) == '\0') {
        uStack_286 = 0;
        uStack_282 = 0;
        local_2a0 = (ulong)*(byte *)(param_2 + 0x14e0);
        local_298 = *(undefined8 *)(param_2 + 0x14f8);
        local_288 = *(undefined2 *)(param_2 + 0x14ea);
        local_290 = (undefined6)*(undefined8 *)(param_2 + 0x14e2);
        uStack_28a = (undefined2)((ulong)*(undefined8 *)(param_2 + 0x14e2) >> 0x30);
        _ZN9app11_sig3addEPN3mpr8libcrypt9cryptAreaEjPNS0_9TrustTimeEPN9DefStruct13UniversalTimeEjPv
                  (*(undefined8 *)(param_1 + 0x14a8),local_480,uVar9,&local_2a0,param_2 + 0x14ec,
                   *(undefined4 *)(param_2 + 0x1510),*(undefined8 *)(param_2 + 0x1508));
      }
      else {
        local_5a8 = *(undefined8 *)(param_3 + 0x2c8);
        local_5a0 = *(undefined8 *)(param_3 + 0x2d0);
        local_598 = *(undefined8 *)(param_3 + 0x2d8);
        uStack_590 = *(undefined8 *)(param_3 + 0x2e0);
        local_588 = *(undefined8 *)(param_3 + 0x2e8);
        uStack_580 = *(undefined8 *)(param_3 + 0x2f0);
        memcpy(&local_2a0,(undefined8 *)(param_3 + 0x28),0x2a0);
        local_5c0 = CONCAT44(local_5c0._4_4_,uVar9);
        local_518 = 0;
        local_578 = 0;
        uStack_570 = 0;
        local_568 = 0;
        uStack_560 = 0;
        local_558 = 0;
        uStack_550 = 0;
        local_548 = 0;
        uStack_540 = 0;
        local_538 = 0;
        uStack_530 = 0;
        local_528 = 0;
        uStack_520 = 0;
        local_5a0 = CONCAT44(local_5a0._4_4_,0x180e0c);
        local_240 = local_c0;
        local_228 = local_a8;
        local_230 = local_b0 + 1;
        local_220 = local_a0;
        uStack_218 = uStack_98;
        local_298 = local_298 & 0xffffffff00000000;
        _local_238 = CONCAT44(uStack_b4,*local_b0);
        local_e8 = 0;
        local_5c8 = local_480;
        local_5b8 = auStack_510;
        iVar10 = _ZN7libjpeg13GetAssetParamEPhmPN3mpr8libcrypt10assetParamE
                           (param_1,local_230,*local_b0,&local_578);
        if (iVar10 != 0) {
          local_5d3 = 0;
          local_5d4 = 3;
          local_5d2 = 0;
          local_5d8 = 0x2240;
          local_5d1 = 0;
          local_5d6 = 0xe080;
          local_5d0 = 0xfffffff5;
          FUN_0036cf68(&local_5d8,0xc);
          return 0xfffffff5;
        }
        local_610 = local_5a8;
        uStack_608 = local_5a0;
        local_600 = local_598;
        uStack_5f8 = uStack_590;
        local_5f0 = local_588;
        uStack_5e8 = uStack_580;
        uVar9 = _ZN4C2PA10CreateC2paEN8stillcom10BufferAreaEPN5still8common_tEPNS_13EncryptC2PA_tEbPN3mpr8libcrypt10assetParamEjPKhPKc
                          (&local_610,&local_2a0,&local_5c8,1,&local_578,iVar23,param_3 + 0x31a,
                           param_3 + 0x347);
        iVar10 = FUN_0035293c(param_3 + 0x238,&local_5a8,uVar9);
        if (iVar10 == 0) {
          return 0xfffffff4;
        }
        local_5d3 = 0;
        local_5d4 = 4;
        local_5d2 = 0;
        local_5d8 = 0x2240;
        local_5d1 = 0;
        local_5d6 = 0xe088;
        FUN_0036cf68(&local_5d8,8);
      }
      uVar13 = 0;
    }
  }
  return uVar13;
}


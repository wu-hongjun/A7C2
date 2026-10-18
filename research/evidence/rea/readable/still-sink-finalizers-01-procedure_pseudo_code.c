// Decompiled pseudocode, not original source.
// REA Evidence: ev_ce615c5d9093908a2626437863bce040911f862049f8b72ef6e9573496755e64
// Source record: research/evidence/rea/still-sink-finalizers/01-procedure_pseudo_code.json

undefined8 FUN_00368f5c(long param_1,void *param_2)

{
  int *piVar1;
  undefined8 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  char cVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  int iVar12;
  undefined4 uVar13;
  void *__s;
  undefined8 uVar14;
  undefined8 uVar15;
  long lVar16;
  int *piVar17;
  long *plVar18;
  ulong uVar19;
  ulong uVar20;
  ulong uVar21;
  long lVar22;
  int *piVar23;
  ulong uVar24;
  uint uVar25;
  int local_bcc;
  undefined8 local_bc0;
  undefined8 uStack_bb8;
  undefined8 local_bb0;
  undefined8 uStack_ba8;
  undefined8 local_ba0;
  undefined8 uStack_b98;
  int local_b84;
  undefined8 *local_b80;
  undefined8 local_b78;
  undefined1 *local_b70;
  undefined8 uStack_b68;
  undefined1 local_b60;
  undefined1 uStack_b5f;
  undefined6 uStack_b5e;
  undefined2 uStack_b58;
  undefined2 local_b56;
  undefined4 uStack_b54;
  undefined8 local_b50;
  undefined8 local_b48;
  ulong local_b40;
  undefined8 local_b38;
  ulong local_b30;
  undefined8 uStack_b28;
  undefined8 local_b20;
  undefined8 uStack_b18;
  undefined8 local_b10;
  undefined8 uStack_b08;
  undefined8 local_b00;
  undefined1 auStack_af8 [168];
  undefined8 local_a50 [2];
  int local_a40 [116];
  undefined1 auStack_870 [232];
  undefined1 auStack_788 [528];
  undefined8 local_578;
  undefined8 uStack_570;
  undefined8 local_568;
  undefined8 uStack_560;
  undefined8 local_558;
  undefined8 uStack_550;
  
  if (*(char *)((long)param_2 + 0x86b) != '\0') {
    memcpy(auStack_870,param_2,0x870);
    local_578 = *(undefined8 *)((long)param_2 + 0x388);
    uStack_570 = *(undefined8 *)((long)param_2 + 0x390);
    local_568 = *(undefined8 *)((long)param_2 + 0x398);
    uStack_560 = *(undefined8 *)((long)param_2 + 0x3a0);
    local_558 = *(undefined8 *)((long)param_2 + 0x3a8);
    uStack_550 = *(undefined8 *)((long)param_2 + 0x3b0);
    uVar14 = _Znwm(8);
    FUN_00355408();
    *(undefined8 *)(param_1 + 0x1498) = uVar14;
    iVar9 = FUN_00368cac(param_1,auStack_870);
    if (iVar9 == 0) {
      return 0xffffffff;
    }
  }
  __s = *(void **)((long)param_2 + 0xf8);
  piVar23 = (int *)(param_1 + 0x11f8);
  memset(__s,0,(ulong)*(uint *)((long)param_2 + 0xf0));
  uVar13 = *(undefined4 *)((long)param_2 + 0xf0);
  lVar16 = *(long *)((long)param_2 + 0xf8);
  lVar22 = *(long *)((long)param_2 + 0x100);
  uVar14 = _Znwm(0x26e0);
  FUN_00377e20(uVar14,__s,lVar16 - lVar22,uVar13);
  *(undefined8 *)(param_1 + 0x1488) = uVar14;
  uVar8 = FUN_0037963c(uVar14,param_2);
  uVar14 = FUN_001c7674(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x38),700,0,0);
  uVar15 = _Znwm(0x1d060);
  FUN_003798a0();
  *(undefined8 *)(param_1 + 0x1490) = uVar15;
  local_b84 = 0;
  FUN_00379c6c(uVar15,0,2,&local_b84);
  FUN_0037a978(*(undefined8 *)(param_1 + 0x1490),uVar14,local_b84,
               *(undefined8 *)((long)param_2 + 0x400),*(undefined4 *)((long)param_2 + 0x3f8),2,0,
               *(undefined1 *)((long)param_2 + 0x44c),(long)param_2 + 0x458,(long)param_2 + 0x485);
  FUN_0037a56c(*(undefined8 *)(param_1 + 0x1490),uVar14,local_b84,2,0);
  lVar16 = *(long *)(param_1 + 0x1490);
  local_b84 = *(int *)(lVar16 + 0x1d048);
  iVar12 = *(int *)(lVar16 + 0x1cff4);
  iVar9 = (int)*(undefined8 *)(lVar16 + 0x1d040) - (int)__s;
  *(int *)(param_1 + 0x13c4) = iVar9;
  iVar9 = iVar12 + iVar9;
  *(int *)(param_1 + 0x13cc) = iVar9;
  *(int *)(param_1 + 0x13d4) = iVar9 + 1;
  lVar16 = *(long *)((long)param_2 + 0x1b8);
  *(int *)(param_1 + 0x13d8) = (local_b84 + -1) - iVar12;
  *(int *)(param_1 + 0x13c8) = iVar12;
  *(undefined4 *)(param_1 + 0x13d0) = 1;
  *(undefined1 *)(lVar16 + 0xa2) = 0xff;
  *(undefined1 *)(*(long *)((long)param_2 + 0x1b8) + 0xa3) = 0xd8;
  piVar17 = piVar23;
  do {
    piVar17[0] = 0;
    piVar17[1] = 0;
    piVar17[2] = 0;
    piVar17[3] = 0;
    piVar1 = piVar17 + 0xc;
    piVar17[4] = 0;
    piVar17[5] = 0;
    piVar17[6] = 0;
    piVar17[7] = 0;
    piVar17[8] = 0;
    piVar17[9] = 0;
    piVar17[10] = 0;
    piVar17[0xb] = 0;
    piVar17 = piVar1;
  } while ((int *)(param_1 + 0x13a8) != piVar1);
  uVar10 = uVar8 + 0xfff & 0xfffff000;
  uVar14 = *(undefined8 *)((long)param_2 + 0xf0);
  *(undefined8 *)piVar23 = *(undefined8 *)((long)param_2 + 0xe8);
  *(undefined8 *)(param_1 + 0x1200) = uVar14;
  uVar15 = *(undefined8 *)((long)param_2 + 0x100);
  uVar14 = *(undefined8 *)((long)param_2 + 0x108);
  uVar2 = *(undefined8 *)((long)param_2 + 0x110);
  *(undefined8 *)(param_1 + 0x1208) = *(undefined8 *)((long)param_2 + 0xf8);
  *(undefined8 *)(param_1 + 0x1210) = uVar15;
  *(undefined8 *)(param_1 + 0x1218) = uVar14;
  *(undefined8 *)(param_1 + 0x1220) = uVar2;
  *(uint *)(param_1 + 0x1200) = uVar10;
  memset((void *)((long)__s + (ulong)uVar8),0,(ulong)(uVar10 - uVar8));
  iVar9 = *(int *)(param_1 + 0x1200);
  lVar16 = *(long *)(*(long *)(param_1 + 0x1488) + 0x50);
  if (lVar16 == 0) {
    *(undefined4 *)(param_1 + 0xbe0) = 0;
    *(undefined8 *)(param_1 + 0xbe8) = 0;
  }
  else {
    lVar16 = FUN_0036b944(lVar16,0x2044);
    uVar14 = FUN_001c7674(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x50),0x2010,0,local_a50);
    *(undefined8 *)(param_1 + 0xbe8) = uVar14;
    *(undefined4 *)(param_1 + 0xbe0) = (undefined4)local_a50[0];
    if (lVar16 != 0) {
      lVar22 = *(long *)((long)param_2 + 0x2a8);
      lVar16 = *(long *)((long)param_2 + 0x2b0);
      uVar14 = _Znwm(0x28);
      FUN_0035aecc(uVar14,1,lVar22,4,iVar9,lVar22 - lVar16);
      *(undefined8 *)(param_1 + 0x14a0) = uVar14;
      iVar12 = FUN_0035aff8(uVar14,param_2);
      uVar20 = 2;
      uVar8 = *(int *)((long)param_2 + 0x2a0) + 0xfffU & 0xfffff000;
      uVar14 = *(undefined8 *)((long)param_2 + 0x2a0);
      *(undefined8 *)(param_1 + 0x1228) = *(undefined8 *)((long)param_2 + 0x298);
      *(undefined8 *)(param_1 + 0x1230) = uVar14;
      uVar15 = *(undefined8 *)((long)param_2 + 0x2b0);
      uVar14 = *(undefined8 *)((long)param_2 + 0x2b8);
      uVar2 = *(undefined8 *)((long)param_2 + 0x2c0);
      *(undefined8 *)(param_1 + 0x1238) = *(undefined8 *)((long)param_2 + 0x2a8);
      *(undefined8 *)(param_1 + 0x1240) = uVar15;
      *(undefined8 *)(param_1 + 0x1248) = uVar14;
      *(undefined8 *)(param_1 + 0x1250) = uVar2;
      lVar16 = *(long *)((long)param_2 + 0x2a8);
      *(uint *)(param_1 + 0x1230) = uVar8;
      memset((void *)(lVar16 + iVar12),0,(ulong)(uVar8 - iVar12));
      local_bcc = iVar9 + *(int *)(param_1 + 0x1230);
      lVar16 = *(long *)(*(long *)(param_1 + 0x1488) + 0x50);
      if (lVar16 != 0) {
        FUN_001c7808(lVar16,0x2044,0,iVar9);
        FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x50),0x2044,1,uVar8);
      }
      goto LAB_003691a0;
    }
  }
  uVar8 = 0;
  uVar20 = 1;
  local_bcc = iVar9;
LAB_003691a0:
  iVar9 = uVar8 + uVar10;
  if ((*(char *)((long)param_2 + 0x868) == '\0') || (*(char *)((long)param_2 + 0x86b) != '\0')) {
    uVar24 = 0;
    uVar8 = 0;
    uVar21 = uVar20;
  }
  else {
    lVar16 = param_1 + uVar20 * 0x30;
    uVar14 = *(undefined8 *)((long)param_2 + 0x2d0);
    *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x2c8);
    *(undefined8 *)(lVar16 + 0x1200) = uVar14;
    uVar14 = *(undefined8 *)((long)param_2 + 0x2e0);
    *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x2d8);
    *(undefined8 *)(lVar16 + 0x1210) = uVar14;
    uVar14 = *(undefined8 *)((long)param_2 + 0x2f0);
    *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x2e8);
    *(undefined8 *)(lVar16 + 0x1220) = uVar14;
    uVar10 = *(uint *)((long)param_2 + 0x2d0);
    uVar8 = uVar10 + 0xfff & 0xfffff000;
    *(uint *)(lVar16 + 0x1200) = uVar8;
    memset((void *)(*(long *)(lVar16 + 0x1208) + (ulong)uVar10),0,(ulong)(uVar8 - uVar10));
    local_bcc = local_bcc + *(int *)(lVar16 + 0x1200);
    FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x38),0xffffff00,0,iVar9);
    FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x38),0xffffff01,0,uVar8);
    uVar21 = (ulong)((int)uVar20 + 1);
    uVar24 = uVar20;
  }
  uVar20 = (ulong)((int)uVar21 + 1);
  lVar16 = param_1 + uVar21 * 0x30;
  uVar14 = *(undefined8 *)((long)param_2 + 0x1b0);
  *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x1a8);
  *(undefined8 *)(lVar16 + 0x1200) = uVar14;
  uVar14 = *(undefined8 *)((long)param_2 + 0x1c0);
  *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x1b8);
  *(undefined8 *)(lVar16 + 0x1210) = uVar14;
  uVar14 = *(undefined8 *)((long)param_2 + 0x1d0);
  *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x1c8);
  *(undefined8 *)(lVar16 + 0x1220) = uVar14;
  uVar25 = *(uint *)((long)param_2 + 0x1b0);
  uVar10 = uVar25 + 0xfff & 0xfffff000;
  *(uint *)(lVar16 + 0x1200) = uVar10;
  memset((void *)(*(long *)(lVar16 + 0x1208) + (ulong)uVar25),0,(ulong)(uVar10 - uVar25));
  local_bcc = local_bcc + *(int *)(lVar16 + 0x1200);
  FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x38),0x201,0,uVar8 + iVar9 + 0xa2);
  FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x38),0x202,0,
               *(int *)((long)param_2 + 0x1b0) + -0xa2);
  iVar12 = uVar8 + iVar9 + uVar10;
  uVar10 = 0;
  if (*(long *)(*(long *)(param_1 + 0x1488) + 0x1848) != 0) {
    lVar16 = param_1 + uVar20 * 0x30;
    uVar14 = *(undefined8 *)((long)param_2 + 0x150);
    *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x148);
    *(undefined8 *)(lVar16 + 0x1200) = uVar14;
    uVar14 = *(undefined8 *)((long)param_2 + 0x160);
    *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x158);
    *(undefined8 *)(lVar16 + 0x1210) = uVar14;
    uVar14 = *(undefined8 *)((long)param_2 + 0x170);
    *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x168);
    *(undefined8 *)(lVar16 + 0x1220) = uVar14;
    uVar25 = *(uint *)((long)param_2 + 0x150);
    uVar10 = uVar25 + 0xfff & 0xfffff000;
    *(uint *)(lVar16 + 0x1200) = uVar10;
    memset((void *)(*(long *)(lVar16 + 0x1208) + (ulong)uVar25),0,(ulong)(uVar10 - uVar25));
    local_bcc = local_bcc + *(int *)(lVar16 + 0x1200);
    uVar20 = (ulong)((int)uVar21 + 2);
    FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x1848),0x201,0,iVar12);
    FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x1848),0x202,0,
                 *(undefined4 *)((long)param_2 + 0x150));
  }
  lVar16 = param_1 + uVar20 * 0x30;
  uVar14 = *(undefined8 *)((long)param_2 + 0x1e0);
  *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x1d8);
  *(undefined8 *)(lVar16 + 0x1200) = uVar14;
  uVar14 = *(undefined8 *)((long)param_2 + 0x1f0);
  *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x1e8);
  *(undefined8 *)(lVar16 + 0x1210) = uVar14;
  uVar14 = *(undefined8 *)((long)param_2 + 0x200);
  *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x1f8);
  *(undefined8 *)(lVar16 + 0x1220) = uVar14;
  uVar3 = *(uint *)((long)param_2 + 0x1e0);
  uVar25 = uVar3 + 0xfff & 0xfffff000;
  *(uint *)(lVar16 + 0x1200) = uVar25;
  memset((void *)(*(long *)(lVar16 + 0x1208) + (ulong)uVar3),0,(ulong)(uVar25 - uVar3));
  uVar21 = (ulong)((int)uVar20 + 1);
  local_bcc = local_bcc + *(int *)(lVar16 + 0x1200);
  iVar12 = iVar12 + uVar10;
  FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x1850),0x111,0,iVar12);
  lVar16 = *(long *)(*(long *)(param_1 + 0x1488) + 0x1850);
  plVar18 = (long *)FUN_0036b944(lVar16,0x144);
  if (plVar18 != (long *)0x0) {
    uVar10 = FUN_001c4d80(*(undefined8 *)(lVar16 + 0x28),*plVar18 + 4,0);
    lVar16 = plVar18[1];
    if (lVar16 != 0) {
      if (0x1000 < uVar10) {
        uVar10 = 0x1000;
      }
      for (uVar25 = 0; uVar25 != uVar10; uVar25 = uVar25 + 1) {
        uVar14 = *(undefined8 *)(*(long *)(*(long *)(param_1 + 0x1488) + 0x1850) + 0x28);
        iVar11 = FUN_001c4d80(uVar14,lVar16,uVar25);
        FUN_001c4d9c(uVar14,lVar16,iVar12 + iVar11,uVar25);
      }
    }
  }
  if (*(char *)((long)param_2 + 0x86b) == '\0') {
    *(undefined4 *)(param_1 + 0x13e0) = 0;
    local_bcc = 0;
  }
  else {
    lVar22 = param_1 + uVar21 * 0x30;
    *(undefined4 *)((long)param_2 + 0x300) = 0x180000;
    uVar14 = *(undefined8 *)((long)param_2 + 0x300);
    *(undefined8 *)(lVar22 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x2f8);
    *(undefined8 *)(lVar22 + 0x1200) = uVar14;
    uVar14 = *(undefined8 *)((long)param_2 + 0x310);
    *(undefined8 *)(lVar22 + 0x1208) = *(undefined8 *)((long)param_2 + 0x308);
    *(undefined8 *)(lVar22 + 0x1210) = uVar14;
    uVar14 = *(undefined8 *)((long)param_2 + 800);
    lVar16 = *(long *)(param_1 + 0x1488);
    *(undefined8 *)(lVar22 + 0x1218) = *(undefined8 *)((long)param_2 + 0x318);
    *(undefined8 *)(lVar22 + 0x1220) = uVar14;
    uVar21 = (ulong)((int)uVar20 + 2);
    uVar14 = *(undefined8 *)(lVar16 + 0x38);
    *(int *)(param_1 + 0x13dc) = local_bcc;
    *(undefined4 *)(param_1 + 0x13e0) = 0x180000;
    FUN_001c7808(uVar14,0xffffcd41,0,local_bcc);
    uVar13 = *(undefined4 *)(param_1 + 0x13e0);
    lVar16 = *(long *)(*(long *)(param_1 + 0x1488) + 0x38);
    plVar18 = (long *)FUN_0036b944(lVar16,0xffffcd41);
    if (plVar18 != (long *)0x0) {
      FUN_001c4d9c(*(undefined8 *)(lVar16 + 0x28),*plVar18 + 4,uVar13,0);
    }
  }
  lVar16 = param_1 + uVar21 * 0x30;
  *(undefined8 *)(lVar16 + 0x11f8) = 0;
  *(undefined8 *)(lVar16 + 0x1200) = 0;
  *(undefined8 *)(lVar16 + 0x1208) = 0;
  *(undefined8 *)(lVar16 + 0x1210) = 0;
  *(undefined8 *)(lVar16 + 0x1218) = 0;
  *(undefined8 *)(lVar16 + 0x1220) = 0;
  cVar7 = *(char *)((long)param_2 + 0x868);
  *(int *)(param_1 + 0x11f0) = (int)uVar21;
  if (cVar7 != '\0') {
    memset(local_a50,0,0x1e0);
    memset(auStack_af8,0,0xa8);
    local_b80 = (undefined8 *)0x0;
    local_b78 = 0;
    local_b70 = (undefined1 *)0x0;
    uStack_b68 = 0;
    iVar12 = 0;
    iVar11 = 0;
    uVar20 = 0;
    do {
      if (((int)uVar24 == iVar12) && (*(char *)((long)param_2 + 0x86b) == '\0')) {
        iVar4 = piVar23[2];
        uVar19 = uVar20;
      }
      else {
        iVar5 = *piVar23;
        uVar19 = (ulong)((int)uVar20 + 1);
        local_a50[uVar20 * 4] = *(undefined8 *)(piVar23 + 4);
        local_a50[uVar20 * 4 + 1] = *(undefined8 *)(piVar23 + 6);
        iVar4 = piVar23[2];
        local_a40[uVar20 * 8] = iVar11;
        local_a40[uVar20 * 8 + 1] = iVar4;
        local_a40[uVar20 * 8 + 2] = iVar5;
      }
      iVar11 = iVar11 + iVar4;
      iVar12 = iVar12 + 1;
      piVar23 = piVar23 + 0xc;
      uVar20 = uVar19;
    } while ((int)uVar21 != iVar12);
    uVar13 = _ZN6libarw13GetExceptListEPN3mpr8libcrypt9cryptAreaEjPhPNS1_21EncryptOrExceptArea_tEPj
                       (param_1,local_a50,uVar19,__s,auStack_af8,&uStack_b68);
    if (*(char *)((long)param_2 + 0x86b) == '\0') {
      uVar14 = _Znwm(0x28);
      FUN_00370168(uVar14,*(undefined8 *)(param_1 + uVar24 * 0x30 + 0x1208),uVar8);
      uVar6 = *(undefined4 *)((long)param_2 + 0x438);
      uVar15 = *(undefined8 *)((long)param_2 + 0x440);
      *(undefined8 *)(param_1 + 0x14a8) = uVar14;
      _ZN12libsignature3addEPN3mpr8libcrypt9cryptAreaEjjPNS0_9TrustTimeEPN9DefStruct13UniversalTimeEjPv
                (uVar14,local_a50,uVar13,iVar9,(long)param_2 + 0x408,(long)param_2 + 0x428,uVar6,
                 uVar15);
    }
    else {
      local_b60 = 0;
      uStack_b5f = 0;
      uStack_b5e = 0;
      uStack_b58 = 0;
      local_b56 = 0;
      uStack_b54 = 0;
      local_b78 = CONCAT44(local_b78._4_4_,uVar13);
      local_b50 = 0;
      local_b48 = 0;
      local_b00 = 0;
      local_b38 = 0;
      local_b30 = 0;
      uStack_b28 = 0;
      local_b20 = 0;
      uStack_b18 = 0;
      local_b10 = 0;
      uStack_b08 = 0;
      uVar8 = *(uint *)((long)param_2 + 0x438);
      if (uVar8 != 0) {
        local_b30 = (ulong)uVar8;
        local_b38 = *(undefined8 *)((long)param_2 + 0x440);
      }
      local_b40 = (ulong)(uVar8 != 0);
      local_b80 = local_a50;
      local_b70 = auStack_af8;
      if (*(char *)((long)param_2 + 0x408) != '\0') {
        local_b60 = 1;
        uStack_b5e = (undefined6)*(undefined8 *)((long)param_2 + 0x418);
        uStack_b58 = (undefined2)((ulong)*(undefined8 *)((long)param_2 + 0x418) >> 0x30);
        local_b56 = *(undefined2 *)((long)param_2 + 0x420);
        _ZN3mpr8timeconv33ConvLocalTimeToTrustUniversalTimeEPN9DefStruct9LocalTimeElPNS1_13UniversalTimeE
                  ((long)param_2 + 0x3b8,*(undefined8 *)((long)param_2 + 0x410),&uStack_b54);
        local_b48 = *(undefined8 *)((long)param_2 + 0x410);
      }
      local_bc0 = *(undefined8 *)((long)param_2 + 0x2f8);
      uStack_bb8 = *(undefined8 *)((long)param_2 + 0x300);
      local_bb0 = *(undefined8 *)((long)param_2 + 0x308);
      uStack_ba8 = *(undefined8 *)((long)param_2 + 0x310);
      local_ba0 = *(undefined8 *)((long)param_2 + 0x318);
      uStack_b98 = *(undefined8 *)((long)param_2 + 800);
      _ZN4C2PA10CreateC2paEN8stillcom10BufferAreaEPN5still8common_tEPNS_13EncryptC2PA_tEbPN3mpr8libcrypt10assetParamEjPKhPKc
                (&local_bc0,auStack_788,&local_b80,0,&local_b60,local_bcc,(long)param_2 + 0x485,
                 (long)param_2 + 0x4df);
    }
  }
  return 0;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_83b1e4b89f0c8fd6fdbf127c1b90d7ff23f177603538018afde286e12279f892
// Source record: research/evidence/rea/still-sink-finalizers/00-procedure_pseudo_code.json

undefined8 FUN_0036e898(long param_1,void *param_2)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  char cVar7;
  byte bVar8;
  int iVar9;
  undefined4 uVar10;
  int iVar11;
  int iVar12;
  undefined8 uVar13;
  long lVar14;
  undefined8 *puVar15;
  long lVar16;
  long lVar17;
  long lVar18;
  long lVar19;
  int iVar20;
  undefined1 *__s;
  undefined8 *puVar21;
  uint uVar22;
  uint uVar23;
  int local_bd4;
  undefined8 local_bd0;
  undefined8 uStack_bc8;
  undefined8 local_bc0;
  undefined8 uStack_bb8;
  undefined8 local_bb0;
  undefined8 uStack_ba8;
  int local_b9c;
  undefined8 *local_b98;
  undefined8 local_b90;
  undefined1 *local_b88;
  undefined8 uStack_b80;
  undefined8 local_b78;
  undefined8 local_b70;
  undefined8 local_b68;
  undefined8 uStack_b60;
  undefined8 local_b58;
  undefined8 uStack_b50;
  undefined1 local_b48;
  undefined1 uStack_b47;
  undefined2 uStack_b46;
  undefined1 uStack_b44;
  undefined1 uStack_b43;
  undefined1 uStack_b42;
  undefined1 uStack_b41;
  undefined2 local_b40;
  undefined2 uStack_b3e;
  undefined4 local_b3c;
  undefined8 local_b38;
  undefined8 local_b30;
  ulong local_b28;
  undefined8 local_b20;
  ulong local_b18;
  undefined8 uStack_b10;
  undefined8 local_b08;
  undefined8 uStack_b00;
  undefined8 local_af8;
  undefined8 uStack_af0;
  undefined8 local_ae8;
  undefined1 auStack_ae0 [144];
  undefined8 local_a50 [2];
  int local_a40 [116];
  undefined1 auStack_870 [232];
  undefined1 auStack_788 [96];
  undefined8 local_728;
  undefined8 uStack_720;
  undefined8 local_718;
  undefined8 uStack_710;
  undefined8 local_708;
  undefined8 uStack_700;
  undefined8 local_548;
  undefined8 uStack_540;
  undefined8 local_538;
  undefined8 uStack_530;
  undefined8 local_528;
  undefined8 uStack_520;
  undefined8 local_4e8;
  undefined8 uStack_4e0;
  undefined8 local_4d8;
  undefined8 uStack_4d0;
  undefined8 local_4c8;
  undefined8 uStack_4c0;
  
  local_b78 = 0;
  local_b70 = 0;
  local_b68 = 0;
  uStack_b60 = 0;
  local_b58 = 0;
  uStack_b50 = 0;
  if (*(char *)((long)param_2 + 0x86b) != '\0') {
    memcpy(auStack_870,param_2,0x870);
    local_548 = local_728;
    uStack_540 = uStack_720;
    local_538 = local_718;
    uStack_530 = uStack_710;
    local_528 = local_708;
    uStack_520 = uStack_700;
    local_b78 = local_4e8;
    local_b70 = uStack_4e0;
    local_b68 = local_4d8;
    uStack_b60 = uStack_4d0;
    local_b58 = local_4c8;
    uStack_b50 = uStack_4c0;
    uVar13 = _Znwm(8);
    FUN_00355408();
    *(undefined8 *)(param_1 + 0x1498) = uVar13;
    iVar11 = FUN_0036e5ec(param_1,auStack_870);
    if (iVar11 == 0) {
      return 0xffffffff;
    }
  }
  __s = *(undefined1 **)((long)param_2 + 0xf8);
  memset(__s,0,(ulong)*(uint *)((long)param_2 + 0xf0));
  uVar10 = *(undefined4 *)((long)param_2 + 0xf0);
  lVar19 = *(long *)((long)param_2 + 0xf8) - *(long *)((long)param_2 + 0x100);
  *__s = 0xff;
  __s[1] = 0xd8;
  uVar13 = _Znwm(0x1848);
  FUN_00371b88(uVar13,__s,2,10,lVar19,uVar10);
  *(undefined8 *)(param_1 + 0x1488) = uVar13;
  iVar9 = FUN_0037963c(uVar13,param_2);
  FUN_0036d254(param_1,*(undefined8 *)(param_1 + 0x1488),0x2047,0xc);
  uVar13 = _Znwm(0x1d060);
  FUN_003798a0();
  *(undefined8 *)(param_1 + 0x1490) = uVar13;
  local_b9c = 0;
  FUN_00379c6c(uVar13,0,1,&local_b9c);
  FUN_0037a978(*(undefined8 *)(param_1 + 0x1490),__s + (iVar9 + 2),local_b9c,
               *(undefined8 *)((long)param_2 + 0x400),*(undefined4 *)((long)param_2 + 0x3f8),1,0,
               *(undefined1 *)((long)param_2 + 0x44c),(long)param_2 + 0x458,(long)param_2 + 0x485);
  iVar11 = iVar9 + 2 + local_b9c;
  FUN_0037a56c(*(undefined8 *)(param_1 + 0x1490),__s + (iVar9 + 2),local_b9c,1,0);
  lVar14 = *(long *)(param_1 + 0x1490);
  iVar20 = *(int *)(lVar14 + 0x1cff4);
  local_b9c = *(int *)(lVar14 + 0x1d048);
  iVar9 = (int)*(undefined8 *)(lVar14 + 0x1d040) - (int)__s;
  *(int *)(param_1 + 0x13c8) = iVar20;
  *(int *)(param_1 + 0x13c4) = iVar9;
  iVar9 = iVar20 + iVar9;
  *(int *)(param_1 + 0x13cc) = iVar9;
  *(undefined4 *)(param_1 + 0x13d0) = 1;
  *(int *)(param_1 + 0x13d4) = iVar9 + 1;
  *(int *)(param_1 + 0x13d8) = (local_b9c + -1) - iVar20;
  uVar13 = _Znwm(0x90);
  FUN_003777ac(uVar13,__s,iVar11,lVar19,uVar10,
               (*(byte *)((long)param_2 + 0x86b) ^ 1) & *(byte *)((long)param_2 + 0x868) ^ 1);
  *(undefined8 *)(param_1 + 0x14a0) = uVar13;
  iVar9 = FUN_0037963c(uVar13,param_2);
  iVar9 = iVar11 + iVar9;
  if ((*(char *)((long)param_2 + 0x868) != '\0') && (*(char *)((long)param_2 + 0x86b) == '\0')) {
    lVar14 = _Znwm(0x48);
    FUN_003525e8(lVar14,__s,iVar9);
    *(long *)(param_1 + 0x14a8) = lVar14;
    iVar9 = iVar9 + *(int *)(lVar14 + 0x18);
  }
  lVar19 = 0;
  lVar14 = *(long *)(*(long *)(param_1 + 0x1488) + 0x50);
  if (lVar14 != 0) {
    lVar19 = FUN_0036b944(lVar14,0x2044);
  }
  puVar21 = (undefined8 *)(param_1 + 0x11f8);
  puVar15 = puVar21;
  do {
    *puVar15 = 0;
    puVar15[1] = 0;
    puVar1 = puVar15 + 6;
    puVar15[2] = 0;
    puVar15[3] = 0;
    puVar15[4] = 0;
    puVar15[5] = 0;
    puVar15 = puVar1;
  } while ((undefined8 *)(param_1 + 0x13a8) != puVar1);
  uVar13 = *(undefined8 *)((long)param_2 + 0xf0);
  *puVar21 = *(undefined8 *)((long)param_2 + 0xe8);
  *(undefined8 *)(param_1 + 0x1200) = uVar13;
  uVar13 = *(undefined8 *)((long)param_2 + 0x100);
  *(undefined8 *)(param_1 + 0x1208) = *(undefined8 *)((long)param_2 + 0xf8);
  *(undefined8 *)(param_1 + 0x1210) = uVar13;
  uVar13 = *(undefined8 *)((long)param_2 + 0x110);
  cVar7 = *(char *)((long)param_2 + 0x86b);
  *(undefined8 *)(param_1 + 0x1218) = *(undefined8 *)((long)param_2 + 0x108);
  *(undefined8 *)(param_1 + 0x1220) = uVar13;
  *(int *)(param_1 + 0x1200) = iVar9;
  if (cVar7 == '\0') {
    lVar14 = 1;
    *(undefined4 *)(param_1 + 0x13e0) = 0;
    local_bd4 = 0;
    iVar20 = iVar9;
  }
  else {
    lVar14 = 2;
    *(undefined4 *)((long)param_2 + 0x300) = 0x181000;
    uVar13 = *(undefined8 *)((long)param_2 + 0x300);
    *(undefined8 *)(param_1 + 0x1228) = *(undefined8 *)((long)param_2 + 0x2f8);
    *(undefined8 *)(param_1 + 0x1230) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x310);
    *(undefined8 *)(param_1 + 0x1238) = *(undefined8 *)((long)param_2 + 0x308);
    *(undefined8 *)(param_1 + 0x1240) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 800);
    *(undefined8 *)(param_1 + 0x1248) = *(undefined8 *)((long)param_2 + 0x318);
    *(undefined8 *)(param_1 + 0x1250) = uVar13;
    *(int *)(param_1 + 0x13dc) = iVar9;
    *(undefined4 *)(param_1 + 0x13e0) = 0x181000;
    iVar20 = iVar9 + 0x181000;
    local_bd4 = iVar9;
  }
  iVar9 = (int)lVar14;
  uVar22 = iVar9 + 1;
  puVar15 = (undefined8 *)((long)param_2 + 0x148);
  bVar8 = 0;
  if ((*(long *)((long)param_2 + 0x1b8) == 0) || (bVar8 = 1, lVar19 != 0)) {
    if (!(bool)(bVar8 & lVar19 != 0)) {
      uVar13 = *(undefined8 *)((long)param_2 + 0x150);
      lVar16 = param_1 + lVar14 * 0x30;
      *(undefined8 *)(lVar16 + 0x11f8) = *puVar15;
      *(undefined8 *)(lVar16 + 0x1200) = uVar13;
      uVar2 = *(undefined8 *)((long)param_2 + 0x160);
      uVar13 = *(undefined8 *)((long)param_2 + 0x168);
      uVar3 = *(undefined8 *)((long)param_2 + 0x170);
      *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x158);
      *(undefined8 *)(lVar16 + 0x1210) = uVar2;
      *(undefined8 *)(lVar16 + 0x1218) = uVar13;
      *(undefined8 *)(lVar16 + 0x1220) = uVar3;
      FUN_00369fa0(param_1 + (ulong)uVar22 * 0x30 + 0x11f8);
      *(uint *)(param_1 + 0x11f0) = uVar22;
      goto LAB_0036f120;
    }
    uVar13 = *(undefined8 *)((long)param_2 + 0x150);
    lVar17 = param_1 + lVar14 * 0x30;
    *(undefined8 *)(lVar17 + 0x11f8) = *puVar15;
    *(undefined8 *)(lVar17 + 0x1200) = uVar13;
    uVar2 = *(undefined8 *)((long)param_2 + 0x160);
    uVar13 = *(undefined8 *)((long)param_2 + 0x168);
    uVar3 = *(undefined8 *)((long)param_2 + 0x170);
    *(undefined8 *)(lVar17 + 0x1208) = *(undefined8 *)((long)param_2 + 0x158);
    *(undefined8 *)(lVar17 + 0x1210) = uVar2;
    *(undefined8 *)(lVar17 + 0x1218) = uVar13;
    *(undefined8 *)(lVar17 + 0x1220) = uVar3;
    uVar5 = *(uint *)((long)param_2 + 0x150);
    lVar16 = *(long *)((long)param_2 + 0x158);
    uVar23 = uVar5 + 0xfff & 0xfffff000;
    *(uint *)(lVar17 + 0x1200) = uVar23;
    memset((void *)(lVar16 + (ulong)uVar5),0,(ulong)(uVar23 - uVar5));
    lVar16 = *(long *)((long)param_2 + 0x2a8);
    lVar18 = *(long *)((long)param_2 + 0x2b0);
    iVar6 = *(int *)(lVar17 + 0x1200);
    uVar13 = _Znwm(0x28);
    iVar6 = iVar20 + iVar6 + -0xc;
    FUN_0035aecc(uVar13,1,lVar16,4,iVar6,lVar16 - lVar18);
    *(undefined8 *)(param_1 + 0x14b0) = uVar13;
    iVar12 = FUN_0035aff8(uVar13,param_2);
    lVar17 = param_1 + (ulong)uVar22 * 0x30;
    uVar13 = *(undefined8 *)((long)param_2 + 0x2a0);
    *(undefined8 *)(lVar17 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x298);
    *(undefined8 *)(lVar17 + 0x1200) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x2b0);
    *(undefined8 *)(lVar17 + 0x1208) = *(undefined8 *)((long)param_2 + 0x2a8);
    *(undefined8 *)(lVar17 + 0x1210) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x2c0);
    *(undefined8 *)(lVar17 + 0x1218) = *(undefined8 *)((long)param_2 + 0x2b8);
    *(undefined8 *)(lVar17 + 0x1220) = uVar13;
    lVar16 = *(long *)((long)param_2 + 0x2a8);
    uVar23 = *(int *)((long)param_2 + 0x2a0) + 0xfffU & 0xfffff000;
    *(uint *)(lVar17 + 0x1200) = uVar23;
    memset((void *)(lVar16 + iVar12),0,(ulong)(uVar23 - iVar12));
    lVar16 = *(long *)(*(long *)(param_1 + 0x1488) + 0x50);
    if (lVar16 != 0) {
      FUN_001c7808(lVar16,0x2044,0,iVar6);
      FUN_001c7808(*(undefined8 *)(*(long *)(param_1 + 0x1488) + 0x50),0x2044,1,
                   *(undefined4 *)(lVar17 + 0x1200));
    }
    uVar23 = iVar9 + 4;
    memcpy(*(void **)((long)param_2 + 0x128),*(void **)((long)param_2 + 0x1b8),0x1000);
    FUN_0036d1b0(param_1,param_2,*(undefined8 *)((long)param_2 + 0x128));
    lVar16 = param_1 + (ulong)(iVar9 + 2) * 0x30;
    uVar13 = *(undefined8 *)((long)param_2 + 0x120);
    *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x118);
    *(undefined8 *)(lVar16 + 0x1200) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x130);
    *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x128);
    *(undefined8 *)(lVar16 + 0x1210) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x140);
    *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x138);
    *(undefined8 *)(lVar16 + 0x1220) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x1b0);
    lVar16 = param_1 + (ulong)(iVar9 + 3) * 0x30;
    *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x1a8);
    *(undefined8 *)(lVar16 + 0x1200) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x1d0);
    *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x1c8);
    *(undefined8 *)(lVar16 + 0x1220) = uVar13;
    *(int *)(lVar16 + 0x11fc) = *(int *)(lVar16 + 0x11fc) + 0x1000;
    *(int *)(lVar16 + 0x1200) = *(int *)((long)param_2 + 0x1b0) + -0x1000;
    *(long *)(lVar16 + 0x1208) = *(long *)((long)param_2 + 0x1b8) + 0x1000;
    *(long *)(lVar16 + 0x1210) = *(long *)((long)param_2 + 0x1c0) + 0x1000;
  }
  else {
    uVar23 = iVar9 + 3;
    lVar17 = param_1 + lVar14 * 0x30;
    uVar13 = *(undefined8 *)((long)param_2 + 0x150);
    *(undefined8 *)(lVar17 + 0x11f8) = *puVar15;
    *(undefined8 *)(lVar17 + 0x1200) = uVar13;
    uVar2 = *(undefined8 *)((long)param_2 + 0x160);
    uVar13 = *(undefined8 *)((long)param_2 + 0x168);
    uVar3 = *(undefined8 *)((long)param_2 + 0x170);
    *(undefined8 *)(lVar17 + 0x1208) = *(undefined8 *)((long)param_2 + 0x158);
    *(undefined8 *)(lVar17 + 0x1210) = uVar2;
    *(undefined8 *)(lVar17 + 0x1218) = uVar13;
    *(undefined8 *)(lVar17 + 0x1220) = uVar3;
    uVar4 = *(uint *)((long)param_2 + 0x150);
    lVar16 = *(long *)((long)param_2 + 0x158);
    uVar5 = uVar4 + 0xfff & 0xfffff000;
    *(uint *)(lVar17 + 0x1200) = uVar5;
    memset((void *)(lVar16 + (ulong)uVar4),0,(ulong)(uVar5 - uVar4));
    memcpy(*(void **)((long)param_2 + 0x128),*(void **)((long)param_2 + 0x1b8),0x1000);
    FUN_0036d1b0(param_1,param_2,*(undefined8 *)((long)param_2 + 0x128));
    lVar16 = param_1 + (ulong)uVar22 * 0x30;
    uVar13 = *(undefined8 *)((long)param_2 + 0x120);
    *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x118);
    *(undefined8 *)(lVar16 + 0x1200) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x130);
    *(undefined8 *)(lVar16 + 0x1208) = *(undefined8 *)((long)param_2 + 0x128);
    *(undefined8 *)(lVar16 + 0x1210) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x140);
    *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x138);
    *(undefined8 *)(lVar16 + 0x1220) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x1b0);
    lVar16 = param_1 + (ulong)(iVar9 + 2) * 0x30;
    *(undefined8 *)(lVar16 + 0x11f8) = *(undefined8 *)((long)param_2 + 0x1a8);
    *(undefined8 *)(lVar16 + 0x1200) = uVar13;
    uVar13 = *(undefined8 *)((long)param_2 + 0x1d0);
    *(undefined8 *)(lVar16 + 0x1218) = *(undefined8 *)((long)param_2 + 0x1c8);
    *(undefined8 *)(lVar16 + 0x1220) = uVar13;
    *(int *)(lVar16 + 0x11fc) = *(int *)(lVar16 + 0x11fc) + 0x1000;
    *(int *)(lVar16 + 0x1200) = *(int *)((long)param_2 + 0x1b0) + -0x1000;
    *(long *)(lVar16 + 0x1208) = *(long *)((long)param_2 + 0x1b8) + 0x1000;
    *(long *)(lVar16 + 0x1210) = *(long *)((long)param_2 + 0x1c0) + 0x1000;
  }
  FUN_00369fa0(param_1 + (ulong)uVar23 * 0x30 + 0x11f8);
  *(uint *)(param_1 + 0x11f0) = uVar23;
LAB_0036f120:
  iVar11 = ((iVar20 + *(int *)(param_1 + lVar14 * 0x30 + 0x1200)) - iVar11) + -8;
  if (*(long *)((long)param_2 + 0x1b8) != 0 && lVar19 != 0) {
    iVar11 = iVar11 + *(int *)(param_1 + (ulong)uVar22 * 0x30 + 0x1200);
  }
  FUN_00377864(*(undefined8 *)(param_1 + 0x14a0),iVar20 + *(int *)((long)param_2 + 0x150),iVar11,
               *(undefined4 *)((long)param_2 + 0x1b0));
  if (*(char *)((long)param_2 + 0x868) != '\0') {
    iVar11 = 0;
    memset(local_a50,0,0x1e0);
    memset(auStack_ae0,0,0x90);
    local_b98 = (undefined8 *)0x0;
    local_b90 = 0;
    local_b88 = (undefined1 *)0x0;
    uStack_b80 = 0;
    puVar15 = local_a50;
    for (uVar22 = 0; uVar22 < *(uint *)(param_1 + 0x11f0); uVar22 = uVar22 + 1) {
      uStack_b44 = 2;
      *puVar15 = puVar21[2];
      uVar13 = puVar21[3];
      local_b48 = 0x40;
      uStack_b47 = 0x22;
      puVar15[1] = uVar13;
      local_b3c = (undefined4)uVar13;
      uStack_b46 = 0xfc3f;
      local_b40 = (undefined2)((ulong)uVar13 >> 0x20);
      uStack_b3e = (undefined2)((ulong)uVar13 >> 0x30);
      uStack_b43 = 0;
      uStack_b42 = 0;
      uStack_b41 = 0;
      FUN_0036cf68(&local_b48,0x10);
      iVar9 = *(int *)(puVar21 + 1);
      *(int *)(puVar15 + 2) = iVar11;
      *(int *)((long)puVar15 + 0x14) = iVar9;
      iVar11 = iVar11 + iVar9;
      *(undefined4 *)(puVar15 + 3) = *(undefined4 *)puVar21;
      puVar15 = puVar15 + 4;
      puVar21 = puVar21 + 6;
    }
    if (*(char *)((long)param_2 + 0x86b) == '\0') {
      local_a40[1] = local_a40[1] - *(int *)(*(long *)(param_1 + 0x14a8) + 0x18);
    }
    uVar10 = _ZN7libjpeg13GetExceptListEPN3mpr8libcrypt9cryptAreaEjPhPNS1_21EncryptOrExceptArea_tEPj
                       (param_1,local_a50,*(uint *)(param_1 + 0x11f0),__s,auStack_ae0,&uStack_b80);
    if (*(char *)((long)param_2 + 0x86b) == '\0') {
      _ZN9app11_sig3addEPN3mpr8libcrypt9cryptAreaEjPNS0_9TrustTimeEPN9DefStruct13UniversalTimeEjPv
                (*(undefined8 *)(param_1 + 0x14a8),local_a50,uVar10,(long)param_2 + 0x408,
                 (long)param_2 + 0x428,*(undefined4 *)((long)param_2 + 0x438),
                 *(undefined8 *)((long)param_2 + 0x440));
    }
    else {
      local_b90 = CONCAT44(local_b90._4_4_,uVar10);
      local_b70 = CONCAT44(local_b70._4_4_,0x180e0c);
      local_ae8 = 0;
      local_b48 = 0;
      uStack_b47 = 0;
      uStack_b46 = 0;
      uStack_b44 = 0;
      uStack_b43 = 0;
      uStack_b42 = 0;
      uStack_b41 = 0;
      local_b40 = 0;
      uStack_b3e = 0;
      local_b3c = 0;
      local_b38 = 0;
      local_b30 = 0;
      local_b20 = 0;
      local_b18 = 0;
      uStack_b10 = 0;
      local_b08 = 0;
      uStack_b00 = 0;
      local_af8 = 0;
      uStack_af0 = 0;
      uVar22 = *(uint *)((long)param_2 + 0x438);
      if (uVar22 != 0) {
        local_b18 = (ulong)uVar22;
        local_b20 = *(undefined8 *)((long)param_2 + 0x440);
      }
      local_b28 = (ulong)(uVar22 != 0);
      local_b98 = local_a50;
      local_b88 = auStack_ae0;
      if (*(char *)((long)param_2 + 0x408) != '\0') {
        local_b48 = 1;
        uVar13 = *(undefined8 *)((long)param_2 + 0x418);
        uStack_b46 = (undefined2)uVar13;
        uStack_b44 = (undefined1)((ulong)uVar13 >> 0x10);
        uStack_b43 = (undefined1)((ulong)uVar13 >> 0x18);
        uStack_b42 = (undefined1)((ulong)uVar13 >> 0x20);
        uStack_b41 = (undefined1)((ulong)uVar13 >> 0x28);
        local_b40 = (undefined2)((ulong)uVar13 >> 0x30);
        uStack_b3e = *(undefined2 *)((long)param_2 + 0x420);
        _ZN3mpr8timeconv33ConvLocalTimeToTrustUniversalTimeEPN9DefStruct9LocalTimeElPNS1_13UniversalTimeE
                  ((long)param_2 + 0x3b8,*(undefined8 *)((long)param_2 + 0x410),&local_b3c);
        local_b30 = *(undefined8 *)((long)param_2 + 0x410);
      }
      local_bd0 = local_b78;
      uStack_bc8 = local_b70;
      local_bc0 = local_b68;
      uStack_bb8 = uStack_b60;
      local_bb0 = local_b58;
      uStack_ba8 = uStack_b50;
      uVar10 = _ZN4C2PA10CreateC2paEN8stillcom10BufferAreaEPN5still8common_tEPNS_13EncryptC2PA_tEbPN3mpr8libcrypt10assetParamEjPKhPKc
                         (&local_bd0,auStack_788,&local_b98,1,&local_b48,local_bd4,
                          (long)param_2 + 0x485,(long)param_2 + 0x4df);
      iVar11 = FUN_0035293c((long)param_2 + 0x2f8,&local_b78,uVar10);
      if (iVar11 == 0) {
        return 0xffffffff;
      }
    }
  }
  return 0;
}


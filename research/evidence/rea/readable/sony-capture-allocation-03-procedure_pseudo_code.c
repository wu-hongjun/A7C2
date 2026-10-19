// Decompiled pseudocode, not original source.
// REA Evidence: ev_48444d80fc944c0ddd4317004c083cf4320547f64b625eb6714a727d8ad9bf5a
// Source record: research/evidence/rea/sony-capture-allocation/03-procedure_pseudo_code.json

undefined8
_ZN3mpr21DecideWriteFileFormatERKNS_13stillRecParamENS_11CaptureTypeENS_16WriteInfoPerSlotEPNS_16WriteInfoPerFileEPb
          (uint *param_1,int param_2,int *param_3,uint *param_4,undefined1 *param_5)

{
  long lVar1;
  undefined8 uVar2;
  uint uVar3;
  int iVar4;
  undefined8 uVar5;
  bool bVar6;
  bool bVar7;
  undefined2 uVar8;
  uint uVar9;
  void *pvVar10;
  int *piVar11;
  int iVar12;
  int iVar13;
  ulong uVar14;
  uint *puVar15;
  uint *puVar16;
  uint *puVar17;
  void *__src;
  uint *puVar18;
  int iVar19;
  uint *puVar20;
  void *pvVar21;
  ulong uVar22;
  long lVar23;
  ulong uVar24;
  long lVar25;
  byte bVar26;
  long lVar27;
  long lVar28;
  char *local_60 [2];
  char acStack_50 [16];
  undefined8 local_40;
  undefined8 uStack_38;
  undefined8 local_30;
  undefined8 local_28;
  undefined8 local_20;
  undefined8 uStack_18;
  undefined8 local_10;
  undefined8 local_8;

  *param_4 = 0;
  puVar20 = param_4 + 0x119;
  do {
    puVar16 = puVar20 + -0x10f;
    puVar15 = puVar20 + -0x106;
    puVar20[-0x118] = 0;
    puVar20[-0x117] = 3;
    puVar20[-0x116] = 3;
    puVar20[-0x115] = 4;
    puVar20[-0x114] = 10;
    puVar20[-0x113] = 0xff;
    puVar20[-0x112] = 0xffffffff;
    puVar20[-0x111] = 0;
    puVar17 = puVar20 + -0x107;
    puVar18 = puVar20 + -0x10b;
    do {
      *(undefined2 *)puVar16 = 0;
      *(undefined2 *)(puVar16 + 2) = 0;
      *puVar18 = 0;
      *(undefined1 *)puVar17 = 0;
      puVar16 = (uint *)((long)puVar16 + 2);
      memset(puVar15,0,0x106);
      puVar15 = (uint *)((long)puVar15 + 0x106);
      puVar17 = (uint *)((long)puVar17 + 1);
      puVar18 = puVar18 + 1;
    } while (puVar16 != puVar20 + -0x10d);
    puVar16 = puVar20 + 0x15a;
    memset(puVar20,0,0x106);
    puVar20 = puVar16;
  } while (puVar16 != param_4 + 0x3cd);
  *param_5 = 0;
  uVar9 = param_1[0x12];
  if (1 < uVar9 - 1) {
    uVar8 = 0x4900;
LAB_001e3fdc:
    local_40._0_4_ = CONCAT22(uVar8,0x2120);
    local_40 = (ulong)CONCAT24(4,(int)local_40);
    FUN_001df8d4(&local_40,8);
    return 3;
  }
  uVar3 = *param_1;
  if ((uVar3 < 2) ||
     (((uVar3 == 2 &&
       ((uVar9 == 1 || ((param_1[0x13] == param_1[0x1b] && (param_1[0x19] == param_1[0x21])))))) ||
      (param_2 - 1U < 3)))) {
    puVar20 = (uint *)(param_3 + 1);
    puVar16 = param_4 + 0x13;
    pvVar21 = (void *)((long)param_3 + 0x13d);
    lVar23 = 0;
    do {
      if (puVar20[0x11] != 0) {
        lVar25 = (long)param_4 + lVar23;
        lVar1 = lVar25 + lVar23;
        lVar28 = 0;
        *param_4 = *puVar20;
        __src = pvVar21;
        puVar15 = puVar16;
        puVar17 = puVar20 + -1;
        lVar27 = lVar1;
        while( true ) {
          uVar9 = (uint)lVar28;
          lVar28 = lVar28 + 1;
          if (*param_4 <= uVar9) break;
          puVar18 = param_4 + lVar28 * 0x15a + -0x159;
          uVar2 = *(undefined8 *)(puVar17 + 4);
          *(undefined8 *)puVar18 = *(undefined8 *)(puVar17 + 2);
          *(undefined8 *)(puVar18 + 2) = uVar2;
          uVar2 = *(undefined8 *)(puVar17 + 8);
          *(undefined8 *)(puVar18 + 4) = *(undefined8 *)(puVar17 + 6);
          *(undefined8 *)(puVar18 + 6) = uVar2;
          *(char *)(lVar25 + 0x24) = (char)puVar17[9];
          pvVar10 = memcpy(puVar15,__src,0x105);
          puVar15 = (uint *)((long)pvVar10 + 0x568);
          *(undefined2 *)(lVar27 + 0x28) = *(undefined2 *)((long)pvVar21 + -9);
          __src = (void *)((long)__src + 0x106);
          *(undefined2 *)(lVar27 + 0x30) = *(undefined2 *)((long)pvVar21 + -7);
          lVar27 = lVar27 + 0x568;
          *(undefined4 *)(lVar1 + lVar28 * 0x568 + lVar23 * 2 + -0x530) =
               *(undefined4 *)((long)pvVar21 + -5);
          *(undefined1 *)(lVar25 + 0x48) = *(undefined1 *)((long)pvVar21 + -1);
          lVar25 = lVar25 + 0x568;
          puVar17 = puVar17 + 8;
        }
        if (*(char *)((long)pvVar21 + -1) != '\0') {
          *param_5 = 1;
        }
      }
      lVar23 = lVar23 + 1;
      puVar20 = puVar20 + 0x13;
      puVar16 = (uint *)((long)puVar16 + 0x106);
      pvVar21 = (void *)((long)pvVar21 + 0x218);
    } while (lVar23 != 4);
  }
  else {
    if (uVar3 != 2 || uVar9 != 2) {
      uVar8 = 0x4910;
      goto LAB_001e3fdc;
    }
    iVar13 = *param_3;
    uVar9 = 2;
    if (iVar13 == 1) {
      uVar9 = (param_1[0x13] != param_1[0x1b]) + 1;
    }
    iVar12 = -1;
    *param_4 = uVar9;
    for (uVar9 = 0; uVar9 < *param_4; uVar9 = uVar9 + 1) {
      piVar11 = param_3 + 0x12;
      uVar22 = 0;
      do {
        iVar19 = (int)uVar22;
        if (*piVar11 != 0) {
          if (iVar13 == 1) {
            uVar14 = (ulong)uVar9;
          }
          else {
            if (iVar12 == iVar19) goto LAB_001e4444;
            uVar14 = 0;
          }
          uVar24 = (ulong)uVar9;
          lVar28 = (-(uVar14 >> 0x1f) & 0xffffffe000000000 | uVar14 << 5) + uVar22 * 0x4c;
          lVar23 = uVar22 + uVar24 * 0x568;
          uVar2 = *(undefined8 *)((long)param_3 + lVar28 + 0x10);
          *(undefined8 *)(param_4 + uVar24 * 0x15a + 1) =
               *(undefined8 *)((long)param_3 + lVar28 + 8);
          *(undefined8 *)(param_4 + uVar24 * 0x15a + 3) = uVar2;
          uVar2 = *(undefined8 *)((long)param_3 + lVar28 + 0x20);
          *(undefined8 *)(param_4 + uVar24 * 0x15a + 5) =
               *(undefined8 *)((long)param_3 + lVar28 + 0x18);
          *(undefined8 *)(param_4 + uVar24 * 0x15a + 7) = uVar2;
          *(undefined1 *)((long)param_4 + lVar23 + 0x24) =
               *(undefined1 *)((long)param_3 + lVar28 + 0x24);
          memcpy((void *)((long)param_4 + (ulong)uVar9 * 0x568 + 0x4c + uVar22 * 0x106),
                 (void *)((long)param_3 + uVar22 * 0x218 + (long)(int)uVar14 * 0x106 + 0x13d),0x105)
          ;
          lVar28 = uVar22 + (ulong)uVar9 * 0x2b4;
          *(short *)((long)param_4 + lVar28 * 2 + 0x28) = (short)param_3[uVar22 * 0x86 + 0x4d];
          *(undefined2 *)((long)param_4 + lVar28 * 2 + 0x30) =
               *(undefined2 *)((long)param_3 + uVar22 * 0x218 + 0x136);
          iVar4 = param_3[uVar22 * 0x86 + 0x4f];
          param_4[uVar22 + uVar24 * 0x15a + 0xe] = param_3[uVar22 * 0x86 + 0x4e];
          *(char *)((long)param_4 + lVar23 + 0x48) = (char)iVar4;
          iVar12 = iVar19;
          if ((char)iVar4 != '\0') {
            *param_5 = 1;
          }
          break;
        }
LAB_001e4444:
        uVar22 = (ulong)(iVar19 + 1U);
        piVar11 = piVar11 + 0x13;
      } while (iVar19 + 1U != 4);
    }
  }
  uVar9 = param_1[0x23];
  if (uVar9 == 0) {
    return 0;
  }
  if (2 < uVar9) {
    return 3;
  }
  local_20 = 0;
  uStack_18 = 0;
  local_10 = 0;
  local_8 = 0;
  if (param_2 == 3) {
    local_40 = 0x300000001;
    uStack_38 = 0x300000002;
    local_30 = 0xff00000000;
    local_28 = 0;
  }
  else {
    local_40 = *(undefined8 *)(param_1 + 0x24);
    uVar2 = local_40;
    uStack_38 = *(undefined8 *)(param_1 + 0x26);
    local_30 = *(undefined8 *)(param_1 + 0x28);
    local_28 = *(undefined8 *)(param_1 + 0x2a);
    local_20 = *(undefined8 *)(param_1 + 0x2c);
    uVar5 = local_20;
    uStack_18 = *(undefined8 *)(param_1 + 0x2e);
    local_10 = *(undefined8 *)(param_1 + 0x30);
    local_8 = *(undefined8 *)(param_1 + 0x32);
    if (uVar9 == 2) {
      local_40._0_4_ = (int)*(undefined8 *)(param_1 + 0x24);
      bVar7 = false;
      local_20._0_4_ = (int)*(undefined8 *)(param_1 + 0x2c);
      bVar6 = (int)local_40 == (int)local_20;
      iVar13 = 2;
      local_40 = uVar2;
      local_20 = uVar5;
      if (bVar6) {
        local_28._0_4_ = (uint)*(undefined8 *)(param_1 + 0x2a);
        local_8._0_4_ = (uint)*(undefined8 *)(param_1 + 0x32);
        bVar7 = (uint)local_28 != (uint)local_8;
      }
      goto LAB_001e4318;
    }
  }
  iVar13 = 1;
  bVar7 = false;
LAB_001e4318:
  puVar20 = (uint *)&local_28;
  bVar26 = 0;
  do {
    for (uVar9 = 0; uVar9 < *param_4; uVar9 = uVar9 + 1) {
      uVar3 = param_4[(ulong)uVar9 * 0x15a + 1];
      if ((uVar3 == puVar20[-6]) && (param_4[(ulong)uVar9 * 0x15a + 7] == *puVar20)) {
        puVar15 = param_4 + (ulong)uVar9 * 0x15a + 0x119;
        puVar16 = param_4 + (ulong)uVar9 * 0x15a + 0x13;
        do {
          if ((char)*puVar16 != '\0') {
            if ((uVar3 == 2) && (bVar26 = *(byte *)((long)param_1 + 0xd9), bVar26 != 0)) {
              local_60[0] = acStack_50;
              FUN_001df468(local_60,puVar16,(long)puVar16 + 0x105);
              lVar23 = _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4findEPKcm
                                 (local_60,&DAT_003ce44f,0);
              if (lVar23 != -1) {
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE5eraseEmm(local_60,lVar23,3);
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6insertEmPKc
                          (local_60,lVar23,&DAT_003cc797);
                strncpy((char *)puVar15,local_60[0],0x105);
              }
              _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_60);
            }
            else {
              bVar26 = 1;
              strncpy((char *)puVar15,(char *)puVar16,0x105);
            }
          }
          puVar16 = (uint *)((long)puVar16 + 0x106);
        } while (puVar16 != puVar15);
      }
    }
    if ((bVar7 & bVar26) != 0) goto LAB_001e44f4;
    puVar20 = puVar20 + 8;
  } while ((uint *)(&local_8 + (ulong)(iVar13 - 1) * 4) != puVar20);
  if (bVar26 == 0) {
    return 3;
  }
LAB_001e44f4:
  *param_5 = 0;
  return 0;
}

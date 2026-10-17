// Decompiled pseudocode, not original source.
// REA Evidence: ev_09d151700c7ab0a49f2e96d26d88395f628c50a253a999129de9771c7c8890a9
// Source record: research/evidence/rea/libmpr-still-sink/00-procedure_pseudo_code.json

void _ZN3mpr11stillrecord14_make_add_if_tEPKNS_18recordStillInParamENS_9StillTypeEPN9DefStruct7JpgInfoEPNS5_7RawInfoEPNS5_7HifInfoEPNS_18recordStillParam_tEPhbbPN5still8add_if_tEPNS0_13sidb_update_tEP10dmm_munmapPc
               (long param_1,int param_2,undefined4 *param_3,undefined4 *param_4,undefined4 *param_5
               ,long *param_6,undefined8 *param_7,char param_8,char param_9,long *param_10,
               undefined2 *param_11,undefined8 param_12,long *param_13)

{
  bool bVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  int iVar4;
  undefined4 uVar5;
  char cVar6;
  undefined1 uVar7;
  long lVar8;
  byte bVar9;
  undefined1 uVar10;
  int iVar11;
  uint uVar12;
  long lVar13;
  long lVar14;
  ulong uVar15;
  size_t sVar16;
  char *pcVar17;
  long lVar18;
  uint uVar19;
  undefined4 uVar20;
  long lVar21;
  long *plVar22;
  undefined4 *puVar23;
  int iVar24;
  long lVar25;
  undefined1 *puVar26;
  undefined4 *puVar27;
  undefined2 local_158;
  undefined2 local_156;
  undefined1 local_154;
  undefined1 local_153;
  undefined1 local_152;
  undefined1 local_151;
  undefined1 auStack_150 [12];
  undefined4 local_144;
  undefined4 local_140;
  long local_138;
  undefined4 local_130;
  undefined4 *local_b8;
  long local_a8;
  undefined8 local_a0;
  byte local_98;
  undefined1 uStack_97;
  undefined1 uStack_96;
  undefined1 uStack_95;
  undefined2 uStack_94;
  undefined2 uStack_92;
  undefined2 uStack_90;
  undefined6 uStack_8e;
  long local_10;
  
  local_a8 = 0x410002120;
  FUN_001df8d4(&local_a8,8);
  memset(auStack_150,0,0xa8);
  if (*(int *)((long)param_6 + 0xc) == 2) {
    FUN_003b2224(0x173762,auStack_150,1,param_12);
    puVar27 = local_b8;
    if (param_2 == 1) {
      *param_3 = local_144;
      param_3[1] = 0;
      param_3[2] = local_130;
      param_3[3] = local_140;
      puVar23 = param_3;
LAB_001e5354:
      pcVar17 = *(char **)(param_1 + 0x80);
      uVar20 = 0xffffffff;
      lVar13 = *(long *)(param_1 + 0x80);
      cVar6 = pcVar17[0x4c];
      *local_b8 = 0xffffffff;
      if (cVar6 == '\0') {
        local_b8[1] = 0xffffffff;
      }
      else {
        local_b8[1] = *(undefined4 *)(pcVar17 + 0x50);
        uVar20 = *(undefined4 *)(pcVar17 + 0x54);
      }
      local_b8[2] = uVar20;
      if (pcVar17[0x42] == '\0') {
        uVar20 = 0xffffffff;
        local_b8[3] = 0xffffffff;
      }
      else {
        local_b8[3] = *(undefined4 *)(pcVar17 + 0x44);
        uVar20 = *(undefined4 *)(pcVar17 + 0x48);
      }
      local_b8[4] = uVar20;
      if (pcVar17[0x58] == '\0') {
        uVar19 = 0xffffffff;
      }
      else {
        uVar19 = (uint)*(ushort *)(pcVar17 + 0x5a);
      }
      cVar6 = pcVar17[0xac];
      local_b8[5] = uVar19;
      uVar20 = 0xffffffff;
      if (cVar6 == '\0') {
        local_b8[6] = 0xffffffff;
      }
      else {
        local_b8[6] = (uint)*(ushort *)(pcVar17 + 0xae);
      }
      cVar6 = pcVar17[0xb4];
      local_b8[7] = 0xffffffff;
      local_b8[8] = 0xffffffff;
      local_b8[9] = 0xffffffff;
      if (cVar6 != '\0') {
        uVar20 = *(undefined4 *)(pcVar17 + 0xb8);
      }
      cVar6 = pcVar17[0x5c];
      local_b8[10] = uVar20;
      uVar20 = 0xffffffff;
      local_b8[0xb] = 0xffffffff;
      local_b8[0xc] = 0xffffffff;
      if (cVar6 == '\0') {
        local_b8[0xd] = 0xffffffff;
      }
      else {
        local_b8[0xd] = *(undefined4 *)(pcVar17 + 0x60);
        uVar20 = *(undefined4 *)(pcVar17 + 100);
      }
      local_b8[0xe] = uVar20;
      if (pcVar17[0x68] == '\0') {
        uVar20 = 0xffffffff;
        local_b8[0xf] = 0xffffffff;
      }
      else {
        local_b8[0xf] = *(undefined4 *)(pcVar17 + 0x6c);
        uVar20 = *(undefined4 *)(pcVar17 + 0x70);
      }
      local_b8[0x10] = uVar20;
      if (pcVar17[0xb0] == '\0') {
        uVar19 = 0xffffffff;
      }
      else {
        uVar19 = (uint)*(ushort *)(pcVar17 + 0xb2);
      }
      local_b8[0x11] = uVar19;
      if (pcVar17[0xa8] == '\0') {
        uVar19 = 0xffffffff;
      }
      else {
        uVar19 = (uint)*(ushort *)(pcVar17 + 0xaa);
      }
      local_b8[0x12] = uVar19;
      if (pcVar17[0x80] == '\0') {
        uVar19 = 0xffffffff;
      }
      else {
        uVar19 = (uint)*(ushort *)(pcVar17 + 0x82);
      }
      local_b8[0x13] = uVar19;
      if (pcVar17[0x74] == '\0') {
        uVar20 = 0xffffffff;
        local_b8[0x14] = 0xffffffff;
      }
      else {
        local_b8[0x14] = *(undefined4 *)(pcVar17 + 0x78);
        uVar20 = *(undefined4 *)(pcVar17 + 0x7c);
      }
      local_b8[0x15] = uVar20;
      if (pcVar17[0x14] == '\0') {
        uVar20 = 0xffffffff;
      }
      else {
        uVar20 = 0xffff;
        if (*(int *)(pcVar17 + 0x18) == 0) {
          uVar20 = 1;
        }
      }
      local_b8[0x16] = uVar20;
      if (*pcVar17 == '\0') {
        uVar20 = 0xffffffff;
        local_b8[0x17] = 0xffffffff;
      }
      else {
        local_b8[0x17] = *(undefined4 *)(pcVar17 + 4);
        uVar20 = *(undefined4 *)(pcVar17 + 8);
      }
      cVar6 = pcVar17[0x84];
      local_b8[0x18] = uVar20;
      uVar19 = 0xffffffff;
      local_b8[0x19] = 0xffffffff;
      local_b8[0x1a] = 0xffffffff;
      local_b8[0x1b] = 0xffffffff;
      local_b8[0x1c] = 0xffffffff;
      if (cVar6 == '\0') {
        local_b8[0x1d] = 0xffffffff;
      }
      else {
        local_b8[0x1d] = (uint)*(ushort *)(pcVar17 + 0x86);
      }
      cVar6 = pcVar17[0x88];
      local_b8[0x1e] = 0xffffffff;
      if (cVar6 != '\0') {
        uVar19 = (uint)*(ushort *)(pcVar17 + 0x8a);
      }
      local_b8[0x1f] = uVar19;
      if (pcVar17[0x9c] == '\0') {
        uVar20 = 0xffffffff;
        local_b8[0x20] = 0xffffffff;
      }
      else {
        local_b8[0x20] = *(undefined4 *)(pcVar17 + 0xa0);
        uVar20 = *(undefined4 *)(pcVar17 + 0xa4);
      }
      cVar6 = pcVar17[0x8c];
      local_b8[0x21] = uVar20;
      uVar19 = 0xffffffff;
      local_b8[0x22] = 0xffffffff;
      if (cVar6 != '\0') {
        uVar19 = (uint)*(ushort *)(pcVar17 + 0x8e);
      }
      local_b8[0x23] = uVar19;
      if (pcVar17[0x263] == '\0') {
        uVar20 = 0xffffffff;
      }
      else {
        uVar20 = *(undefined4 *)(pcVar17 + 0x264);
      }
      local_b8[0x24] = uVar20;
      if (pcVar17[0x268] == '\0') {
        uVar20 = 0xffffffff;
      }
      else {
        uVar20 = *(undefined4 *)(pcVar17 + 0x26c);
      }
      cVar6 = pcVar17[0x270];
      local_b8[0x25] = uVar20;
      if (cVar6 == '\0') {
        local_b8[0x26] = 0xffffffff;
      }
      else {
        local_b8[0x26] = *(undefined4 *)(pcVar17 + 0x274);
      }
      local_b8[0x27] = 0xffffffff;
      local_b8[0x28] = 0xffffffff;
      local_b8[0x29] = 0xffffffff;
      local_b8[0x2a] = 0xffffffff;
      *local_b8 = 1;
      local_b8[0x2b] = 0xffffffff;
      local_b8[0x2c] = 0xffffffff;
      local_b8[0x2d] = 0xffffffff;
      local_b8[0x2e] = 0xffffffff;
      *(undefined8 *)(puVar23 + 0xc) = 0;
      *(undefined8 *)(puVar23 + 0xe) = 0;
      *(undefined8 *)(puVar23 + 0x10) = 0;
      *(undefined8 *)(puVar23 + 0x12) = 0;
      *(undefined8 *)(puVar23 + 0x14) = 0;
      *(undefined8 *)(puVar23 + 0x16) = 0;
      *(undefined8 *)(puVar23 + 0x18) = 0;
      *(undefined8 *)(puVar23 + 0x1a) = 0;
      if (*(int *)(lVar13 + 0x18) == 2) {
        uVar20 = 2;
        if ((param_2 == 4) || (uVar20 = 0, param_2 == 1)) {
LAB_001e5674:
          local_a0 = local_a0 & 0xffffffffffff0000;
          local_a8 = 0;
          if (*(char *)(lVar13 + 0x34) != '\0') {
            _ZN3mpr8timeconv33ConvLocalTimeToTrustUniversalTimeEPN9DefStruct9LocalTimeElPNS1_13UniversalTimeE
                      (lVar13 + 0x36,param_6[0x21],&local_a8);
          }
          lVar14 = *(long *)(param_1 + 0x80);
          lVar13 = param_6[0x1d];
          lVar18 = param_6[0x1c];
          lVar21 = *param_6;
          lVar8 = param_6[0x25];
          lVar25 = param_6[0x24];
          param_10[0x77] = *param_6;
          *(int *)(param_10 + 0x78) = (int)param_6[1];
          uVar5 = *(undefined4 *)(lVar14 + 0x3e);
          *(undefined8 *)((long)param_10 + 0x3c4) = *(undefined8 *)(lVar14 + 0x36);
          *(undefined4 *)((long)param_10 + 0x3cc) = uVar5;
          *(int *)(param_10 + 0x7f) = (int)lVar13;
          param_10[0x7e] = (long)param_6 + 0x92;
          param_10[0x80] = lVar18;
          lVar13 = param_6[0x20];
          lVar14 = param_6[0x21];
          *(undefined1 *)(param_10 + 0x7a) = 0;
          *(undefined1 *)(param_10 + 0x7c) = 0;
          param_10[0x7b] = 0;
          param_10[0x7d] = 0;
          param_10[0x81] = lVar13;
          param_10[0x82] = lVar14;
          lVar13 = param_6[0x23];
          param_10[0x83] = param_6[0x22];
          param_10[0x84] = lVar13;
          *(short *)((long)param_10 + 0x432) = (short)lVar21;
          param_10[0x85] = local_a8;
          *(int *)(param_10 + 0x87) = (int)lVar8;
          *(undefined2 *)(param_10 + 0x86) = (undefined2)local_a0;
          param_10[0x88] = lVar25;
          *(undefined4 *)(param_10 + 0x89) = uVar20;
          *(undefined1 *)((long)param_10 + 0x44c) = 0;
          uVar19 = puVar27[10];
          puVar27[9] = uVar19;
          if (0xffff < uVar19) {
            uVar19 = 0xffff;
          }
          bVar9 = *(byte *)(param_1 + 0x88);
          puVar27[6] = uVar19;
          puVar27[10] = 0xffffffff;
          puVar27[7] = 2;
          bVar9 = *(byte *)((long)param_6 + 0xdd) & bVar9;
          goto LAB_001e5770;
        }
        uVar19 = 0x532;
      }
      else {
        uVar20 = 0;
        if (param_2 == 4 || param_2 == 1) goto LAB_001e5674;
        uVar19 = 0x53c;
      }
    }
    else {
      if (param_2 == 4) {
        *param_5 = local_144;
        param_5[1] = 0;
        param_5[2] = local_130;
        param_5[3] = local_140;
        puVar23 = param_5;
        goto LAB_001e5354;
      }
      uVar19 = 0x51c;
    }
  }
  else {
    local_a0 = local_a0 & 0xffffffffffff0000;
    local_a8 = 0;
    _ZN3mpr8timeconv33ConvLocalTimeToTrustUniversalTimeEPN9DefStruct9LocalTimeElPNS1_13UniversalTimeE
              (param_6,param_6[0x21],&local_a8);
    lVar13 = param_6[2];
    uVar10 = *(undefined1 *)((long)param_6 + 0x51);
    uVar7 = *(undefined1 *)((long)param_6 + 0xde);
    uVar20 = *(undefined4 *)((long)param_6 + 0xd4);
    lVar14 = param_6[0x1d];
    lVar21 = param_6[0x25];
    lVar18 = param_6[0x1c];
    lVar25 = param_6[0x24];
    lVar8 = *param_6;
    param_10[0x77] = *param_6;
    *(int *)(param_10 + 0x78) = (int)param_6[1];
    *(long *)((long)param_10 + 0x3c4) = *param_6;
    *(int *)((long)param_10 + 0x3cc) = (int)param_6[1];
    param_10[0x80] = lVar18;
    *(undefined1 *)(param_10 + 0x7c) = uVar10;
    param_10[0x7d] = (long)param_6 + 0x52;
    *(char *)(param_10 + 0x7a) = (char)lVar13;
    *(int *)(param_10 + 0x7f) = (int)lVar14;
    param_10[0x7e] = (long)param_6 + 0x92;
    lVar13 = param_6[0x20];
    lVar14 = param_6[0x21];
    param_10[0x7b] = (long)param_6 + 0x11;
    param_10[0x81] = lVar13;
    param_10[0x82] = lVar14;
    lVar13 = param_6[0x23];
    param_10[0x83] = param_6[0x22];
    param_10[0x84] = lVar13;
    param_10[0x85] = local_a8;
    *(undefined2 *)(param_10 + 0x86) = (undefined2)local_a0;
    *(short *)((long)param_10 + 0x432) = (short)lVar8;
    *(undefined1 *)((long)param_10 + 0x44c) = uVar7;
    *(int *)(param_10 + 0x87) = (int)lVar21;
    param_10[0x88] = lVar25;
    *(undefined4 *)(param_10 + 0x89) = uVar20;
    if (param_2 == 2) {
      *(undefined4 *)(param_10 + 0x7f) = 0;
      param_10[0x80] = 0;
    }
    bVar9 = *(byte *)((long)param_6 + 0xdd);
LAB_001e5770:
    *(byte *)((long)param_10 + 0x86b) = bVar9;
    uVar10 = *(undefined1 *)((long)param_6 + 0xdc);
    lVar13 = param_6[0x1b];
    param_10[0x8a] = *(long *)(param_1 + 0x38);
    uVar10 = _ZN3mpr16IsSignatureValidENS_9StillTypeES0_b(param_2,(int)lVar13,uVar10);
    *(undefined1 *)(param_10 + 0x10d) = uVar10;
    lVar13 = param_13[1];
    param_10[0x8b] = *param_13;
    param_10[0x8c] = lVar13;
    lVar13 = param_13[3];
    param_10[0x8d] = param_13[2];
    param_10[0x8e] = lVar13;
    param_10[0x8f] = param_13[4];
    *(undefined8 *)((long)param_10 + 0x47d) = *(undefined8 *)((long)param_13 + 0x25);
    _ZN3mpr10GetXmpUUIDEPci((long)param_10 + 0x485,0x2d);
    _ZN3mpr10GetXmpUUIDEPci((long)param_10 + 0x4b2,0x2d);
    puVar27 = param_3;
    if (((param_2 == 1) || (puVar27 = param_4, param_2 == 2)) || (puVar27 = param_5, param_2 == 4))
    {
      lVar13 = FUN_003b21e8(*puVar27,auStack_150,1,param_12);
      if (lVar13 == 0) {
        uVar19 = 0x576;
        pcVar17 = 
        "app_dmm::get_addr( p_exif->exifSrcArea.memHandle, &tmp_unit, true, p_dmm_munmap )";
        goto LAB_001e55b4;
      }
      *param_10 = (long)local_b8 + (ulong)(uint)puVar27[1];
      lVar13 = _ZN3mpr13gp_paramBoardE + 0x28;
      param_10[1] = _ZN3mpr13gp_paramBoardE + 8;
      param_10[2] = lVar13;
      uVar19 = puVar27[0xd];
      *(undefined4 *)(param_10 + 3) = puVar27[0xe];
      param_10[4] = (long)local_b8 + (ulong)uVar19;
      uVar19 = puVar27[0x11];
      *(undefined4 *)(param_10 + 6) = puVar27[0x12];
      param_10[7] = (long)local_b8 + (ulong)uVar19;
      uVar19 = puVar27[0x15];
      *(undefined4 *)(param_10 + 8) = puVar27[0x16];
      param_10[9] = (long)local_b8 + (ulong)uVar19;
      uVar19 = puVar27[0x19];
      *(undefined4 *)(param_10 + 10) = puVar27[0x1a];
      param_10[0xb] = (long)local_b8 + (ulong)uVar19;
      if (*(int *)((long)param_6 + 0xc) == 2) {
        memset(&local_a8,0,0xa8);
        if (puVar27[0x20] != 0) {
          lVar13 = FUN_003b21c8(puVar27[0x20],&local_a8,1,param_12);
          if (lVar13 == 0) {
            uVar19 = 0x585;
            pcVar17 = 
            "app_dmm::open( p_exif->extendedMakerNote.memHandle, &tempextendedMakerNote, true, p_dmm_munmap )"
            ;
            goto LAB_001e55b4;
          }
          uVar19 = puVar27[0x21];
          uVar20 = puVar27[0x22];
          param_10[0xe] = local_10 + (ulong)uVar19;
          param_10[0xf] = (ulong)uVar19 + CONCAT62(uStack_8e,uStack_90);
          *(undefined4 *)(param_10 + 0xd) = uVar20;
        }
      }
      else {
        uVar19 = puVar27[0x21];
        *(undefined4 *)(param_10 + 0xd) = puVar27[0x22];
        param_10[0xe] = (long)local_b8 + (ulong)uVar19;
        param_10[0xf] = (ulong)uVar19 + local_138;
      }
      if (param_2 == 1) {
        if (param_3[0x26] != 0) {
          FUN_001e03a0(param_3 + 0x24,param_10 + 0x29,1,param_12);
        }
        if (param_3[0x2e] != 0) {
          FUN_001e03a0(param_3 + 0x2c,param_10 + 0x35,1,param_12);
        }
        if (param_3[0x2a] != 0) {
          FUN_001e03a0(param_3 + 0x28,param_10 + 0x2f,1,param_12);
        }
        if (param_11 != (undefined2 *)0x0) {
          param_11[2] = (short)param_3[0x2a];
        }
        if (*(int *)((long)param_6 + 0xc) == 2) {
          local_a8 = param_10[0x2b];
          local_a0._0_4_ = 0x800;
          iVar11 = FUN_003713b0(&local_a8);
          lVar13 = *param_10;
          if (iVar11 == 0) {
            *(undefined4 *)(lVar13 + 0x5c) = local_a0._4_4_;
            uVar20 = CONCAT13(uStack_95,CONCAT12(uStack_96,CONCAT11(uStack_97,local_98)));
          }
          else {
            uVar20 = 0xffffffff;
            *(undefined4 *)(lVar13 + 0x5c) = 0xffffffff;
          }
          *(undefined4 *)(lVar13 + 0x60) = uVar20;
          local_a8 = param_10[0x37];
          local_a0._0_4_ = 0x800;
          iVar11 = FUN_003713b0(&local_a8);
          lVar13 = *param_10;
          if (iVar11 == 0) {
            *(undefined4 *)(lVar13 + 100) = local_a0._4_4_;
            uVar20 = CONCAT13(uStack_95,CONCAT12(uStack_96,CONCAT11(uStack_97,local_98)));
          }
          else {
            uVar20 = 0xffffffff;
            *(undefined4 *)(lVar13 + 100) = 0xffffffff;
          }
          *(undefined4 *)(lVar13 + 0x68) = uVar20;
        }
      }
      else if (param_2 == 2) {
        lVar13 = FUN_003b21e8(param_4[0x28],auStack_150,1,param_12);
        if (lVar13 == 0) {
          uVar19 = 0x5bc;
          pcVar17 = 
          "app_dmm::get_addr( p_dev->developInfoSrcData.memHandle, &tmp_unit, true, p_dmm_munmap )";
          goto LAB_001e55b4;
        }
        uVar20 = param_4[0x2e];
        param_10[0x10] = (long)local_b8 + (ulong)(uint)param_4[0x29];
        param_10[0x17] = (long)local_b8 + (ulong)(uint)param_4[0x39];
        param_10[0x1a] = (long)local_b8 + (ulong)(uint)param_4[0x41];
        uVar19 = param_4[0x2d];
        *(undefined4 *)(param_10 + 0x11) = uVar20;
        param_10[0x12] = (long)local_b8 + (ulong)uVar19;
        uVar19 = param_4[0x31];
        *(undefined4 *)(param_10 + 0x13) = param_4[0x32];
        param_10[0x14] = (long)local_b8 + (ulong)uVar19;
        uVar19 = param_4[0x35];
        *(undefined4 *)(param_10 + 0x15) = param_4[0x36];
        param_10[0x16] = (long)local_b8 + (ulong)uVar19;
        uVar19 = param_4[0x3d];
        *(undefined4 *)(param_10 + 0x18) = param_4[0x3e];
        param_10[0x19] = (long)local_b8 + (ulong)uVar19;
        uVar20 = param_4[0x52];
        param_10[0x1c] = (long)local_b8 + (ulong)(uint)param_4[0x51];
        iVar11 = param_4[0x4e];
        *(undefined4 *)(param_10 + 0x1b) = uVar20;
        if (iVar11 != 0) {
          FUN_001e03a0(param_4 + 0x4c,param_10 + 0x29,1,param_12);
        }
        if (param_4[0x4a] != 0) {
          FUN_001e03a0(param_4 + 0x48,param_10 + 0x3b,1,param_12);
        }
        if (param_4[0x46] != 0) {
          FUN_001e03a0(param_4 + 0x44,param_10 + 0x35,1,param_12);
        }
        if (param_4[0x26] != 0) {
          FUN_001e03a0(param_4 + 0x24,param_10 + 0x2f,1,param_12);
        }
        if (param_11 != (undefined2 *)0x0) {
          param_11[2] = (short)param_4[0x26];
        }
      }
      else {
        if (param_2 != 4) {
          uVar19 = 0x5f6;
          goto LAB_001e55a4;
        }
        if (param_5[0x26] != 0) {
          FUN_001e03a0(param_5 + 0x24,param_10 + 0x29,1,param_12);
        }
        if (param_5[0x2a] != 0) {
          FUN_001e03a0(param_5 + 0x28,param_10 + 0x41,1,param_12);
        }
        if (param_5[0x2e] != 0) {
          FUN_001e03a0(param_5 + 0x2c,param_10 + 0x35,1,param_12);
        }
        if (param_5[0x32] != 0) {
          FUN_001e03a0(param_5 + 0x30,param_10 + 0x2f,1,param_12);
        }
        if (param_11 != (undefined2 *)0x0) {
          param_11[2] = (short)param_5[0x32];
        }
        if (*(int *)((long)param_6 + 0xc) == 2) {
          lVar14 = param_10[0x2b];
          lVar13 = *param_10;
          if (lVar14 == 0) {
            *(undefined4 *)(lVar13 + 0x5c) = 0xffffffff;
            *(undefined4 *)(lVar13 + 0x60) = 0xffffffff;
          }
          else {
            uVar20 = *(undefined4 *)(lVar14 + 8);
            *(undefined4 *)(lVar13 + 0x5c) = *(undefined4 *)(lVar14 + 4);
            *(undefined4 *)(lVar13 + 0x60) = uVar20;
          }
          lVar14 = param_10[0x37];
          if (lVar14 == 0) {
            *(undefined4 *)(lVar13 + 100) = 0xffffffff;
            *(undefined4 *)(lVar13 + 0x68) = 0xffffffff;
          }
          else {
            uVar20 = *(undefined4 *)(lVar14 + 8);
            *(undefined4 *)(lVar13 + 100) = *(undefined4 *)(lVar14 + 4);
            *(undefined4 *)(lVar13 + 0x68) = uVar20;
          }
        }
      }
      if (*(char *)((long)param_10 + 0x86b) != '\0') {
        param_10[0x6b] = param_10[0x35];
        param_10[0x6c] = param_10[0x36];
        param_10[0x6d] = param_10[0x37];
        param_10[0x6e] = param_10[0x38];
        param_10[0x6f] = param_10[0x39];
        param_10[0x70] = param_10[0x3a];
      }
      if (param_11 != (undefined2 *)0x0) {
        lVar13 = *param_10;
        iVar11 = *(int *)(lVar13 + 0x5c);
        if (iVar11 == -1) {
          iVar11 = 0;
        }
        *param_11 = (short)iVar11;
        iVar11 = *(int *)(lVar13 + 0x60);
        if (iVar11 == -1) {
          iVar11 = 0;
        }
        param_11[1] = (short)iVar11;
      }
      lVar13 = 0;
      if (*(int *)((long)param_6 + 0xc) == 2) {
        lVar13 = 2;
        *(undefined1 *)((long)param_10 + 0x624) = 0;
        *(undefined2 *)((long)param_10 + 0x5ec) = 0x102;
        *(undefined1 *)(param_10 + 0xbf) = 3;
        *(undefined4 *)(param_10 + 0xbe) = 4;
        *(undefined2 *)(param_10 + 0xc3) = 0x202e;
        *(undefined1 *)((long)param_10 + 0x625) = 0;
        *(undefined1 *)((long)param_10 + 0x627) = 0;
        *(undefined4 *)((long)param_10 + 0x61c) = 3;
        *(undefined4 *)(param_10 + 0xc4) = 2;
        *(undefined1 *)((long)param_10 + 0x626) = 1;
        *(undefined4 *)((long)param_10 + 0x5f4) = 1;
      }
      *(undefined2 *)((long)param_10 + lVar13 * 0x2c + 0x5ec) = 0x203a;
      *(undefined4 *)((long)param_10 + lVar13 * 0x2c + 0x5f0) = 1;
      *(undefined4 *)((long)param_10 + lVar13 * 0x2c + 0x5f4) = 0x20;
      puVar2 = (undefined8 *)((long)param_10 + lVar13 * 0x2c + 0x5f8);
      uVar3 = param_7[1];
      *puVar2 = *param_7;
      puVar2[1] = uVar3;
      uVar3 = param_7[3];
      iVar11 = (int)lVar13;
      uVar19 = iVar11 + 1;
      puVar2[2] = param_7[2];
      puVar2[3] = uVar3;
      uVar15 = 0;
      plVar22 = param_6;
      do {
        lVar13 = 0;
        do {
          if (*(char *)((long)plVar22 + lVar13 + 0x16c) != '\0') goto LAB_001e5bf0;
          lVar13 = lVar13 + 1;
        } while (lVar13 != 0x10);
        uVar12 = (int)uVar15 + 1;
        uVar15 = (ulong)uVar12;
        plVar22 = plVar22 + 2;
      } while (uVar12 != 4);
      uVar15 = 0xffffffff;
LAB_001e5bf0:
      local_98 = 0;
      uStack_97 = 0;
      uStack_96 = 0;
      uStack_95 = 0;
      uStack_94 = 0;
      local_a8 = 0;
      local_a0 = 0;
      if ((int)uVar15 != -1) {
        local_a8 = *(long *)((long)param_6 + uVar15 * 0x10 + 0x16c);
        local_a0 = *(long *)((long)param_6 + uVar15 * 0x10 + 0x174);
        iVar4 = *(int *)((long)param_6 + 300);
        if ((iVar4 == 1) || (iVar4 == 2)) {
          local_98 = (byte)iVar4;
        }
        uStack_96 = (undefined1)*(undefined4 *)((long)param_6 + uVar15 * 4 + 0x14c);
      }
      uStack_90 = 0;
      uStack_92 = 0;
      uVar12 = iVar11 + 3;
      lVar13 = 0;
      *(undefined2 *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5ec) = 0x203c;
      iVar24 = 7;
      *(undefined4 *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5f0) = 7;
      plVar22 = (long *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5f8);
      *(undefined4 *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5f4) = 0x1a;
      *plVar22 = local_a8;
      plVar22[1] = local_a0;
      plVar22[2] = (ulong)CONCAT12(uStack_96,(ushort)local_98);
      *(undefined2 *)(plVar22 + 3) = 0;
      uVar19 = iVar11 + 2;
      iVar4 = *(int *)((long)param_6 + 0xc);
      *(undefined2 *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5ec) = 0x2041;
      *(bool *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5f8) = iVar4 == 3;
      *(undefined4 *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5f0) = 1;
      *(undefined4 *)((long)param_10 + (ulong)uVar19 * 0x2c + 0x5f4) = 1;
      if (iVar4 == 3 || iVar4 == 0) {
        if (*(char *)(param_1 + 0xb8) != '\0') {
          *(undefined2 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5ec) = 0x2042;
          *(undefined4 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5f0) = 1;
          *(undefined4 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5f4) = 8;
          uVar15 = *(ulong *)(param_1 + 0xc0);
          puVar26 = (undefined1 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5f8);
          do {
            uVar19 = iVar24 << 3;
            iVar24 = iVar24 + -1;
            *puVar26 = (char)(uVar15 >> ((ulong)uVar19 & 0x3f));
            puVar26 = puVar26 + 1;
          } while (iVar24 != -1);
          uVar12 = iVar11 + 4;
        }
        bVar9 = *(byte *)(param_1 + 200);
        lVar13 = 0;
        if ((bVar9 - 1 >> 7 & 1) == 0) {
          *(uint *)(param_10 + 0xeb) = (uint)bVar9;
          *(undefined2 *)(param_10 + 0xea) = 0x2043;
          lVar13 = 1;
          *(undefined4 *)((long)param_10 + 0x754) = 7;
          memcpy((void *)((long)param_10 + 0x75c),(void *)(param_1 + 0xc9),(ulong)bVar9);
        }
      }
      iVar11 = (int)lVar13;
      iVar4 = *(int *)((long)param_6 + 0xc);
      if (iVar4 == 0 || iVar4 == 3) {
        lVar14 = 0;
        *(undefined2 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5ec) = 0x2049;
        *(undefined4 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5f0) = 5;
        *(undefined4 *)((long)param_10 + (ulong)uVar12 * 0x2c + 0x5f4) = 2;
        lVar21 = 0;
        if ((iVar4 == 0) && (*(char *)((long)param_6 + 0xec) != '\0')) {
          lVar14 = param_6[0x1e];
          lVar21 = param_6[0x1f];
        }
        uVar15 = (ulong)uVar12;
        uVar12 = uVar12 + 1;
        *(long *)((long)param_10 + uVar15 * 0x2c + 0x5f8) = lVar14;
        *(long *)((long)param_10 + uVar15 * 0x2c + 0x600) = lVar21;
      }
      if (param_2 == 1) {
        if (*(char *)((long)param_6 + 0x1ac) != '\0') {
          sVar16 = strnlen((char *)((long)param_6 + 0x1ac),0x3f);
          uVar19 = (int)sVar16 + 1;
          *(undefined2 *)((long)param_10 + lVar13 * 0x8c + 0x750) = 0x205b;
          *(undefined4 *)((long)param_10 + lVar13 * 0x8c + 0x758) = 0x40;
          *(undefined4 *)((long)param_10 + lVar13 * 0x8c + 0x754) = 2;
          lVar13 = lVar13 * 0x8c + 0x75c;
          iVar11 = iVar11 + 1;
          memcpy((void *)((long)param_10 + lVar13),(char *)((long)param_6 + 0x1ac),(ulong)uVar19);
          memset((void *)((long)param_10 + (ulong)uVar19 + lVar13),0,(ulong)(0x40 - uVar19));
        }
      }
      else {
        bVar1 = true;
        if (param_2 == 2) goto LAB_001e60bc;
      }
      bVar1 = 0x1fa400 < (uint)(*(int *)(*param_10 + 0x5c) * *(int *)(*param_10 + 0x60));
LAB_001e60bc:
      if ((*(int *)((long)param_6 + 0xc) == 0) ||
         ((*(int *)((long)param_6 + 0xc) == 2 && ((int)param_10[0xd] != 0)))) {
        if (param_2 - 1U < 2) {
          if ((bVar1) && ((param_9 == '\0' && param_8 == '\x01' || (param_8 == '\0')))) {
            uVar12 = FUN_001e0654(param_10,uVar12);
            *(undefined1 *)((long)param_10 + 0x869) = 1;
          }
        }
        else if (param_2 == 4) {
          if ((bVar1) && ((param_9 == '\0' && param_8 == '\x01' || (param_8 == '\0')))) {
            *(undefined1 *)((long)param_10 + 0x869) = 1;
          }
          else {
            *(undefined1 *)((long)param_10 + 0x869) = 0;
            param_10[0xd] = 0;
            param_10[0xe] = 0;
            param_10[0xf] = 0;
          }
          uVar12 = FUN_001e0654(param_10,uVar12);
        }
      }
      *(uint *)(param_10 + 0xbd) = uVar12;
      *(int *)((long)param_10 + 0x74c) = iVar11;
      local_154 = 4;
      local_153 = 0;
      local_158 = 0x2120;
      local_152 = 0;
      local_156 = 0x1100;
      local_151 = 0;
      FUN_001df8d4(&local_158,8);
      return;
    }
    uVar19 = 0x574;
  }
LAB_001e55a4:
  pcVar17 = "0";
LAB_001e55b4:
                    /* WARNING: Subroutine does not return */
  __assert_fail(pcVar17,"integRecordStill.cpp",uVar19,
                "void mpr::stillrecord::_make_add_if_t(const mpr::recordStillInParam*, mpr::StillType, DefStruct::JpgInfo*, DefStruct::RawInfo*, DefStruct::HifInfo*, mpr::recordStillParam_t*, uint8_t*, bool, bool, still::add_if_t*, mpr::stillrecord::sidb_update_t*, dmm_munmap*, char*)"
               );
}


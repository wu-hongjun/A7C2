// Decompiled pseudocode, not original source.
// REA Evidence: ev_2e8ef4961152d79a5016e2f4e0e53e348ac02a77fca4579aa960df99eaec303e
// Source record: research/evidence/rea/sony-capture-allocation/00-procedure_pseudo_code.json

int _ZN3mpr7CaptureERKNS_14captureInParamEPNS_15captureOutParamE(long param_1,undefined4 *param_2)

{
  bool bVar1;
  bool bVar2;
  undefined4 *puVar3;
  int *piVar4;
  undefined4 uVar5;
  char cVar6;
  undefined2 uVar7;
  ushort uVar8;
  char cVar9;
  short sVar10;
  bool bVar11;
  int *piVar12;
  int *piVar13;
  char *pcVar14;
  bool bVar15;
  undefined1 uVar16;
  int iVar17;
  uint uVar18;
  uint uVar19;
  int iVar20;
  undefined4 uVar21;
  int iVar22;
  undefined4 *puVar23;
  undefined2 *puVar24;
  uint *puVar25;
  long *plVar26;
  void *pvVar27;
  undefined8 *puVar28;
  undefined8 uVar29;
  byte *pbVar30;
  ulong uVar31;
  char cVar32;
  undefined4 *puVar33;
  char *pcVar34;
  long lVar35;
  char *pcVar36;
  int *piVar37;
  char *pcVar38;
  char cVar39;
  char *pcVar40;
  long lVar41;
  char *pcVar42;
  uint *__s;
  ulong uVar43;
  undefined1 *puVar44;
  undefined8 *puVar45;
  uint *puVar46;
  long lVar47;
  ulong uVar48;
  long lVar49;
  int *piVar50;
  int *piVar51;
  ulong local_2818;
  int *local_2810;
  int *local_2808;
  undefined4 local_27e4;
  int local_27e0;
  int local_27d0;
  char *local_27c8;
  long local_27c0;
  undefined4 local_27b4;
  undefined1 auStack_27a0 [2464];
  undefined8 local_1e00;
  undefined8 uStack_1df8;
  undefined8 local_1df0;
  undefined4 local_1ddc;
  undefined1 *local_1dd8;
  undefined8 local_1dd0;
  undefined1 local_1dc8 [16];
  undefined8 *local_1db8 [4];
  undefined8 local_1d98;
  undefined8 local_1d68;
  undefined8 *local_1d60;
  undefined8 local_1d58;
  undefined8 uStack_1d50;
  undefined8 local_1d48;
  undefined8 local_1d40;
  int local_1d38 [18];
  uint local_1cf0;
  uint local_1cec;
  int local_1cd0;
  uint local_1ccc;
  undefined4 local_1c68;
  undefined4 local_1c64;
  undefined1 local_1c5f;
  byte local_1c5e;
  char local_1c1c;
  char acStack_1c1b [64];
  char local_1bdb;
  char acStack_1bda [64];
  char local_1b9a [66];
  long local_1b58;
  undefined4 local_1b50;
  undefined4 local_1b4c;
  undefined4 local_1b24;
  undefined4 local_1b20;
  undefined1 local_1b1c;
  undefined8 local_1b18;
  undefined8 uStack_1b10;
  undefined8 local_1b08;
  undefined8 uStack_1b00;
  undefined4 local_1af8;
  long local_1af0;
  char local_1ae8;
  undefined8 local_1ae4;
  undefined8 local_1adc;
  char local_1ad4 [68];
  undefined8 local_1a90;
  undefined4 local_1a88;
  undefined8 uStack_1a84;
  undefined4 local_1a7c;
  undefined2 local_1a78;
  ushort local_1a76;
  undefined1 local_1a74;
  undefined4 local_1a70;
  undefined1 local_1a6c;
  undefined1 local_1a6b;
  undefined1 local_1a6a;
  undefined1 local_1a40;
  undefined1 local_1a34;
  undefined1 local_19fc;
  undefined1 local_19f0;
  undefined1 local_19d0;
  char acStack_19cf [47];
  undefined1 local_19a0;
  undefined8 auStack_1950 [3];
  undefined2 auStack_1938 [55];
  undefined1 local_18c9;
  char acStack_18c8 [262];
  undefined1 local_17c2;
  char acStack_17c1 [262];
  undefined1 local_16bb;
  undefined8 local_158c;
  undefined8 uStack_1584;
  undefined8 local_157c;
  undefined8 uStack_1574;
  undefined8 local_156c;
  undefined8 uStack_1564;
  undefined8 local_155c;
  undefined8 uStack_1554;
  undefined8 local_154c;
  undefined8 uStack_1544;
  undefined8 local_153c;
  undefined8 uStack_1534;
  undefined1 local_14a0;
  undefined1 local_149f;
  undefined1 local_149e;
  undefined8 local_149d;
  undefined8 uStack_1495;
  undefined1 local_147e;
  undefined1 local_147d;
  uint local_1474;
  uint local_1470;
  uint local_146c [17];
  uint local_1428 [59];
  uint local_133c [2];
  char local_1334 [2140];
  undefined8 local_ad8;
  undefined4 local_ad0;
  undefined8 uStack_acc;
  undefined8 local_ac4;
  undefined8 auStack_8b8 [3];
  undefined1 auStack_8a0 [556];
  char local_674 [264];
  undefined8 local_56c;
  undefined8 uStack_564;
  undefined8 local_55c;

  FUN_001df8d4(param_1,0x20);
  _ZNK3mpr14captureInParam8DbgPrintEv(param_1);
  pthread_mutex_lock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
  param_2[1] = 0;
  puVar23 = param_2 + 10;
  puVar33 = param_2;
  do {
    puVar33[2] = 0xffff;
    puVar33[3] = 0xffffffff;
    puVar3 = (undefined4 *)((long)puVar23 + 2);
    *(undefined2 *)puVar23 = 0;
    *(undefined2 *)(puVar23 + 2) = 0;
    puVar23 = puVar3;
    puVar33 = puVar33 + 2;
  } while (puVar3 != param_2 + 0xc);
  _ZN3mpr16GetStillRecParamEPNS_13stillRecParamE(local_1d38);
  __s = &local_1470;
  memset(__s,0,0x994);
  local_1ddc = 1;
  if (local_1cf0 - 1 < 2) {
    iVar17 = _ZN11StillRemain24GetStillRemainRecordInfoEN9DefStruct9LocalTimeEN3mpr11CaptureTypeEPNS_17RecordInfoPerSlotEPNS_15CompressionTypeE
                       (*(undefined8 *)(param_1 + 4),*(undefined4 *)(param_1 + 0xc),
                        *(undefined4 *)(param_1 + 0x14),__s,&local_1ddc);
    if ((local_1d38[0] == 0) && (*(int *)(param_1 + 0x14) == 0 || *(int *)(param_1 + 0x14) == 3)) {
      uVar18 = _ZN3mpr8msidconv9GetSlotIDEj(&_ZN3mpr10g_msidconvE,local_1c68);
      uVar19 = _ZN3mpr8msidconv9GetSlotIDEj(&_ZN3mpr10g_msidconvE,local_1c64);
      bVar11 = uVar18 != 0xffffffff && uVar19 != 0xffffffff;
      if ((uVar18 == 0xffffffff || uVar19 == 0xffffffff) ||
         ((local_1428[(ulong)uVar18 * 0x13] != 0 || (local_1428[(ulong)uVar19 * 0x13] == 0))))
      goto LAB_001eba38;
      local_27e4 = local_1c64;
      local_27b4 = local_1c68;
    }
    else {
LAB_001eba38:
      local_27e4 = 0xffffffff;
      bVar11 = false;
      local_27b4 = 0xffffffff;
    }
    if (iVar17 != 0) {
      local_ad8 = 0;
      puVar28 = &local_ad8;
LAB_001eba54:
      _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE12emplace_backIJS2_EEEvDpOT_
                (_ZN3mpr4fifoE,puVar28);
      goto LAB_001eba64;
    }
    if (*(int *)(param_1 + 0x14) == 3) {
      puVar25 = local_146c;
      do {
        if (puVar25[0x11] != 0) {
          if (*puVar25 == 2) {
            *puVar25 = 1;
          }
          puVar25[1] = 1;
          puVar25[2] = 3;
          puVar25[3] = 2;
          puVar25[4] = 3;
          puVar25[5] = 0;
          puVar25[6] = 0xff;
          puVar25[7] = 0;
          puVar25[8] = 0;
          puVar25[9] = 0;
          puVar25[10] = 3;
          puVar25[0xb] = 3;
          puVar25[0xc] = 4;
          puVar25[0xd] = 10;
          puVar25[0xe] = 0xff;
          puVar25[0xf] = 0xffffffff;
          puVar25[0x10] = 0;
          for (piVar37 = &local_1cd0; piVar37 != local_1d38 + (ulong)local_1cf0 * 8 + 0x1a;
              piVar37 = piVar37 + 8) {
            if ((char)*piVar37 != '\0') {
              *(undefined1 *)(puVar25 + 8) = 1;
            }
          }
        }
        puVar25 = puVar25 + 0x13;
      } while (puVar25 != local_133c);
    }
    pcVar36 = local_1334 + 1;
    pcVar40 = pcVar36;
    do {
      pcVar40[-9] = '\0';
      pcVar40[-8] = '\0';
      pcVar40[-7] = '\0';
      pcVar40[-6] = '\0';
      pcVar40[-1] = '\0';
      pcVar40[-5] = '\0';
      pcVar40[-4] = '\0';
      pcVar40[-3] = '\0';
      pcVar40[-2] = '\0';
      memset(pcVar40,0,0x106);
      pcVar34 = pcVar40 + 0x106;
      pcVar40 = pcVar40 + 0x218;
      memset(pcVar34,0,0x106);
    } while (pcVar40 != (char *)((long)&local_ad8 + 5));
    _ZN3mpr18ConvMediaId2SlotIdERKNS_13stillRecParamEPNS_8SlotInfoE(local_1d38,&local_1d98);
    if (*(int *)(param_1 + 0x10) == 0) {
      DAT_00481854 = '\0';
      sVar10 = *(short *)(param_1 + 6);
      cVar9 = *(char *)(param_1 + 9);
      cVar39 = *(char *)(param_1 + 8);
LAB_001ebb98:
      local_27e0 = (int)cVar39;
      local_27d0 = (int)cVar9;
      iVar17 = (int)sVar10;
    }
    else {
      if (*(int *)(param_1 + 0x10) - 1U < 2) {
        if (DAT_00481854 == '\0') {
          DAT_00481854 = '\x01';
          DAT_00481848 = *(undefined8 *)(param_1 + 4);
          DAT_00481850 = *(undefined4 *)(param_1 + 0xc);
        }
        sVar10 = DAT_00481848._2_2_;
        cVar9 = DAT_00481848._5_1_;
        cVar39 = DAT_00481848._4_1_;
        goto LAB_001ebb98;
      }
      iVar17 = 0;
      local_27e0 = 0;
      local_27d0 = 0;
    }
    puVar24 = &local_1a78;
    do {
      *(undefined2 **)(puVar24 + -8) = puVar24;
      *(undefined8 *)(puVar24 + -4) = 0;
      *(undefined1 *)puVar24 = 0;
      *(undefined1 *)(puVar24 + 0x10) = 0;
      *(undefined2 **)(puVar24 + 8) = puVar24 + 0x10;
      *(undefined8 *)(puVar24 + 0xc) = 0;
      puVar24 = puVar24 + 0x28;
    } while (puVar24 != auStack_1938);
    puVar23 = &local_1a88;
    local_1a90 = (undefined2 *)((ulong)local_1a90 & 0xffffffffffffff00);
    local_1a40 = 0;
    local_19f0 = 0;
    uVar43 = 0;
    local_19a0 = 0;
    bVar1 = false;
    local_27c0 = (long)&local_1d98 + 6;
    lVar47 = local_27c0;
    do {
      if (local_1428[uVar43 * 0x13] != 0) {
        iVar20 = _ZN3mpr8msidconv9GetSlotIDEj(&_ZN3mpr10g_msidconvE,*_ZN3mpr13gp_paramBoardE);
        if (iVar20 == (int)uVar43) {
          if (0x270e < DAT_00481898) {
            DAT_00481898 = 0;
          }
          DAT_00481898 = DAT_00481898 + 1;
          local_1334[uVar43 * 0x218] = '\x01';
        }
        else {
          *(undefined1 *)(puVar23 + -2) = (&_ZN3mpr10g_msidconvE)[uVar43 * 0x80];
          local_1dd8 = local_1dc8;
          FUN_001df468(&local_1dd8,lVar47 + -6,lVar47 + -1);
          if (*(char *)(lVar47 + 5) != '\0') {
            local_1dd0 = 0;
            *local_1dd8 = 0;
            local_1d68 = 0;
            local_1d58 = 0;
            uStack_1d50 = 0;
            local_1d48 = 0;
            local_1d40 = 0;
            local_1d60 = &uStack_1d50;
            iVar20 = FUN_002229f4(uVar43 & 0xffffffff,&local_1d68);
            if (iVar20 != 3) {
              if ((ushort)local_1d40 < 0x3e6) {
                _ZNSt7__cxx1119basic_ostringstreamIcSt11char_traitsIcESaIcEEC1ESt13_Ios_Openmode
                          (&local_ad8,0x10);
                plVar26 = (long *)_ZNSolsEi(&local_ad8,
                                            (int)(short)((short)iVar17 + (short)(iVar17 / 10) * -10)
                                           );
                *(undefined8 *)((long)plVar26 + *(long *)(*plVar26 + -0x18) + 0x10) = 2;
                uVar29 = _ZStlsIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_St8_SetfillIS3_E
                                   (plVar26,0x30);
                plVar26 = (long *)_ZNSo9_M_insertImEERSoT_(uVar29,local_27e0);
                *(undefined8 *)((long)plVar26 + *(long *)(*plVar26 + -0x18) + 0x10) = 2;
                uVar29 = _ZStlsIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_St8_SetfillIS3_E
                                   (plVar26,0x30);
                _ZNSolsEj(uVar29,local_27d0);
                _ZNKSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEE3strEv
                          (local_1db8,&local_ad0);
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEOS4_(&local_1dd8,local_1db8)
                ;
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_1db8);
                _ZNSt7__cxx1119basic_ostringstreamIcSt11char_traitsIcESaIcEED1Ev(&local_ad8);
              }
              else {
                _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6substrEmm
                          (&local_ad8,&local_1d60,3,5);
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEOS4_(&local_1dd8,&local_ad8)
                ;
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_ad8);
              }
            }
            _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_1d60);
          }
          bVar1 = true;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_assignERKS4_(puVar23,&local_1dd8)
          ;
          local_ad8 = (long)&uStack_acc + 4;
          FUN_001df468(&local_ad8,lVar47,lVar47 + 4);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_assignERKS4_
                    (puVar23 + 8,&local_ad8);
          *(bool *)(puVar23 + 0x10) = bVar11;
          uVar21 = _ZN3mpr8msidconv9GetSlotIDEj(&_ZN3mpr10g_msidconvE,local_1c68);
          puVar23[0x11] = uVar21;
          *(undefined1 *)((long)puVar23 + 0x41) = *(undefined1 *)(lVar47 + 5);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_ad8);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_1dd8);
        }
      }
      uVar43 = uVar43 + 1;
      puVar23 = puVar23 + 0x14;
      lVar47 = lVar47 + 0xc;
    } while (uVar43 != 4);
    uVar18 = (uint)local_1c5e << 8;
    puVar44 = (undefined1 *)((long)&local_ac4 + 4);
    do {
      memset(puVar44 + -0x18,0,0x88);
      *(undefined1 **)(puVar44 + -0x10) = puVar44;
      *(undefined1 **)(puVar44 + 0x10) = puVar44 + 0x20;
      *(undefined1 **)(puVar44 + 0x30) = puVar44 + 0x40;
      *(undefined1 **)(puVar44 + 0x50) = puVar44 + 0x60;
      puVar44 = puVar44 + 0x88;
    } while (puVar44 != auStack_8a0);
    if ((local_1d38[0] != 0) && ((local_1d38[0] != 2 || (local_1cf0 != 1)))) {
      uVar18 = (uint)local_1c5e << 8 | 1;
    }
    if (bVar1) {
      FUN_00223ce8(&local_1a90,uVar18,&local_ad8);
    }
    lVar47 = 0;
    puVar25 = local_1428;
    puVar28 = &local_ad8;
    lVar41 = local_ad8;
    do {
      if (*puVar25 != 0) {
        puVar46 = puVar25 + -0x10;
        *(undefined2 *)(pcVar36 + -9) = *(undefined2 *)((long)puVar28 + 4);
        *(undefined2 *)(pcVar36 + -7) = *(undefined2 *)((long)puVar28 + 6);
        *(undefined4 *)(pcVar36 + -5) = *(undefined4 *)puVar28;
        pcVar40 = pcVar36;
        do {
          local_ad8 = lVar41;
          if (pcVar36[-1] == '\0') {
            uVar18 = *puVar46;
            if (uVar18 == 1) {
              pcVar34 = (char *)puVar28[1];
            }
            else if (uVar18 == 2) {
              pcVar34 = (char *)puVar28[5];
            }
            else {
              if (uVar18 != 4) goto LAB_001ec164;
              pcVar34 = (char *)puVar28[9];
            }
            strncpy(pcVar40,pcVar34,0x105);
            lVar41 = local_ad8;
          }
          else {
            _ZN3mpr22CreateFilePathInternalENS_9StillTypeEPcS1_(*puVar46,local_27c0,pcVar40);
            lVar41 = local_ad8;
          }
LAB_001ec164:
          puVar46 = puVar46 + 8;
          pcVar40 = pcVar40 + 0x106;
        } while (puVar46 != puVar25);
      }
      lVar47 = lVar47 + 0x218;
      puVar25 = puVar25 + 0x13;
      local_27c0 = local_27c0 + 0xc;
      puVar28 = puVar28 + 0x11;
      pcVar36 = pcVar36 + 0x218;
    } while (lVar47 != 0x860);
    uVar43 = 0;
    local_1d68 = 0;
    local_1d60 = (undefined8 *)0x0;
    local_1db8[0] = (undefined8 *)((ulong)local_1db8[0] & 0xffffffff00000000);
    puVar25 = local_146c;
    do {
      if ((puVar25[0x11] != 0) && (local_1334[uVar43 * 0x218] == '\0')) {
        local_ad8 = lVar41;
        for (uVar18 = 0; uVar18 < *puVar25; uVar18 = uVar18 + 1) {
          _ZN11StillRemain18GetEachStdFileSizeEN3mpr17StillPictureParamENS0_8LensTypeENS_15CompressionTypeEPj
                    (&local_1e00,local_1b4c,local_1ddc,local_1db8);
          *(int *)((long)&local_1d68 + uVar43 * 4) =
               *(int *)((long)&local_1d68 + uVar43 * 4) + (int)local_1db8[0];
        }
        FUN_0022a740(uVar43 & 0xffffffff,(long)*(int *)((long)&local_1d68 + uVar43 * 4));
        (&DAT_004818b8)[uVar43 * 3] = (&DAT_004818b8)[uVar43 * 3] + 1;
        lVar41 = local_ad8;
      }
      uVar43 = uVar43 + 1;
      puVar25 = puVar25 + 0x13;
    } while (uVar43 != 4);
    puVar28 = auStack_8b8;
    local_ad8 = lVar41;
    do {
      puVar45 = puVar28 + -0x11;
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(puVar28 + -4);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(puVar28 + -8);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(puVar28 + -0xc);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(puVar28 + -0x10);
      puVar28 = puVar45;
    } while (puVar45 != &local_ad8);
    puVar28 = auStack_1950;
    do {
      puVar45 = puVar28 + -10;
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(puVar28 + -5);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(puVar28 + -9);
      puVar28 = puVar45;
    } while (puVar45 != &local_1a90);
  }
  else {
    local_27e4 = 0xffffffff;
    bVar11 = false;
    local_27b4 = 0xffffffff;
  }
  pvVar27 = memcpy(auStack_27a0,__s,0x994);
  iVar17 = _ZN3mpr21DecideWriteFileFormatERKNS_13stillRecParamENS_11CaptureTypeENS_16WriteInfoPerSlotEPNS_16WriteInfoPerFileEPb
                     (local_1d38,*(undefined4 *)(param_1 + 0x14),pvVar27,&local_ad8,&local_1dd8);
  puVar28 = calloc(1,0xd08);
  *(undefined4 *)((long)puVar28 + 300) = 4;
  iVar20 = *(int *)(param_1 + 0x14);
  *(undefined4 *)((long)puVar28 + 0x14c) = 0;
  *(undefined4 *)(puVar28 + 0x2a) = 0;
  *(undefined4 *)((long)puVar28 + 0x154) = 0;
  *(undefined4 *)(puVar28 + 0x2b) = 0;
  *(undefined4 *)(puVar28 + 0x26) = 0xffffffff;
  *(undefined4 *)((long)puVar28 + 0x134) = 0xffffffff;
  *(undefined4 *)(puVar28 + 0x27) = 0xffffffff;
  *(undefined4 *)((long)puVar28 + 0x13c) = 0xffffffff;
  *(undefined8 *)((long)puVar28 + 500) = 0x300000000;
  *(undefined8 *)((long)puVar28 + 0x1fc) = 0x400000003;
  *(undefined8 *)((long)puVar28 + 0x204) = 0xff0000000a;
  *(undefined8 *)((long)puVar28 + 0x20c) = 0xffffffff;
  *(undefined8 *)((long)puVar28 + 0x77c) = 0x300000000;
  *(undefined8 *)((long)puVar28 + 0x784) = 0x400000003;
  *(undefined8 *)((long)puVar28 + 0x78c) = 0xff0000000a;
  *(undefined8 *)((long)puVar28 + 0x794) = 0xffffffff;
  uVar29 = *(undefined8 *)(param_1 + 4);
  *(char *)((long)puVar28 + 0x51) = local_1bdb;
  *puVar28 = uVar29;
  *(char *)((long)puVar28 + 0xec) = local_1ae8;
  *(int *)((long)puVar28 + 0xc) = iVar20;
  *(undefined4 *)(puVar28 + 1) = *(undefined4 *)(param_1 + 0xc);
  *(char *)(puVar28 + 2) = local_1c1c;
  local_1db8[0] = puVar28;
  if (local_1c1c != '\0') {
    strncpy((char *)((long)puVar28 + 0x11),acStack_1c1b,0x3f);
  }
  if (local_1bdb != '\0') {
    strncpy((char *)((long)puVar28 + 0x52),acStack_1bda,0x3f);
  }
  if (local_1b9a[0] != '\0') {
    strncpy((char *)((long)puVar28 + 0x92),local_1b9a,0x3f);
  }
  if (local_1ae8 != '\0') {
    puVar28[0x1e] = local_1ae4;
    puVar28[0x1f] = local_1adc;
  }
  if (local_1b58 != 0) {
    puVar28[0x1c] = local_1b58;
  }
  puVar28[0x20] = local_1b18;
  puVar28[0x21] = uStack_1b10;
  puVar28[0x22] = local_1b08;
  puVar28[0x23] = uStack_1b00;
  if (local_1af0 != 0) {
    puVar28[0x24] = local_1af0;
  }
  uVar21 = local_1b20;
  if (iVar20 == 3) {
    uVar21 = 0;
  }
  *(undefined4 *)((long)puVar28 + 0xd4) = local_1b24;
  *(undefined4 *)(puVar28 + 0x1b) = uVar21;
  uVar5 = *(undefined4 *)(param_1 + 0x18);
  uVar16 = local_1b1c;
  if (iVar20 == 3) {
    uVar16 = 0;
  }
  *(undefined1 *)((long)puVar28 + 0xdc) = uVar16;
  *(char *)((long)puVar28 + 0xde) = (char)uVar5;
  *(undefined4 *)(puVar28 + 0x1d) = local_1b50;
  *(undefined4 *)(puVar28 + 0x25) = local_1af8;
  *(undefined4 *)((long)puVar28 + 300) = *(undefined4 *)(param_1 + 0x10);
  if ((iVar20 == 0) && (local_1ad4[0] != '\0')) {
    strncpy((char *)((long)puVar28 + 0x1ac),local_1ad4,0x3f);
  }
  uVar18 = 0;
  if ((local_1cf0 != 0) && (uVar18 = local_1cec, 1 < local_1cf0)) {
    uVar18 = local_1cec | local_1ccc;
  }
  uVar16 = _ZN3mpr16IsC2PARecordableENS_9StillTypeES0_bNS_11CaptureTypeE
                     (uVar18,uVar21,uVar16,iVar20);
  *(undefined1 *)((long)puVar28 + 0xdd) = uVar16;
  pbVar30 = (byte *)(local_1db8[0] + 0x28);
  puVar25 = local_1428;
  lVar47 = 0;
  puVar23 = (undefined4 *)((long)local_1db8[0] + 0x15c);
  do {
    *pbVar30 = (&DAT_004818a0)[lVar47] ^ 1;
    if ((char)local_1dd8 == '\0') {
      if (*puVar25 < 2) {
        pbVar30[4] = 1;
        pbVar30[8] = 1;
      }
      else {
        pbVar30[4] = 0;
        pbVar30[8] = 0;
      }
    }
    else {
      pbVar30[4] = 1;
    }
    lVar41 = lVar47 + 1;
    pbVar30 = pbVar30 + 1;
    puVar25 = puVar25 + 0x13;
    *puVar23 = (&DAT_004818b8)[lVar47 * 3];
    lVar47 = lVar41;
    puVar23 = puVar23 + 1;
  } while (lVar41 != 4);
  pcVar36 = local_674;
  uVar43 = 0;
  local_1e00 = CONCAT44(local_ad0,local_ad8._4_4_);
  uStack_1df8 = uStack_acc;
  local_1df0 = local_ac4;
  _ZN11StillRemain18GetEachStdFileSizeEN3mpr17StillPictureParamENS0_8LensTypeENS_15CompressionTypeEPj
            (&local_1e00,local_1b4c,local_1ddc,(long)local_1db8[0] + 0x1ec);
  local_1e00 = local_56c;
  uStack_1df8 = uStack_564;
  local_1df0 = local_55c;
  lVar47 = 0x676;
  _ZN11StillRemain18GetEachStdFileSizeEN3mpr17StillPictureParamENS0_8LensTypeENS_15CompressionTypeEPj
            (&local_1e00,local_1b4c,local_1ddc,local_1db8[0] + 0x3e);
  do {
    lVar41 = lVar47 + -0x43a;
    pcVar40 = pcVar36 + -0x418;
    lVar49 = uVar43 * 0x588;
    uVar29 = *(undefined8 *)(pcVar36 + -0x458);
    *(undefined8 *)((long)local_1db8[0] + lVar49 + 500) = *(undefined8 *)(pcVar36 + -0x460);
    *(undefined8 *)((long)local_1db8[0] + lVar49 + 0x1fc) = uVar29;
    uVar29 = *(undefined8 *)(pcVar36 + -0x448);
    *(undefined8 *)((long)local_1db8[0] + lVar49 + 0x204) = *(undefined8 *)(pcVar36 + -0x450);
    *(undefined8 *)((long)local_1db8[0] + lVar49 + 0x20c) = uVar29;
    uVar48 = 0;
    pcVar34 = pcVar36 + -0x43c;
    pcVar38 = pcVar36 + -0x440;
    pcVar42 = pcVar36 + -0x42c;
    do {
      *(char *)((long)local_1db8[0] + uVar48 + lVar49 + 0x214) = *pcVar38;
      strncpy((char *)((long)local_1db8[0] + lVar41),pcVar40,0x105);
      lVar35 = uVar48 + uVar43 * 0x2c4;
      *(undefined2 *)((long)local_1db8[0] + lVar35 * 2 + 0x218) = *(undefined2 *)pcVar34;
      uVar18 = (int)uVar48 + 1;
      pcVar40 = pcVar40 + 0x106;
      *(undefined2 *)((long)local_1db8[0] + lVar35 * 2 + 0x220) = *(undefined2 *)(pcVar34 + 8);
      lVar41 = lVar41 + 0x106;
      *(undefined4 *)((long)local_1db8[0] + (uVar43 * 0x162 + uVar48) * 4 + 0x228) =
           *(undefined4 *)pcVar42;
      *(char *)((long)local_1db8[0] + uVar48 + lVar49 + 0x238) = pcVar38[0x24];
      uVar48 = (ulong)uVar18;
      pcVar34 = pcVar34 + 2;
      pcVar38 = pcVar38 + 1;
      pcVar42 = pcVar42 + 4;
    } while (uVar18 != 4);
    if (*pcVar36 != '\0') {
      *(undefined1 *)((long)local_1db8[0] + lVar49 + 0x674) = 1;
      *(undefined1 *)((long)local_1db8[0] + lVar49 + 0x675) = local_1c5f;
      strncpy((char *)((long)local_1db8[0] + lVar47),pcVar36,0x105);
    }
    uVar18 = (int)uVar43 + 1;
    uVar43 = (ulong)uVar18;
    lVar47 = lVar47 + 0x588;
    pcVar36 = pcVar36 + 0x568;
  } while (uVar18 != 2);
  _ZN3mpr4UMID11SetTimeInfoEPKN9DefStruct9LocalTimeE(_ZN3mpr7gp_umidE,param_1 + 4);
  if (*(int *)((long)local_1db8[0] + 0x794) == -1) {
    if (*(int *)((long)local_1db8[0] + 0x20c) != -1) goto LAB_001ec680;
  }
  else {
    if (*(int *)((long)local_1db8[0] + 0x20c) == -1) {
LAB_001ec680:
      _ZN3mpr4UMID15CreateBasicUMIDENS0_23_MATERIAL_NO_GEN_METHODENS_9StillTypeES2_NS0_14UmidCreateModeE
                (_ZN3mpr7gp_umidE,4,*(undefined4 *)((long)local_1db8[0] + 500),0,0);
      bVar15 = false;
      uVar21 = *(undefined4 *)((long)local_1db8[0] + 500);
      puVar28 = local_1db8[0];
    }
    else {
      uVar18 = *(uint *)((long)local_1db8[0] + 0x1ec);
      uVar19 = *(uint *)(local_1db8[0] + 0x3e);
      bVar2 = uVar19 > uVar18 || uVar18 == uVar19;
      bVar1 = uVar19 <= uVar18 && uVar18 != uVar19;
      lVar41 = (ulong)bVar2 * 0x588;
      bVar15 = *(int *)((long)local_1db8[0] + 500) == *(int *)((long)local_1db8[0] + 0x77c);
      lVar47 = (ulong)bVar1 * 0x588;
      _ZN3mpr4UMID15CreateBasicUMIDENS0_23_MATERIAL_NO_GEN_METHODENS_9StillTypeES2_NS0_14UmidCreateModeE
                (_ZN3mpr7gp_umidE,4,*(undefined4 *)((long)local_1db8[0] + lVar41 + 500),
                 *(undefined4 *)((long)local_1db8[0] + lVar47 + 500),bVar15);
      _ZN3mpr4UMID12GetBasicUMIDENS_10StillImageENS_9StillTypeENS0_11UmidGetModeEPh
                (_ZN3mpr7gp_umidE,0,*(undefined4 *)((long)local_1db8[0] + lVar41 + 500),0,
                 (long)local_1db8[0] + (ulong)bVar2 * 0x588 + 0x654);
      puVar28 = local_1db8[0] + (ulong)bVar1 * 0xb1;
      uVar21 = *(undefined4 *)((long)local_1db8[0] + lVar47 + 500);
    }
    _ZN3mpr4UMID12GetBasicUMIDENS_10StillImageENS_9StillTypeENS0_11UmidGetModeEPh
              (_ZN3mpr7gp_umidE,0,uVar21,bVar15,(long)puVar28 + 0x654);
  }
  local_1d68 = 0;
  local_1d60 = (undefined8 *)0x0;
  if (((char)local_1dd8 == '\0') && (*(int *)(param_1 + 0x10) - 1U < 2)) {
    _ZN3mpr16UniversalGroupId11SetTimeInfoEPKN9DefStruct9LocalTimeE
              (_ZN3mpr19gp_universalGroupIdE,param_1 + 4);
    _ZN3mpr16UniversalGroupId22CreateUniversalGroupIdENS0_25UniversalGroupIdGenMethodEPh
              (_ZN3mpr19gp_universalGroupIdE,4,&local_1d68);
  }
  else {
    local_1d68 = 0;
    local_1d60 = (undefined8 *)0x0;
  }
  puVar28 = &local_1a90;
  uVar48 = 0;
  lVar47 = 0x16c;
  _ZN3mpr11stillrecord18clear_pseudo_stillEb(1);
  uVar43 = 0;
  local_2818 = 0;
  cVar9 = '\0';
  uVar18 = 0;
  do {
    piVar37 = (int *)((long)local_1db8[0] + 500);
    lVar41 = (long)local_1db8[0] + uVar43 * 2;
    pcVar36 = (char *)((long)local_1db8[0] + uVar43 * 0x106 + 0x23c);
    cVar32 = '\0';
    local_2810 = (int *)0x0;
    local_2808 = (int *)0x0;
    uVar8 = 0;
    uVar7 = 0;
    local_27c8 = (char *)0x0;
    pcVar40 = (char *)0x0;
    pcVar34 = (char *)0x0;
    piVar50 = (int *)0x0;
    cVar39 = DAT_0048189c;
    do {
      pcVar38 = pcVar40;
      pcVar42 = pcVar34;
      piVar51 = piVar50;
      if ((piVar37[6] != -1) && (*pcVar36 != '\0')) {
        iVar20 = *piVar37;
        piVar4 = piVar37 + 0x118;
        pcVar38 = pcVar36;
        piVar12 = local_2810;
        piVar13 = piVar4;
        pcVar14 = local_27c8;
        if ((iVar20 != 1) &&
           ((pcVar38 = pcVar40, piVar12 = piVar4, piVar13 = local_2808, pcVar14 = pcVar36,
            iVar20 != 2 &&
            (pcVar42 = pcVar36, piVar51 = piVar4, piVar12 = local_2810, pcVar14 = local_27c8,
            iVar20 != 4)))) {
          iVar17 = 3;
          pcVar42 = pcVar34;
          piVar51 = piVar50;
        }
        local_27c8 = pcVar14;
        local_2808 = piVar13;
        local_2810 = piVar12;
        uVar7 = *(undefined2 *)(lVar41 + 0x220);
        uVar8 = *(ushort *)(lVar41 + 0x218);
        cVar6 = *(char *)((long)piVar37 + uVar43 + 0x44);
        if (cVar6 != '\0') {
          cVar32 = cVar6;
          cVar39 = cVar6;
        }
      }
      piVar37 = piVar37 + 0x162;
      pcVar36 = pcVar36 + 0x588;
      lVar41 = lVar41 + 0x588;
      pcVar40 = pcVar38;
      pcVar34 = pcVar42;
      piVar50 = piVar51;
    } while ((int *)((long)local_1db8[0] + 0xd04) != piVar37);
    if (cVar32 != '\0') {
      DAT_0048189c = cVar39;
    }
    if (iVar17 != 0) {
      free(local_1db8[0]);
      local_1a90 = (undefined2 *)0x0;
      puVar28 = &local_1a90;
      goto LAB_001eba54;
    }
    if ((pcVar38 != (char *)0x0 || local_27c8 != (char *)0x0) || pcVar42 != (char *)0x0) {
      lVar41 = (uVar43 + 0x4c) * 4;
      iVar20 = *(int *)(lVar47 * 8 + 0x4741cc);
      iVar22 = _ZN4sidb13IssueUniqueIdEjPjb(iVar20,(long)local_1db8[0] + lVar41,1);
      if (iVar22 != 0) {
LAB_001ec95c:
        free(local_1db8[0]);
        local_1a90 = (undefined2 *)0x0;
        goto LAB_001ec96c;
      }
      if (*(char *)((long)local_1db8[0] + (uVar43 & 0xffffffff) + 0x140) == '\0') {
        *(int *)(_ZN3mpr22s_integRecordMovieLockE + lVar41) =
             *(int *)(_ZN3mpr22s_integRecordMovieLockE + lVar41) + 1;
      }
      else {
        if (((char)local_1dd8 == '\0') && (*(int *)(param_1 + 0x10) - 1U < 2)) {
          iVar22 = _ZN4sidb12IssueGroupIdEjPj(iVar20,(long)&DAT_004818f8 + lVar41);
          if (iVar22 != 0) goto LAB_001ec95c;
        }
        else {
          *(undefined4 *)((long)&DAT_004818f8 + lVar41) = 0;
        }
        *(undefined4 *)(_ZN3mpr22s_integRecordMovieLockE + lVar41) = 1;
        *(undefined8 *)(lVar47 + 0x4816ec) = local_1d68;
        *(undefined8 **)(lVar47 + 0x4816f4) = local_1d60;
      }
      *(undefined4 *)((long)local_1db8[0] + (uVar43 & 0xffffffff) * 4 + 0x14c) =
           *(undefined4 *)(_ZN3mpr22s_integRecordMovieLockE + lVar41);
      uVar29 = *(undefined8 *)(lVar47 + 0x4816f4);
      *(undefined8 *)((long)local_1db8[0] + lVar47) = *(undefined8 *)(lVar47 + 0x4816ec);
      ((undefined8 *)((long)local_1db8[0] + lVar47))[1] = uVar29;
      _ZN4sidb10recordInfo5clearEv(puVar28);
      puVar45 = local_1db8[0];
      local_1a90 = (undefined2 *)
                   CONCAT44(*(undefined4 *)((long)local_1db8[0] + uVar43 * 4 + 0x130),1);
      local_1a88 = *(undefined4 *)((long)&DAT_004818f8 + lVar41);
      uStack_1a84 = *local_1db8[0];
      local_1a7c = *(undefined4 *)(local_1db8[0] + 1);
      local_1a74 = *(undefined1 *)((long)local_1db8[0] + (uVar43 & 0xffffffff) + 0x140);
      local_1a6c = 0;
      local_1a70 = *(undefined4 *)((long)local_1db8[0] + 300);
      local_1a6a = 0;
      local_1a34 = 0;
      local_1a6b = (undefined1)*(undefined4 *)(param_1 + 0x18);
      local_19fc = 0;
      local_14a0 = *(int *)(param_1 + 0x14) == 3;
      local_19d0 = pcVar38 != (char *)0x0;
      local_149f = *(undefined1 *)((long)local_1db8[0] + 0xdc);
      local_149e = *(undefined1 *)((long)local_1db8[0] + 0xdd);
      local_18c9 = local_27c8 != (char *)0x0;
      local_17c2 = pcVar42 != (char *)0x0;
      local_16bb = 0;
      local_1a78 = uVar7;
      local_1a76 = uVar8;
      if (pcVar38 != (char *)0x0) {
        strncpy(acStack_19cf,pcVar38,0x106);
        local_158c = *(undefined8 *)local_2808;
        uStack_1584 = *(undefined8 *)(local_2808 + 2);
        local_157c = *(undefined8 *)(local_2808 + 4);
        uStack_1574 = *(undefined8 *)(local_2808 + 6);
      }
      if (local_27c8 != (char *)0x0) {
        strncpy(acStack_18c8,local_27c8,0x106);
        local_156c = *(undefined8 *)local_2810;
        uStack_1564 = *(undefined8 *)(local_2810 + 2);
        local_155c = *(undefined8 *)(local_2810 + 4);
        uStack_1554 = *(undefined8 *)(local_2810 + 6);
      }
      if (pcVar42 != (char *)0x0) {
        strncpy(acStack_17c1,pcVar42,0x106);
        local_154c = *(undefined8 *)piVar51;
        uStack_1544 = *(undefined8 *)(piVar51 + 2);
        local_153c = *(undefined8 *)(piVar51 + 4);
        uStack_1534 = *(undefined8 *)(piVar51 + 6);
      }
      local_147e = *(undefined1 *)(param_1 + 0x1c);
      local_147d = *(undefined1 *)(param_1 + 0x1d);
      local_149d = *(undefined8 *)((long)puVar45 + lVar47);
      uStack_1495 = ((undefined8 *)((long)puVar45 + lVar47))[1];
      local_1474 = (uint)((byte)local_1474 | 2);
      local_1474 = local_1474 & 0x1f;
      if (*(int *)((long)puVar45 + 300) - 1U < 2) {
        iVar22 = *(int *)((long)puVar45 + uVar43 * 4 + 0x14c);
        if (iVar22 == 1) {
          local_1a88 = 0;
          local_1a70 = 0;
          *(undefined4 *)(_ZN3mpr35g_orderRegistrationEvtStillTransferE + lVar41 + 0x10) =
               *(undefined4 *)((long)puVar45 + uVar43 * 4 + 0x130);
        }
        else {
          if (iVar22 == 2) {
            if (*(int *)(_ZN3mpr35g_orderRegistrationEvtStillTransferE + lVar41 + 0x10) == 0) break;
            _ZN4sidb13PartialUpdateIjEEijjNS_6COL_IDET_bb
                      (0xffffffff,
                       *(int *)(_ZN3mpr35g_orderRegistrationEvtStillTransferE + lVar41 + 0x10),9,
                       *(int *)((long)puVar45 + 300),1,0);
            _ZN4sidb13PartialUpdateIjEEijjNS_6COL_IDET_bb
                      (0xffffffff,
                       *(undefined4 *)
                        (_ZN3mpr35g_orderRegistrationEvtStillTransferE + lVar41 + 0x10),2,
                       *(undefined4 *)((long)&DAT_004818f8 + lVar41),1,0);
          }
          else if (*(int *)(_ZN3mpr35g_orderRegistrationEvtStillTransferE + lVar41 + 0x10) == 0)
          goto LAB_001ecca8;
          *(undefined4 *)(_ZN3mpr35g_orderRegistrationEvtStillTransferE + lVar41 + 0x10) = 0;
        }
      }
LAB_001ecca8:
      iVar22 = _ZN4sidb6UpdateEjRKNS_10recordInfoEbb(0xffffffff,puVar28,1,0);
      if (iVar22 != 0) {
        _ZN4sidb6DeleteEjjbb
                  (0xffffffff,
                   *(undefined4 *)((long)local_1db8[0] + (uVar43 & 0xffffffff) * 4 + 0x130),1,0);
        free(local_1db8[0]);
        local_1d98 = 0;
        puVar28 = &local_1d98;
LAB_001ec96c:
        _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE12emplace_backIJS2_EEEvDpOT_
                  (_ZN3mpr4fifoE,puVar28);
        pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
        _ZNK3mpr15captureOutParam8DbgPrintEv(param_2);
        FUN_001df8d4(param_2,0x38);
        return 2;
      }
      uVar31 = (ulong)uVar18;
      param_2[uVar31 * 2 + 2] = iVar20;
      param_2[uVar31 * 2 + 3] =
           *(undefined4 *)((long)local_1db8[0] + (uVar43 & 0xffffffff) * 4 + 0x130);
      *(undefined2 *)((long)param_2 + uVar31 * 2 + 0x28) = uVar7;
      *(ushort *)((long)param_2 + uVar31 * 2 + 0x30) = uVar8;
      uVar18 = uVar18 + 1;
      if (uVar18 == local_1470) {
        param_2[1] = uVar18;
      }
      cVar39 = *(char *)((long)local_1db8[0] + (uVar43 & 0xffffffff) + 0x140);
      if (cVar39 != '\0') {
        if (iVar20 == 0x1000) {
          uVar19 = 1;
        }
        else {
          if (iVar20 != 0x1010) goto LAB_001ece08;
          uVar19 = 2;
        }
        uVar48 = (ulong)(uVar19 | (uint)uVar48);
        local_2818 = (ulong)uVar8;
        cVar9 = cVar39;
      }
    }
LAB_001ece08:
    uVar43 = uVar43 + 1;
    lVar47 = lVar47 + 0x10;
  } while (uVar43 != 4);
  if (cVar9 != '\0') {
    iVar20 = osal_valloc_msg_wait(0x811006e9,&local_1a90,0x14,0);
    if (iVar20 == 0) {
      *local_1a90 = 0xd5;
      *(undefined4 *)(local_1a90 + 1) = 0x22041;
      *(ulong *)(local_1a90 + 5) = local_2818;
      *(undefined4 *)(local_1a90 + 3) = 0x80000010;
      iVar20 = osal_snd_msg(0x811006e9);
      if (iVar20 != 0) {
        osal_free_msg(0x811006e9,&local_1a90);
      }
    }
    iVar20 = osal_valloc_msg_wait(0x811006e9,&local_1a90,0x14,0);
    if (iVar20 == 0) {
      *local_1a90 = 0xd5;
      *(ulong *)(local_1a90 + 5) = uVar48;
      *(undefined4 *)(local_1a90 + 1) = 0x22071;
      *(undefined4 *)(local_1a90 + 3) = 0x80000010;
      iVar20 = osal_snd_msg(0x811006e9);
      if (iVar20 != 0) {
        osal_free_msg(0x811006e9,&local_1a90);
      }
    }
  }
  uVar43 = 0;
  _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE9push_backERKS2_(_ZN3mpr4fifoE,local_1db8);
  FUN_0022fe1c(0);
  do {
    __s = __s + 0x13;
    uVar18 = *__s;
    (&DAT_004818a0)[uVar43] = *(byte *)((long)local_1db8[0] + (uVar43 & 0xffffffff) + 0x144) ^ 1;
    if (uVar18 == 1) {
      FUN_0022a90c(uVar43 & 0xffffffff);
    }
    uVar43 = uVar43 + 1;
  } while (uVar43 != 4);
  if (bVar11) {
    local_1c68 = local_27e4;
    local_1c64 = local_27b4;
    _ZN3mpr24SetStillRecParamInternalERKNS_13stillRecParamEbb(local_1d38,0,0);
    local_1a90 = (undefined2 *)CONCAT44(local_27e4,0x100e);
    local_1a88 = local_27b4;
    _ZN3mpr13SetIssueEventENS_5EVTIDEPvm(0x100e,&local_1a90,0xc);
  }
LAB_001eba64:
  pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
  _ZNK3mpr15captureOutParam8DbgPrintEv(param_2);
  FUN_001df8d4(param_2,0x38);
  return iVar17;
}

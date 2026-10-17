// Decompiled pseudocode, not original source.
// REA Evidence: ev_a1016b61e20627fc080b541b24b186fe205c8e6960745c94b964966ab3adada9
// Source record: research/evidence/rea/libmpr-still/01-procedure_pseudo_code.json

/* WARNING: Type propagation algorithm not settling */

int _ZN3mpr11stillrecord13_record_stillERKNS_18recordStillInParamEPNS_18recordStillParam_tE
              (undefined8 *param_1,long param_2)

{
  int iVar1;
  byte bVar2;
  uint5 uVar3;
  bool bVar4;
  bool bVar5;
  byte bVar6;
  int iVar7;
  undefined4 uVar8;
  int iVar9;
  int iVar10;
  size_t sVar11;
  char *pcVar12;
  long *plVar13;
  int *piVar14;
  size_t sVar15;
  undefined8 *puVar16;
  undefined1 *puVar17;
  long *plVar18;
  ulong uVar19;
  long *plVar20;
  long lVar21;
  void *pvVar22;
  char *pcVar23;
  undefined8 uVar24;
  undefined1 *puVar25;
  long lVar26;
  undefined4 *puVar27;
  char *pcVar28;
  ulong uVar29;
  uint uVar30;
  int iVar31;
  uint *puVar32;
  char *__string;
  int *piVar33;
  long *in_stack_ffffffffffffcb10;
  undefined1 local_3444;
  undefined1 local_3440;
  undefined1 local_343c;
  undefined1 local_3438;
  undefined1 local_3434;
  undefined2 local_3430;
  undefined2 local_342c;
  undefined2 local_33f8;
  long local_33c8;
  long *local_33b8;
  long local_33a8;
  long *local_33a0;
  char *local_3398;
  undefined2 local_3388 [4];
  undefined2 local_3380 [4];
  undefined2 local_3378 [4];
  undefined2 local_3370 [4];
  undefined2 local_3368 [4];
  undefined2 local_3360 [4];
  long local_3358;
  undefined8 local_3350;
  undefined4 local_3348;
  undefined1 auStack_3344 [20];
  long local_3330 [10];
  undefined8 local_32e0;
  int local_32d8;
  uint uStack_32d4;
  uint local_32d0;
  uint uStack_32cc;
  undefined8 local_32c8;
  undefined8 uStack_32c0;
  undefined8 local_32b8;
  undefined8 uStack_32b0;
  undefined8 local_32a8;
  undefined4 local_32a0;
  undefined1 local_329c;
  undefined1 auStack_3298 [16];
  long local_3288;
  undefined1 auStack_3268 [144];
  undefined1 auStack_31d8 [168];
  undefined1 auStack_3130 [168];
  undefined1 auStack_3088 [168];
  undefined1 auStack_2fe0 [168];
  undefined1 auStack_2f38 [168];
  undefined1 auStack_2e90 [12];
  int local_2e84;
  undefined1 auStack_2de8 [384];
  undefined1 auStack_2c68 [448];
  undefined1 auStack_2aa8 [672];
  undefined2 local_2808;
  undefined2 local_2806;
  undefined1 local_2804;
  undefined1 local_2803;
  undefined1 local_2802;
  undefined1 local_2801;
  int local_2800;
  undefined4 local_27fc;
  int local_27f8;
  undefined4 local_27f4;
  undefined1 auStack_2518 [432];
  long alStack_2368 [12];
  long alStack_2308 [102];
  undefined8 local_1fd8;
  undefined8 local_1fd0;
  undefined8 local_1fc8;
  long local_1fc0 [2];
  undefined8 uStack_1fb0;
  undefined8 local_1fa8;
  undefined8 uStack_1fa0;
  undefined8 local_1f98;
  undefined8 uStack_1f90;
  undefined8 local_1f88;
  char local_1f18 [527];
  char acStack_1d09 [121];
  long local_1c90;
  size_t local_1c88;
  byte local_1be4;
  byte local_1be1;
  byte local_1bde;
  char local_1bdd;
  undefined2 local_1afc;
  undefined2 local_1afa;
  undefined2 local_1af8;
  undefined2 local_1af6;
  undefined2 local_1af4;
  undefined2 local_1af2;
  undefined4 local_1af0;
  undefined2 local_1aec;
  undefined4 local_1ae8;
  undefined2 local_1ae4;
  undefined4 local_1ae0;
  undefined2 local_1adc;
  byte local_19bc;
  code *local_1988;
  long lStack_1980;
  undefined1 auStack_1974 [220];
  undefined8 auStack_1898 [30];
  long local_17a8 [217];
  undefined2 local_10e0;
  undefined2 local_10de;
  undefined1 local_10dc;
  undefined1 local_10db;
  undefined1 local_10da;
  undefined1 local_10d9;
  undefined4 local_10b8;
  undefined8 auStack_f38 [67];
  undefined1 auStack_d1c [1236];
  undefined4 local_848;
  
  puVar25 = auStack_3344 + 4;
  lVar26 = 0;
  local_10dc = 4;
  local_10db = 0;
  local_10da = 0;
  local_10d9 = 0;
  local_10e0 = 0x2120;
  local_10de = 0x2700;
  FUN_001df8d4(&local_10e0,8);
  local_3398 = (char *)(param_2 + 0x23c);
  pcVar28 = local_3398;
  do {
    if (*(int *)(pcVar28 + -0x30) == -1) {
      *(undefined8 *)(puVar25 + lVar26) = 0;
    }
    else {
      piVar14 = (int *)_Znwm(4);
      *(int **)(puVar25 + lVar26) = piVar14;
      *piVar14 = 0;
      pcVar23 = pcVar28;
      do {
        if (*pcVar23 != '\0') {
          *piVar14 = *piVar14 + 1;
        }
        pcVar23 = pcVar23 + 0x106;
      } while (pcVar23 != pcVar28 + 0x418);
    }
    lVar26 = lVar26 + 8;
    pcVar28 = pcVar28 + 0x588;
  } while (lVar26 != 0x10);
  iVar1 = *(int *)(param_2 + 0xc);
  lVar26 = 0;
  bVar5 = iVar1 == 1;
  local_3330[0] = 0;
  local_3330[1] = 0;
  local_3330[2] = 0;
  local_3330[3] = 0;
  local_3330[4] = 0;
  local_3330[5] = 0;
  puVar16 = param_1;
  do {
    pvVar22 = (void *)puVar16[1];
    if (pvVar22 != (void *)0x0) {
      pvVar22 = memcpy(auStack_2de8 + lVar26 * 0x18,pvVar22,0xc0);
      *(void **)((long)local_3330 + lVar26) = pvVar22;
    }
    if ((void *)puVar16[3] != (void *)0x0) {
      pvVar22 = memcpy(auStack_2aa8 + lVar26 * 0x2a,(void *)puVar16[3],0x150);
      *(void **)((long)local_3330 + lVar26 + 0x10) = pvVar22;
    }
    if ((void *)puVar16[5] != (void *)0x0) {
      pvVar22 = memcpy(auStack_2c68 + lVar26 * 0x1c,(void *)puVar16[5],0xe0);
      *(void **)((long)local_3330 + lVar26 + 0x20) = pvVar22;
    }
    lVar26 = lVar26 + 8;
    puVar16 = puVar16 + 1;
  } while (lVar26 != 0x10);
  puVar32 = (uint *)(param_2 + 0x20c);
  uVar29 = 0;
  if (iVar1 == 3) {
    do {
      uVar30 = *puVar32;
      if (uVar30 != 0xffffffff) {
        memset(auStack_2de8 + (uVar29 & 0xffffffff) * 0xc0,0,0xc0);
        _ZN3mpr11stillrecord21_make_dummy_stillinfoEPN9DefStruct7JpgInfoEP10dmm_munmap
                  (auStack_2de8 + (ulong)uVar30 * 0xc0,*(undefined8 *)(puVar25 + uVar29 * 8));
        local_3330[uVar30] = (long)(auStack_2de8 + (ulong)uVar30 * 0xc0);
      }
      uVar29 = uVar29 + 1;
      puVar32 = puVar32 + 0x162;
    } while (uVar29 != 2);
  }
  local_3388[0] = 0;
  local_3380[0] = 0;
  uVar29 = 0;
  local_3330[6] = 0;
  local_3330[7] = 0;
  local_3378[0] = 0;
  local_3370[0] = 0;
  local_3330[8] = 0;
  local_3330[9] = 0;
  local_3368[0] = 0;
  local_3360[0] = 0;
  local_3358 = 0;
  memset(&local_10e0,0,0x10e0);
  local_329c = 0;
  local_3350 = 0;
  local_32c8 = 0;
  uStack_32c0 = 0;
  local_3348 = 0;
  local_32a8 = 0;
  local_32b8 = 0;
  uStack_32b0 = 0;
  local_32a0 = 0;
  local_10b8 = 4;
  local_848 = 4;
  memset(&lStack_1980,0,0x8a0);
  memset(auStack_2518,0,0x540);
  _ZN3mpr10GetXmpUUIDEPci(&local_32c8,0x2d);
  local_33c8 = 0;
  local_33b8 = local_17a8;
  plVar13 = &lStack_1980;
  local_33a8 = param_2;
  local_33a0 = (long *)&local_10e0;
  do {
    uVar30 = *(uint *)(local_33a8 + 0x20c);
    if (uVar30 != 0xffffffff) {
      iVar10 = *(int *)(local_33a8 + 500);
      pcVar28 = (char *)(local_33a8 + 0x654);
      if (*(int *)(param_2 + 0xc) == 3) {
        lVar26 = _Znwm(0x1558);
        FUN_0036ffac();
        local_3330[uVar29 + 6] = lVar26;
      }
      else {
        lVar26 = _ZN9stillutil7createrEN3mpr9StillTypeE(iVar10);
        local_3330[uVar29 + 6] = lVar26;
      }
      if (iVar1 == 1) {
        pcVar23 = local_1f18 + 1;
        if (iVar10 != 1) {
          pcVar23 = acStack_1d09;
        }
        __string = (char *)(local_33a8 + 0x23c);
        do {
          if (*__string != '\0') {
            _ZN4sidb10recordInfo5clearEv(&local_1fd8);
            sVar11 = strnlen(__string,0x105);
            iVar7 = _ZN4sidb6SelectEjjPNS_10recordInfoEb
                              (0xffffffff,*(undefined4 *)(param_1 + 0x16),&local_1fd8,1);
            if (iVar7 == 0) {
              if (iVar10 == 1) {
                pcVar12 = local_1f18;
LAB_001e88e8:
                if (*pcVar12 != '\0') {
                  sVar15 = strnlen(pcVar23,0x105);
                  *(undefined4 *)(__string + ((sVar11 & 0xffffffff) - 0xc)) =
                       *(undefined4 *)(pcVar23 + ((sVar15 & 0xffffffff) - 0xc));
                  memcpy((void *)((long)plVar13 + 0x347),__string,0x106);
                  goto LAB_001e8b3c;
                }
              }
              else if (iVar10 == 4) {
                pcVar12 = local_1f18 + 0x20e;
                goto LAB_001e88e8;
              }
              local_2803 = 0;
              iVar7 = -10;
              local_2804 = 3;
              local_2802 = 0;
              local_2808 = 0x2120;
              local_2801 = 0;
              local_2806 = 10000;
              local_2800 = iVar10;
              FUN_001df8d4(&local_2808,0xc);
            }
            else {
              local_2803 = 0;
              local_2804 = 3;
              local_2802 = 0;
              local_2808 = 0x2120;
              local_2801 = 0;
              local_2806 = 0x2720;
              local_2800 = iVar7;
              FUN_001df8d4(&local_2808,0xc);
            }
            goto LAB_001e8928;
          }
LAB_001e8b3c:
          __string = __string + 0x106;
        } while (__string != pcVar28);
        uVar8 = *(undefined4 *)(local_33a8 + 0x200);
        memset(auStack_31d8,0,0xa8);
        memset(auStack_3130,0,0xa8);
        memset(auStack_3088,0,0xa8);
        memset(auStack_2fe0,0,0xa8);
        memset(auStack_2f38,0,0xa8);
        memset(auStack_2e90,0,0xa8);
        if (*(int *)((long)param_1 + 0x44) == 0) {
          iVar7 = -2;
          local_1fd8 = (undefined1 *)0x429002120;
          FUN_001df8d4(&local_1fd8,8);
          goto LAB_001e8928;
        }
        lVar26 = FUN_003b21c8(*(undefined4 *)(param_1 + 9),auStack_31d8,1,
                              *(undefined8 *)(puVar25 + uVar29 * 8));
        if (lVar26 == 0) {
          uVar24 = 8;
          uVar3 = 0x428002120;
LAB_001e8c38:
          local_1fd8 = (undefined1 *)(ulong)uVar3;
          puVar16 = &local_1fd8;
        }
        else {
          if (1 < *(uint *)((long)param_1 + 0x44)) {
            lVar26 = FUN_003b21c8(*(undefined4 *)(param_1 + 0xb),auStack_3130,1,
                                  *(undefined8 *)(puVar25 + uVar29 * 8));
            if (lVar26 != 0) {
              puVar17 = auStack_3088;
              if (*(int *)((long)param_1 + 100) == 0x698) {
                puVar17 = auStack_2fe0;
              }
              memcpy(puVar17,auStack_3130,0xa8);
              goto LAB_001e8d14;
            }
            uVar3 = 0x328802120;
            uVar8 = *(undefined4 *)(param_1 + 0xb);
LAB_001e8cec:
            uVar24 = 0xc;
            local_1fd0 = (undefined2 *)CONCAT44(local_1fd0._4_4_,uVar8);
            goto LAB_001e8c38;
          }
LAB_001e8d14:
          if ((*(int *)((long)param_1 + 0x44) == 3) &&
             (lVar26 = FUN_003b21c8(*(undefined4 *)(param_1 + 0xd),auStack_2fe0,1,
                                    *(undefined8 *)(puVar25 + uVar29 * 8)), lVar26 == 0)) {
            uVar3 = 0x328882120;
            uVar8 = *(undefined4 *)(param_1 + 0xd);
            goto LAB_001e8cec;
          }
          FUN_001c4f1c(auStack_31d8,auStack_3298);
          plVar18 = (long *)_ZN9stillutil7createrEN3mpr9StillTypeE(*(undefined4 *)(param_1 + 0xf));
          if (plVar18 == (long *)0x0) {
            iVar31 = 0;
            iVar10 = 0;
            iVar9 = 0;
            iVar7 = 0;
LAB_001e9048:
            puVar16 = param_1 + 9;
            for (uVar30 = 0; uVar30 < *(uint *)((long)param_1 + 0x44); uVar30 = uVar30 + 1) {
              FUN_003b24d8(*(undefined4 *)puVar16);
              puVar16 = puVar16 + 2;
            }
            if ((iVar10 != 0 || iVar31 != 0) || (iVar7 != 0 || iVar9 != 0)) {
              iVar7 = -4;
              local_1fd8 = (undefined1 *)0x42b002120;
              FUN_001df8d4(&local_1fd8,8);
              goto LAB_001e8928;
            }
            goto LAB_001e937c;
          }
          iVar7 = _ZN8libstill6ana_ifEPN5still8ana_if_tEN3mpr10StillImageE(plVar18,auStack_3298,0);
          if (iVar7 != 0) {
            iVar31 = 0;
            iVar10 = 0;
            iVar9 = 0;
LAB_001e9038:
            (**(code **)(*plVar18 + 8))(plVar18);
            goto LAB_001e9048;
          }
          memset(&local_2808,0,0x2ec);
          memset(auStack_3268,0,0x90);
          memset(&local_1fd8,0,0x400);
          local_1fd8 = auStack_3268;
          local_1fd0 = &local_2808;
          iVar9 = FUN_00370dc4(plVar18,&local_1fd8);
          if ((((iVar9 != 0) || (local_1be4 == 0)) || (local_1be1 == 0)) ||
             ((local_1bde == 0 || (local_1bdd == '\0')))) {
            local_32e0._5_1_ = 0;
            iVar10 = 0;
            local_32e0._4_1_ = 1;
            local_32e0._6_1_ = 0;
            iVar31 = -3;
            local_32e0._0_2_ = 0x2120;
            local_32e0._7_1_ = 0;
            local_32e0._2_2_ = 0x2a00;
            local_32d8 = iVar9;
            uStack_32d4 = (uint)local_1be4;
            local_32d0 = (uint)local_1be1;
            uStack_32cc = (uint)local_1bde;
            FUN_001df8d4(&local_32e0,0x18);
            goto LAB_001e9038;
          }
          in_stack_ffffffffffffcb10 = plVar13;
          _ZN3mpr11stillrecord14_make_cnv_if_tEPKNS_18recordStillInParamENS_9StillTypeENS_12StillQualityEPN9DefStruct7JpgInfoEPNS6_7HifInfoEPNS_18recordStillParam_tEPhP8libstillPN5still8cnv_if_tEPNS0_13sidb_update_tEP10dmm_munmapPc
                    (param_1,iVar10,uVar8,local_3330[uVar30],local_3330[(ulong)uVar30 + 4],param_2,
                     pcVar28,plVar18,plVar13,(long)&local_3350 + uVar29 * 6,
                     *(undefined8 *)(puVar25 + uVar29 * 8),&local_32c8);
          bVar2 = *(byte *)(local_1fd0 + 0x13);
          bVar6 = _ZN3mpr16IsC2PARecordableENS_9StillTypeES0_bNS_11CaptureTypeE
                            (iVar10,*(undefined4 *)(param_2 + 0xd8),*(undefined1 *)(param_2 + 0xdc),
                             *(undefined4 *)(param_2 + 0xc));
          *(byte *)((long)plVar13 + 0x44e) = bVar6 & bVar2;
          memcpy(auStack_2518 + local_33c8,plVar13 + 5,0x2a0);
          if (iVar10 == 2) {
LAB_001e8fe0:
            uVar19 = uVar29 & 0xffffffff;
            local_17a8[uVar19 * 0x8a] = 0;
            local_17a8[uVar19 * 0x8a + 1] = 0;
            local_17a8[uVar19 * 0x8a + 2] = 0;
            local_17a8[uVar19 * 0x8a + 3] = 0;
            local_17a8[uVar19 * 0x8a + 4] = 0;
            local_17a8[uVar19 * 0x8a + 5] = 0;
            if (iVar10 == 1) {
LAB_001e99d8:
              if (*(char *)((long)plVar13 + 0x44e) != '\0') {
                FUN_003b2224(0x695,auStack_2f38,1,*(undefined8 *)(puVar25 + uVar29 * 8));
                FUN_001c4f1c(auStack_2f38,plVar13 + 0x47);
                FUN_003b2224(0x695,auStack_2e90,1,*(undefined8 *)(puVar25 + uVar29 * 8));
                FUN_001c4f1c(auStack_2e90,plVar13 + 0x59);
                if ((int)plVar13[0x3c] == 0) {
                  FUN_001c4f1c(auStack_3088,local_33b8);
                }
                FUN_001c4f1c(auStack_2fe0,plVar13 + 0x41);
                plVar20 = (long *)(auStack_2518 + local_33c8 + 0x210);
                lVar26 = plVar13[0x48];
                *plVar20 = plVar13[0x47];
                *(long *)((long)alStack_2308 + local_33c8 + 8) = lVar26;
                lVar26 = plVar13[0x4a];
                *(long *)((long)alStack_2308 + local_33c8 + 0x10) = plVar13[0x49];
                *(long *)((long)alStack_2308 + local_33c8 + 0x18) = lVar26;
                lVar26 = plVar13[0x4b];
                lVar21 = plVar13[0x4c];
LAB_001e90dc:
                plVar20[4] = lVar26;
                plVar20[5] = lVar21;
              }
            }
LAB_001e9010:
            iVar31 = 0;
            iVar10 = FUN_003715c0(local_3330[uVar29 + 6],plVar18,plVar13);
            if (local_2e84 != 0) {
              FUN_003b24f0();
            }
            goto LAB_001e9038;
          }
          if ((uint)(*(int *)(param_1 + 0x14) * *(int *)((long)param_1 + 0xa4)) < 0x1fa401) {
            uVar19 = uVar29 & 0xffffffff;
            auStack_1898[uVar19 * 0x8a] = 0;
            auStack_1898[uVar19 * 0x8a + 1] = 0;
            auStack_1898[uVar19 * 0x8a + 2] = 0;
            auStack_1898[uVar19 * 0x8a + 3] = 0;
            auStack_1898[uVar19 * 0x8a + 4] = 0;
            auStack_1898[uVar19 * 0x8a + 5] = 0;
            if ((iVar10 == 1) || (iVar10 == 4)) goto LAB_001e8fe0;
            uVar19 = uVar29 & 0xffffffff;
            local_17a8[uVar19 * 0x8a] = 0;
            local_17a8[uVar19 * 0x8a + 1] = 0;
            local_17a8[uVar19 * 0x8a + 2] = 0;
            local_17a8[uVar19 * 0x8a + 3] = 0;
            local_17a8[uVar19 * 0x8a + 4] = 0;
            local_17a8[uVar19 * 0x8a + 5] = 0;
            goto LAB_001e9010;
          }
          if (iVar10 == 1) {
            FUN_001c4f1c(auStack_3088,local_33b8);
            goto LAB_001e99d8;
          }
          if (iVar10 != 4) goto LAB_001e8fe0;
          lVar26 = FUN_003b2224(0x66d,auStack_3088,1,*(undefined8 *)(puVar25 + uVar29 * 8));
          if (lVar26 != 0) {
            FUN_001c4f1c(auStack_3088,local_33b8);
            memcpy((void *)plVar13[0x3d],(void *)(local_3288 + local_1c90),local_1c88);
            *(int *)(plVar13 + 0x3c) = (int)local_1c88;
            *(int *)(plVar13 + 0x30) = (int)plVar13[0x30] - (int)local_1c88;
            plVar20 = (long *)(auStack_2518 + local_33c8 + 0x1b0);
            lVar26 = local_33b8[1];
            *plVar20 = *local_33b8;
            *(long *)((long)alStack_2368 + local_33c8 + 8) = lVar26;
            lVar26 = local_33b8[3];
            *(long *)((long)alStack_2368 + local_33c8 + 0x10) = local_33b8[2];
            *(long *)((long)alStack_2368 + local_33c8 + 0x18) = lVar26;
            lVar26 = local_33b8[4];
            lVar21 = local_33b8[5];
            goto LAB_001e90dc;
          }
          local_32e0._5_1_ = 0;
          local_32e0._6_1_ = 0;
          uVar24 = 0xc;
          local_32e0._4_1_ = 3;
          local_32e0._7_1_ = 0;
          local_32e0._0_2_ = 0x2120;
          local_32d8 = 0;
          local_32e0._2_2_ = 0x2a0d;
          puVar16 = &local_32e0;
        }
        FUN_001df8d4(puVar16,uVar24);
        iVar7 = -1;
LAB_001e8928:
        lVar26 = 0;
        _ZN3mpr11stillrecord5_freeERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tEb
                  (param_1,param_2,local_3330,local_3330 + 2,local_3330 + 4,auStack_2518,1);
        do {
          plVar13 = *(long **)((long)local_3330 + lVar26 + 0x30);
          if (plVar13 != (long *)0x0) {
            (**(code **)(*plVar13 + 8))();
          }
          if (*(long *)(puVar25 + lVar26) != 0) {
            _ZdlPvm(*(long *)(puVar25 + lVar26),4);
          }
          lVar26 = lVar26 + 8;
        } while (lVar26 != 0x10);
        puVar27 = (undefined4 *)(param_2 + 0x130);
        _ZN3mpr30evtStillTransferDataBuiltParamC2Ev(&local_1fd8);
        local_1f18[0x74] = '\x02';
        local_1f18[0x75] = '\0';
        local_1f18[0x76] = '\0';
        local_1f18[0x77] = '\0';
        local_1fd8 = (undefined1 *)
                     CONCAT44(*(undefined4 *)((long)param_1 + 4),(undefined4)local_1fd8);
        _ZN3mpr13SetIssueEventENS_5EVTIDEPvm(0x102c,&local_1fd8,0x138);
        do {
          if (((*(int *)(param_2 + 0x20c) != -1) && (*local_3398 != '\0')) ||
             ((*(int *)(param_2 + 0x794) != -1 && (local_3398[0x588] != '\0')))) {
            local_1fd0 = (undefined2 *)0x0;
            local_1fc8 = 0;
            local_1fc0[0] = 0;
            local_1fc0[1] = 0;
            uStack_1fb0 = 0;
            local_1fa8 = 0;
            uStack_1fa0 = 0;
            local_1f98 = 0;
            uStack_1f90 = 0;
            local_1f88 = 0;
            local_1fd8 = (undefined1 *)0x100c;
            _ZN4sidb6DeleteEjjbb(0xffffffff,*puVar27,1,0);
            local_1f88 = CONCAT44(2,(undefined4)local_1f88);
            local_1fd8 = (undefined1 *)
                         CONCAT44(*(undefined4 *)((long)param_1 + 4),(undefined4)local_1fd8);
            _ZN3mpr13SetIssueEventENS_5EVTIDEPvm(0x100c,&local_1fd8,0x58);
          }
          puVar27 = puVar27 + 1;
          local_3398 = local_3398 + 0x106;
        } while ((char *)(param_2 + 0x654) != local_3398);
        local_1fd8 = (undefined1 *)0x42c102120;
        FUN_001df8d4(&local_1fd8,8);
        return iVar7;
      }
      in_stack_ffffffffffffcb10 = (long *)((ulong)in_stack_ffffffffffffcb10 & 0xffffffffffffff00);
      _ZN3mpr11stillrecord14_make_add_if_tEPKNS_18recordStillInParamENS_9StillTypeEPN9DefStruct7JpgInfoEPNS5_7RawInfoEPNS5_7HifInfoEPNS_18recordStillParam_tEPhbbPN5still8add_if_tEPNS0_13sidb_update_tEP10dmm_munmapPc
                (param_1,iVar10,local_3330[uVar30],local_3330[(ulong)uVar30 + 2],
                 local_3330[(ulong)uVar30 + 4],param_2,pcVar28,0,in_stack_ffffffffffffcb10,
                 local_33a0,(long)&local_3350 + uVar29 * 6,*(undefined8 *)(puVar25 + uVar29 * 8),
                 &local_32c8);
      pvVar22 = (void *)_ZN3mpr11GetFilePathEPNS_18recordStillParam_tEbj
                                  (param_2,0,uVar29 & 0xffffffff);
      if (pvVar22 != (void *)0x0) {
        memcpy((void *)((long)local_33a0 + 0x4df),pvVar22,0x106);
      }
      *(undefined1 *)((long)local_33a0 + 0x5e4) = 0;
      memcpy(auStack_2518 + local_33c8,local_33a0 + 0x1d,0x2a0);
      bVar4 = true;
      if ((iVar10 != 2) &&
         ((uint)(*(int *)(*local_33a0 + 0x5c) * *(int *)(*local_33a0 + 0x60)) < 0x1fa401)) {
        uVar19 = uVar29 & 0xffffffff;
        bVar4 = false;
        auStack_f38[uVar19 * 0x10e] = 0;
        auStack_f38[uVar19 * 0x10e + 1] = 0;
        auStack_f38[uVar19 * 0x10e + 2] = 0;
        auStack_f38[uVar19 * 0x10e + 3] = 0;
        auStack_f38[uVar19 * 0x10e + 4] = 0;
        auStack_f38[uVar19 * 0x10e + 5] = 0;
      }
      *(undefined1 *)((long)local_3388 + uVar29) = *(undefined1 *)((long)local_33a0 + 0x869);
      *(char *)((long)local_3378 + uVar29) = (char)local_33a0[0x10d];
      *(undefined1 *)((long)local_3368 + uVar29) = *(undefined1 *)((long)local_33a0 + 0x86b);
      FUN_00370b2c(local_3330[uVar29 + 6],local_33a0);
      if (((bVar4) && (*(char *)(local_33a8 + 0x674) != '\0')) &&
         (*(char *)(local_33a8 + 0x675) != '\0')) {
        in_stack_ffffffffffffcb10 = (long *)((long)&local_3358 + uVar29 * 4);
        _ZN3mpr11stillrecord24_add_if_for_scn_transferEPKNS_18recordStillInParamENS_9StillTypeEPN9DefStruct7JpgInfoEPNS5_7RawInfoEPNS5_7HifInfoEPNS_18recordStillParam_tEPhPP8libstillPS4_PbSJ_SJ_P10dmm_munmapPc
                  (param_1,iVar10,local_3330[uVar30],local_3330[(ulong)uVar30 + 2],
                   local_3330[(ulong)uVar30 + 4],param_2,pcVar28,local_3330 + uVar29 + 8,
                   in_stack_ffffffffffffcb10,(long)local_3380 + uVar29,(long)local_3370 + uVar29,
                   (long)local_3360 + uVar29,*(undefined8 *)(puVar25 + uVar29 * 8),&local_32c8);
      }
    }
LAB_001e937c:
    uVar29 = uVar29 + 1;
    plVar13 = plVar13 + 0x8a;
    local_33a8 = local_33a8 + 0x588;
    local_33a0 = local_33a0 + 0x10e;
    local_33b8 = local_33b8 + 0x8a;
    local_33c8 = local_33c8 + 0x2a0;
  } while (uVar29 != 2);
  plVar13 = local_3330 + 2;
  plVar18 = local_3330 + 4;
  if (*(int *)(param_2 + 0xc) != 3) {
    _ZN3mpr11stillrecord7_shrareERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tEbPP10dmm_munmap
              (param_1,param_2,local_3330,plVar13,plVar18,auStack_2518,bVar5,puVar25);
    _ZN3mpr11stillrecord5_freeERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tEb
              (param_1,param_2,local_3330,plVar13,plVar18,auStack_2518,bVar5);
  }
  puVar27 = (undefined4 *)(param_2 + 0x130);
  _ZN3mpr11stillrecord6_cacheERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tERA2_P8libstillbPP10dmm_munmap
            (param_1,param_2,local_3330,plVar13,plVar18,auStack_2518,local_3330 + 6,bVar5,puVar25);
  piVar14 = &DAT_00474d2c;
  do {
    uVar30 = *(uint *)(param_2 + 0x20c);
    if (((uVar30 != 0xffffffff) && (*local_3398 != '\0')) ||
       ((uVar30 = *(uint *)(param_2 + 0x794), uVar30 != 0xffffffff && (local_3398[0x588] != '\0'))))
    {
      if (iVar1 == 1) {
        lVar21 = (ulong)uVar30 * 0x450 + 0xc;
        lVar26 = (long)&lStack_1980 + lVar21;
        plVar13 = &lStack_1980;
LAB_001e94c0:
        local_342c = *(undefined2 *)((long)plVar13 + lVar21);
        local_3430 = *(undefined2 *)(lVar26 + 2);
        local_3444 = *(undefined1 *)(lVar26 + 4);
        local_3434 = *(undefined1 *)(lVar26 + 5);
        local_3438 = *(undefined1 *)(lVar26 + 6);
        local_343c = *(undefined1 *)(lVar26 + 7);
        local_3440 = *(undefined1 *)(lVar26 + 8);
        local_33f8 = *(undefined2 *)(lVar26 + 10);
      }
      else if (*(int *)(param_2 + 0xc) == 2) {
        lVar21 = (ulong)uVar30 * 0x870 + 0x3c4;
        lVar26 = (long)&local_10e0 + lVar21;
        plVar13 = (long *)&local_10e0;
        goto LAB_001e94c0;
      }
      uVar8 = *puVar27;
      iVar10 = *piVar14;
      _ZN4sidb10recordInfo5clearEv(&local_1fd8);
      iVar7 = _ZN4sidb6SelectEjjPNS_10recordInfoEb(0xffffffff,uVar8,&local_1fd8,1);
      uVar29 = local_1fc8;
      if (iVar7 == 0) {
        if ((iVar1 == 1) || (*(int *)(param_2 + 0xc) == 2)) {
          local_1fd0 = (undefined2 *)
                       CONCAT26(local_3430,CONCAT24(local_342c,(undefined4)local_1fd0));
          local_1fc8._5_3_ = SUB83(uVar29,5);
          local_1fc8._0_5_ =
               CONCAT14(local_3440,
                        CONCAT13(local_343c,CONCAT12(local_3438,CONCAT11(local_3434,local_3444))));
          local_1fc8 = CONCAT26(local_33f8,(undefined6)local_1fc8);
        }
        puVar16 = &local_3350;
        piVar33 = (int *)(param_2 + 500);
        pcVar28 = local_3398;
        do {
          if ((piVar33[6] != -1) && (*pcVar28 != '\0')) {
            iVar7 = *piVar33;
            if (iVar1 == 1) {
              if (iVar7 == 1) {
                if (local_1f18[0] != '\0') {
                  strncpy(local_1f18 + 1,pcVar28,0x105);
                }
LAB_001e97cc:
                local_1af0 = 0;
                local_1afc = *(undefined2 *)puVar16;
                local_1afa = *(undefined2 *)((long)puVar16 + 2);
                local_1aec = *(undefined2 *)((long)puVar16 + 4);
              }
              else {
                if (iVar7 != 4) goto LAB_001e994c;
                if (local_1f18[0x20e] != '\0') {
                  strncpy(acStack_1d09,pcVar28,0x105);
                }
LAB_001e991c:
                local_1ae0 = 0;
                local_1af4 = *(undefined2 *)puVar16;
                local_1af2 = *(undefined2 *)((long)puVar16 + 2);
                local_1adc = *(undefined2 *)((long)puVar16 + 4);
              }
            }
            else {
              if (iVar7 == 1) goto LAB_001e97cc;
              if (iVar7 == 4) goto LAB_001e991c;
LAB_001e994c:
              if (iVar7 == 2) {
                local_1ae8 = 0;
                local_1af8 = *(undefined2 *)puVar16;
                local_1af6 = *(undefined2 *)((long)puVar16 + 2);
                local_1ae4 = *(undefined2 *)((long)puVar16 + 4);
              }
            }
          }
          puVar16 = (undefined8 *)((long)puVar16 + 6);
          piVar33 = piVar33 + 0x162;
          pcVar28 = pcVar28 + 0x588;
        } while ((undefined8 *)auStack_3344 != puVar16);
        local_19bc = local_19bc & 0xfd | 0x10;
        _ZN4sidb6UpdateEjRKNS_10recordInfoEbb(0xffffffff,&local_1fd8,1,0);
        if (*(int *)(param_2 + 0xc) == 0 || *(int *)(param_2 + 0xc) == 3) {
          uVar29 = 0;
          if (0x76c < local_1fd0._6_2_) {
            uVar29 = ((ulong)((int)local_1fd0._6_2_ - 0x76c) & 0xff) << 0x24;
          }
          _ZN4sidb14SetHeaderParamImEEijNS_9HEADER_IDET_
                    (iVar10,3,(local_1fc8 >> 0x10 & 0x1f) << 0x16 |
                              (local_1fc8 >> 0x18 & 0x3f) << 0x10 |
                              (local_1fc8 & 0xf) << 0x20 | (local_1fc8 >> 8 & 0x1f) << 0x1b |
                              (local_1fc8 >> 0x20 & 0x3f) << 10 | local_1fc8 >> 0x30 & 0x3ff |
                              ((ulong)((int)local_1fd0._4_2_ / 0x1e + 0x60) & 0xff) << 0x2c | uVar29
                    );
        }
      }
      else {
        local_2804 = 1;
        local_27f4 = local_1fd8._4_4_;
        local_2808 = 0x2120;
        local_2803 = 0;
        local_2806 = 0x2b10;
        local_2802 = 0;
        local_2801 = 0;
        local_2800 = iVar10;
        local_27fc = uVar8;
        local_27f8 = iVar7;
        FUN_001df8d4(&local_2808,0x18);
      }
    }
    piVar14 = piVar14 + 0x20;
    puVar27 = puVar27 + 1;
    local_3398 = local_3398 + 0x106;
    if ((char *)(param_2 + 0x654) == local_3398) {
      local_1988 = _ZN3mpr11stillrecord12_make_headerEPv;
      local_1fa8._0_5_ = CONCAT14(bVar5,*(undefined4 *)((long)param_1 + 4));
      lVar26 = 0;
      puVar25 = (undefined1 *)((long)&local_1fa8 + 5);
      plVar13 = &local_1fd0;
      puVar16 = &uStack_1fb0;
      do {
        *plVar13 = (local_3330 + 6)[lVar26];
        plVar13[2] = local_3330[lVar26 + 8];
        *(undefined4 *)puVar16 = *(undefined4 *)((long)&local_3358 + lVar26 * 4);
        *puVar25 = *(undefined1 *)((long)local_3388 + lVar26);
        puVar25[2] = *(undefined1 *)((long)local_3380 + lVar26);
        puVar25[4] = *(undefined1 *)((long)local_3378 + lVar26);
        puVar25[6] = *(undefined1 *)((long)local_3370 + lVar26);
        puVar25[8] = *(undefined1 *)((long)local_3368 + lVar26);
        puVar17 = (undefined1 *)((long)local_3360 + lVar26);
        lVar26 = lVar26 + 1;
        puVar25[10] = *puVar17;
        puVar25 = puVar25 + 1;
        plVar13 = plVar13 + 1;
        puVar16 = (undefined8 *)((long)puVar16 + 4);
      } while (lVar26 != 2);
      local_1fd8 = (undefined1 *)param_2;
      _ZN9execqueueIN3mpr11stillrecord19make_header_input_tEE4execERNS3_6exec_tE
                (_ZN3mpr11stillrecord18make_header_threadE);
      local_2803 = 0;
      local_2802 = 0;
      local_2804 = 4;
      local_2801 = 0;
      local_2808 = 0x2120;
      local_2806 = 0x2c00;
      FUN_001df8d4(&local_2808,8);
      return 0;
    }
  } while( true );
}


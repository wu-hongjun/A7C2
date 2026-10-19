// Decompiled pseudocode, not original source.
// REA Evidence: ev_dabffebe56aadd78dc8e40964ad1717edf4df4da101d3905e238e5d4cc12de3f
// Source record: research/evidence/rea/sony-mpr-events/00-procedure_pseudo_code.json

void _ZN3mpr11stillrecord27ArbitrateProcessingSequenceEv(void)

{
  int iVar1;
  undefined4 uVar2;
  long lVar3;
  int *piVar4;
  uint *puVar5;
  undefined4 *puVar6;
  long lVar7;
  long *plVar8;
  long *plVar9;
  long *plVar10;
  undefined8 uVar11;
  ulong uVar12;
  ulong uVar13;
  long *plVar14;
  long lVar15;
  long lVar16;
  long lVar17;
  long extraout_x8;
  long extraout_x8_00;
  long extraout_x8_01;
  long extraout_x8_02;
  ulong extraout_x9;
  ulong extraout_x9_00;
  ulong extraout_x9_01;
  ulong extraout_x9_02;
  long *extraout_x10;
  long *extraout_x10_00;
  long *extraout_x10_01;
  long *extraout_x10_02;
  uint local_4c;
  void *local_48;
  ulong *local_40 [4];
  undefined2 local_20;
  undefined2 uStack_1e;
  undefined1 uStack_1c;
  undefined1 uStack_1b;
  undefined1 uStack_1a;
  undefined1 uStack_19;
  undefined4 local_18;
  uint uStack_14;
  undefined4 local_10;
  undefined4 uStack_c;
  undefined8 local_8;

  local_4c = 0;
  _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(&local_20,0x481540);
  _ZN3mpr32g_orderRegistrationConstIteratorE =
       (ulong *)CONCAT17(uStack_19,
                         CONCAT16(uStack_1a,
                                  CONCAT15(uStack_1b,
                                           CONCAT14(uStack_1c,CONCAT22(uStack_1e,local_20)))));
  DAT_00481750 = CONCAT44(uStack_14,local_18);
  DAT_00481758 = CONCAT44(uStack_c,local_10);
  DAT_00481760 = local_8;
  do {
    while( true ) {
      _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(&local_20,0x481560);
      if ((ulong *)CONCAT17(uStack_19,
                            CONCAT16(uStack_1a,
                                     CONCAT15(uStack_1b,
                                              CONCAT14(uStack_1c,CONCAT22(uStack_1e,local_20))))) ==
          _ZN3mpr32g_orderRegistrationConstIteratorE) {
        return;
      }
      local_48 = (void *)*_ZN3mpr32g_orderRegistrationConstIteratorE;
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr18g_finThreadStartedE,&local_48);
      iVar1 = *(int *)(lVar3 + 4);
      piVar4 = (int *)_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_jESaIS6_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                                (&_ZN3mpr18g_orderThreadCountE,&local_48);
      if (iVar1 != *piVar4) {
        return;
      }
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr19g_finThreadFinishedE,&local_48);
      iVar1 = *(int *)(lVar3 + 4);
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr18g_finThreadStartedE,&local_48);
      if (iVar1 != *(int *)(lVar3 + 4)) {
        return;
      }
      _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(local_40,0x481778);
      _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(&local_20,0x481798);
      if ((((ulong *)CONCAT17(uStack_19,
                              CONCAT16(uStack_1a,
                                       CONCAT15(uStack_1b,
                                                CONCAT14(uStack_1c,CONCAT22(uStack_1e,local_20)))))
            != local_40[0]) && ((void *)*local_40[0] == local_48)) &&
         (lVar3 = FUN_001df868(), lVar3 != 0)) {
        uStack_1b = 0;
        uStack_1c = 1;
        uStack_1a = 0;
        local_20 = 0x2120;
        uStack_19 = 0;
        uStack_1e = 0x8080;
        local_18 = osal_thread_id();
        puVar5 = (uint *)_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_19trasnferDataBuilt_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                                   (&_ZN3mpr27g_evtStillTransferDataBuiltE,&local_48);
        uStack_14 = *puVar5;
        lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_19trasnferDataBuilt_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                          (&_ZN3mpr27g_evtStillTransferDataBuiltE,&local_48);
        local_10 = (undefined4)*(undefined8 *)(lVar3 + 0x140);
        uStack_c = SUB84(local_48,0);
        FUN_001df8d4(&local_20,0x18);
        puVar6 = (undefined4 *)
                 _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_19trasnferDataBuilt_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                           (&_ZN3mpr27g_evtStillTransferDataBuiltE,&local_48);
        uVar2 = *puVar6;
        lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_19trasnferDataBuilt_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                          (&_ZN3mpr27g_evtStillTransferDataBuiltE,&local_48);
        lVar7 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_19trasnferDataBuilt_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                          (&_ZN3mpr27g_evtStillTransferDataBuiltE,&local_48);
        _ZN3mpr13SetIssueEventENS_5EVTIDEPvm(uVar2,lVar3 + 4,*(undefined8 *)(lVar7 + 0x140));
        _ZNSt13unordered_mapIPN3mpr18recordStillParam_tENS0_19trasnferDataBuilt_tESt4hashIS2_ESt8equal_toIS2_ESaISt4pairIKS2_S3_EEE5eraseERS9_
                  (&_ZN3mpr27g_evtStillTransferDataBuiltE,&local_48);
        _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE9pop_frontEv
                  (_ZN3mpr35g_orderRegistrationEvtStillTransferE);
      }
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9execqueueINS1_11stillrecord33writeAndTransferSequencer_input_tEE6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjSB_EEEESaISK_ENS_10_Select1stESE_IS3_ESC_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr19g_writerThreadParamE,&local_48);
      plVar10 = *(long **)(lVar3 + 0x10);
      while (_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9execqueueINS1_11stillrecord33writeAndTransferSequencer_input_tEE6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjSB_EEEESaISK_ENS_10_Select1stESE_IS3_ESC_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                       (&_ZN3mpr19g_writerThreadParamE,&local_48), plVar10 != (long *)0x0) {
        uStack_1b = 0;
        uStack_1a = 0;
        uStack_1c = 2;
        uStack_19 = 0;
        local_20 = 0x2120;
        uStack_1e = 33000;
        local_18 = osal_thread_id();
        uStack_14 = local_4c;
        FUN_001df8d4(&local_20,0x10);
        _ZN9execqueueIN3mpr11stillrecord33writeAndTransferSequencer_input_tEE4execERNS3_6exec_tE
                  (_ZN3mpr11stillrecord32writeAndTransferSequencer_threadE,plVar10 + 2);
        lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                          (&_ZN3mpr21g_writerThreadStartedE,&local_48);
        *(int *)(lVar3 + 4) = *(int *)(lVar3 + 4) + 1;
        plVar8 = (long *)_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9execqueueINS1_11stillrecord33writeAndTransferSequencer_input_tEE6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjSB_EEEESaISK_ENS_10_Select1stESE_IS3_ESC_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                                   (&_ZN3mpr19g_writerThreadParamE,&local_48);
        lVar3 = *plVar8;
        uVar13 = plVar8[1];
        uVar12 = 0;
        if (uVar13 != 0) {
          uVar12 = *(uint *)(plVar10 + 1) / uVar13;
        }
        lVar7 = (ulong)*(uint *)(plVar10 + 1) - uVar12 * uVar13;
        lVar17 = lVar7 * 8;
        plVar14 = *(long **)(lVar3 + lVar17);
        do {
          plVar9 = plVar14;
          plVar14 = (long *)*plVar9;
        } while (plVar10 != (long *)*plVar9);
        lVar15 = *plVar10;
        if (*(long **)(lVar3 + lVar17) == plVar9) {
          lVar16 = 0;
          if (lVar15 != 0) {
            uVar12 = 0;
            if (uVar13 != 0) {
              uVar12 = *(uint *)(lVar15 + 8) / uVar13;
            }
            lVar16 = (ulong)*(uint *)(lVar15 + 8) - uVar12 * uVar13;
          }
          if (lVar15 == 0 || lVar7 != lVar16) {
            if (lVar15 != 0) {
              *(long **)(lVar3 + lVar16 * 8) = plVar9;
            }
            if (*(long **)(lVar3 + lVar17) == plVar8 + 2) {
              plVar8[2] = lVar15;
            }
            *(undefined8 *)(lVar3 + lVar17) = 0;
          }
        }
        else if (lVar15 != 0) {
          uVar12 = 0;
          if (uVar13 != 0) {
            uVar12 = *(uint *)(lVar15 + 8) / uVar13;
          }
          lVar17 = (ulong)*(uint *)(lVar15 + 8) - uVar12 * uVar13;
          if (lVar7 != lVar17) {
            *(long **)(lVar3 + lVar17 * 8) = plVar9;
          }
        }
        *plVar9 = *plVar10;
        plVar14 = (long *)*plVar10;
        _ZdlPv(plVar10);
        plVar8[3] = plVar8[3] + -1;
        plVar10 = plVar14;
      }
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr22g_writerThreadFinishedE,&local_48);
      iVar1 = *(int *)(lVar3 + 4);
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr21g_writerThreadStartedE,&local_48);
      if (iVar1 == *(int *)(lVar3 + 4)) break;
      uStack_1b = 0;
      uStack_1a = 0;
      uStack_1c = 1;
      uStack_19 = 0;
      local_20 = 0x2120;
      uStack_1e = 0x80ec;
      local_18 = osal_thread_id();
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr22g_writerThreadFinishedE,&local_48);
      uStack_14 = *(uint *)(lVar3 + 4);
      lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_NS1_18writeReqFinCount_tEESaIS7_ENS_10_Select1stESt8equal_toIS3_ESt4hashIS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                        (&_ZN3mpr21g_writerThreadStartedE,&local_48);
      local_10 = *(undefined4 *)(lVar3 + 4);
      uStack_c = 0;
      FUN_001df8d4(&local_20,0x18);
LAB_001efc14:
      _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERKS2_PS3_EppEv
                (&_ZN3mpr32g_orderRegistrationConstIteratorE);
    }
    for (local_4c = 0; local_4c < 2; local_4c = local_4c + 1) {
      if (*(int *)((long)local_48 + (ulong)local_4c * 0x588 + 0x20c) != -1) {
        plVar10 = (long *)_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                                    (&_ZN3mpr23g_fsysWriteRequestCountE,&local_48);
        uVar12 = plVar10[1];
        uVar13 = 0;
        if (uVar12 != 0) {
          uVar13 = local_4c / uVar12;
        }
        lVar3 = (ulong)local_4c - uVar13 * uVar12;
        plVar10 = *(long **)(*plVar10 + lVar3 * 8);
        if ((plVar10 != (long *)0x0) && (plVar10 = (long *)*plVar10, plVar10 != (long *)0x0)) {
          lVar7 = 0;
          do {
            if (local_4c == *(uint *)(plVar10 + 1)) {
              lVar7 = lVar7 + 1;
            }
            else if (lVar7 != 0) goto LAB_001efcc8;
            plVar10 = (long *)*plVar10;
            if (plVar10 == (long *)0x0) break;
            uVar13 = 0;
            if (uVar12 != 0) {
              uVar13 = *(uint *)(plVar10 + 1) / uVar12;
            }
          } while (lVar3 == (ulong)*(uint *)(plVar10 + 1) - uVar13 * uVar12);
          if (lVar7 != 0) {
LAB_001efcc8:
            uVar11 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                               (&_ZN3mpr24g_fsysWriteFinishedCountE,&local_48);
            lVar3 = _ZNSt8__detail9_Map_baseIjSt4pairIKjN3mpr18writeReqFinCount_tEESaIS5_ENS_10_Select1stESt8equal_toIjESt4hashIjENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS2_
                              (uVar11,&local_4c);
            iVar1 = *(int *)(lVar3 + 4);
            uVar11 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                               (&_ZN3mpr23g_fsysWriteRequestCountE,&local_48);
            lVar3 = _ZNSt8__detail9_Map_baseIjSt4pairIKjN3mpr18writeReqFinCount_tEESaIS5_ENS_10_Select1stESt8equal_toIjESt4hashIjENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS2_
                              (uVar11,&local_4c);
            if (iVar1 != *(int *)(lVar3 + 4)) goto LAB_001efc14;
          }
        }
      }
    }
    _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(&local_20,0x481540);
    if (_ZN3mpr32g_orderRegistrationConstIteratorE !=
        (ulong *)CONCAT17(uStack_19,
                          CONCAT16(uStack_1a,
                                   CONCAT15(uStack_1b,
                                            CONCAT14(uStack_1c,CONCAT22(uStack_1e,local_20)))))) {
      return;
    }
    lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_16writeCountdown_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                      (&_ZN3mpr24g_evtStillWriteCountdownE,&local_48);
    plVar10 = *(long **)(lVar3 + 0x10);
    local_4c = 0;
    for (; (uVar13 = (ulong)local_4c,
           lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_16writeCountdown_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                             (&_ZN3mpr24g_evtStillWriteCountdownE,&local_48),
           uVar13 < *(ulong *)(lVar3 + 0x18) &&
           (_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_16writeCountdown_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                      (&_ZN3mpr24g_evtStillWriteCountdownE,&local_48), plVar10 != (long *)0x0));
        plVar10 = (long *)*plVar10) {
      uStack_1b = 0;
      uStack_1a = 0;
      uStack_1c = 1;
      uStack_19 = 0;
      local_20 = 0x2120;
      uStack_1e = 0x8088;
      local_18 = osal_thread_id();
      uStack_14 = local_4c;
      local_10 = *(undefined4 *)((long)plVar10 + 0x1c);
      uStack_c = 0;
      FUN_001df8d4(&local_20,0x18);
      _ZN3mpr13SetIssueEventENS_5EVTIDEPvm
                (*(undefined4 *)(plVar10 + 2),(long)plVar10 + 0x14,plVar10[0xe]);
      local_4c = local_4c + 1;
    }
    lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                      (&_ZN3mpr9g_writeDbE,&local_48);
    plVar10 = *(long **)(lVar3 + 0x10);
    local_4c = 0;
    for (; (uVar13 = (ulong)local_4c,
           lVar3 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                             (&_ZN3mpr9g_writeDbE,&local_48), uVar13 < *(ulong *)(lVar3 + 0x18) &&
           (_ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                      (&_ZN3mpr9g_writeDbE,&local_48), plVar10 != (long *)0x0));
        plVar10 = (long *)*plVar10) {
      FUN_00194278(plVar10 + 2);
      local_4c = local_4c + 1;
    }
    _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE9pop_frontEv(_ZN3mpr19g_orderRegistrationE);
    uVar13 = 0;
    if (DAT_004814c8 != 0) {
      uVar13 = (ulong)local_48 / DAT_004814c8;
    }
    plVar10 = (long *)FUN_001df548(&_ZN3mpr18g_orderThreadCountE,
                                   (long)local_48 - uVar13 * DAT_004814c8,&local_48);
    if (plVar10 != (long *)0x0) {
      lVar3 = *extraout_x10;
      lVar17 = extraout_x8 * 8;
      plVar8 = (long *)*plVar10;
      lVar7 = *plVar8;
      if (plVar10 == *(long **)(lVar3 + lVar17)) {
        lVar15 = 0;
        if (lVar7 != 0) {
          uVar13 = 0;
          if (extraout_x9 != 0) {
            uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9;
          }
          lVar15 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9;
        }
        if (lVar7 == 0 || extraout_x8 != lVar15) {
          if (lVar7 != 0) {
            *(long **)(lVar3 + lVar15 * 8) = plVar10;
          }
          if (*(undefined8 **)(lVar3 + lVar17) == &DAT_004814d0) {
            DAT_004814d0 = lVar7;
          }
          *(undefined8 *)(lVar3 + lVar17) = 0;
        }
      }
      else if (lVar7 != 0) {
        uVar13 = 0;
        if (extraout_x9 != 0) {
          uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9;
        }
        lVar7 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9;
        if (extraout_x8 != lVar7) {
          *(long **)(lVar3 + lVar7 * 8) = plVar10;
        }
      }
      *plVar10 = *plVar8;
      _ZdlPv(plVar8);
      DAT_004814d8 = DAT_004814d8 + -1;
    }
    FUN_001df638(&_ZN3mpr18g_finThreadStartedE,&local_48);
    FUN_001df638(&_ZN3mpr19g_finThreadFinishedE,&local_48);
    uVar13 = 0;
    if (DAT_00481588 != 0) {
      uVar13 = (ulong)local_48 / DAT_00481588;
    }
    plVar10 = (long *)FUN_001df718(&_ZN3mpr19g_writerThreadParamE,
                                   (long)local_48 - uVar13 * DAT_00481588,&local_48);
    if (plVar10 != (long *)0x0) {
      lVar3 = *extraout_x10_00;
      lVar17 = extraout_x8_00 * 8;
      plVar8 = (long *)*plVar10;
      lVar7 = *plVar8;
      if (plVar10 == *(long **)(lVar3 + lVar17)) {
        lVar15 = 0;
        if (lVar7 != 0) {
          uVar13 = 0;
          if (extraout_x9_00 != 0) {
            uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9_00;
          }
          lVar15 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9_00;
        }
        if (lVar7 == 0 || extraout_x8_00 != lVar15) {
          if (lVar7 != 0) {
            *(long **)(lVar3 + lVar15 * 8) = plVar10;
          }
          if (*(undefined8 **)(lVar3 + lVar17) == &DAT_00481590) {
            DAT_00481590 = lVar7;
          }
          *(undefined8 *)(lVar3 + lVar17) = 0;
        }
      }
      else if (lVar7 != 0) {
        uVar13 = 0;
        if (extraout_x9_00 != 0) {
          uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9_00;
        }
        lVar7 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9_00;
        if (extraout_x8_00 != lVar7) {
          *(long **)(lVar3 + lVar7 * 8) = plVar10;
        }
      }
      *plVar10 = *plVar8;
      _ZNSt10_HashtableIjSt4pairIKjN9execqueueIN3mpr11stillrecord33writeAndTransferSequencer_input_tEE6exec_tEESaIS8_ENSt8__detail10_Select1stESt8equal_toIjESt4hashIjENSA_18_Mod_range_hashingENSA_20_Default_ranged_hashENSA_20_Prime_rehash_policyENSA_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
                (plVar8 + 2);
      _ZNSt10_HashtableIjSt4pairIKjN9execqueueIN3mpr11stillrecord33writeAndTransferSequencer_input_tEE6exec_tEESaIS8_ENSt8__detail10_Select1stESt8equal_toIjESt4hashIjENSA_18_Mod_range_hashingENSA_20_Default_ranged_hashENSA_20_Prime_rehash_policyENSA_17_Hashtable_traitsILb0ELb0ELb1EEEE21_M_deallocate_bucketsEv
                (plVar8 + 2);
      _ZdlPv(plVar8);
      DAT_00481598 = DAT_00481598 + -1;
    }
    FUN_001df638(&_ZN3mpr21g_writerThreadStartedE,&local_48);
    FUN_001df638(&_ZN3mpr22g_writerThreadFinishedE,&local_48);
    FUN_001ef508(&_ZN3mpr23g_fsysWriteRequestCountE,&local_48);
    FUN_001ef508(&_ZN3mpr24g_fsysWriteFinishedCountE,&local_48);
    uVar13 = 0;
    if (DAT_00481668 != 0) {
      uVar13 = (ulong)local_48 / DAT_00481668;
    }
    plVar10 = (long *)FUN_001df7c0(&_ZN3mpr24g_evtStillWriteCountdownE,
                                   (long)local_48 - uVar13 * DAT_00481668,&local_48);
    if (plVar10 != (long *)0x0) {
      lVar3 = *extraout_x10_01;
      lVar17 = extraout_x8_01 * 8;
      plVar8 = (long *)*plVar10;
      lVar7 = *plVar8;
      if (plVar10 == *(long **)(lVar3 + lVar17)) {
        lVar15 = 0;
        if (lVar7 != 0) {
          uVar13 = 0;
          if (extraout_x9_01 != 0) {
            uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9_01;
          }
          lVar15 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9_01;
        }
        if (lVar7 == 0 || extraout_x8_01 != lVar15) {
          if (lVar7 != 0) {
            *(long **)(lVar3 + lVar15 * 8) = plVar10;
          }
          if (*(undefined8 **)(lVar3 + lVar17) == &DAT_00481670) {
            DAT_00481670 = lVar7;
          }
          *(undefined8 *)(lVar3 + lVar17) = 0;
        }
      }
      else if (lVar7 != 0) {
        uVar13 = 0;
        if (extraout_x9_01 != 0) {
          uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9_01;
        }
        lVar7 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9_01;
        if (extraout_x8_01 != lVar7) {
          *(long **)(lVar3 + lVar7 * 8) = plVar10;
        }
      }
      *plVar10 = *plVar8;
      _ZNSt10_HashtableIjSt4pairIKjN3mpr16writeCountdown_tEESaIS4_ENSt8__detail10_Select1stESt8equal_toIjESt4hashIjENS6_18_Mod_range_hashingENS6_20_Default_ranged_hashENS6_20_Prime_rehash_policyENS6_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
                (plVar8 + 2);
      _ZNSt10_HashtableIjSt4pairIKjN3mpr16writeCountdown_tEESaIS4_ENSt8__detail10_Select1stESt8equal_toIjESt4hashIjENS6_18_Mod_range_hashingENS6_20_Default_ranged_hashENS6_20_Prime_rehash_policyENS6_17_Hashtable_traitsILb0ELb0ELb1EEEE21_M_deallocate_bucketsEv
                (plVar8 + 2);
      _ZdlPv(plVar8);
      DAT_00481678 = DAT_00481678 + -1;
    }
    uVar13 = 0;
    if (DAT_00481818 != 0) {
      uVar13 = (ulong)local_48 / DAT_00481818;
    }
    plVar10 = (long *)FUN_001df814(&_ZN3mpr9g_writeDbE,(long)local_48 - uVar13 * DAT_00481818,
                                   &local_48);
    if (plVar10 != (long *)0x0) {
      lVar3 = *extraout_x10_02;
      lVar17 = extraout_x8_02 * 8;
      plVar8 = (long *)*plVar10;
      lVar7 = *plVar8;
      if (plVar10 == *(long **)(lVar3 + lVar17)) {
        lVar15 = 0;
        if (lVar7 != 0) {
          uVar13 = 0;
          if (extraout_x9_02 != 0) {
            uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9_02;
          }
          lVar15 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9_02;
        }
        if (lVar7 == 0 || extraout_x8_02 != lVar15) {
          if (lVar7 != 0) {
            *(long **)(lVar3 + lVar15 * 8) = plVar10;
          }
          if (*(undefined8 **)(lVar3 + lVar17) == &DAT_00481820) {
            DAT_00481820 = lVar7;
          }
          *(undefined8 *)(lVar3 + lVar17) = 0;
        }
      }
      else if (lVar7 != 0) {
        uVar13 = 0;
        if (extraout_x9_02 != 0) {
          uVar13 = *(ulong *)(lVar7 + 8) / extraout_x9_02;
        }
        lVar7 = *(ulong *)(lVar7 + 8) - uVar13 * extraout_x9_02;
        if (extraout_x8_02 != lVar7) {
          *(long **)(lVar3 + lVar7 * 8) = plVar10;
        }
      }
      *plVar10 = *plVar8;
      FUN_001ef794(plVar8 + 2);
      FUN_001ef190(plVar8 + 2);
      _ZdlPv(plVar8);
      DAT_00481828 = DAT_00481828 + -1;
    }
    uStack_1b = 0;
    uStack_1a = 0;
    uStack_1c = 2;
    uStack_19 = 0;
    local_20 = 0x2120;
    uStack_1e = 0x809c;
    local_18 = osal_thread_id();
    uStack_14 = (uint)local_48;
    FUN_001df8d4(&local_20,0x10);
    free(local_48);
    _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(&local_20,0x481540);
    _ZN3mpr32g_orderRegistrationConstIteratorE =
         (ulong *)CONCAT17(uStack_19,
                           CONCAT16(uStack_1a,
                                    CONCAT15(uStack_1b,
                                             CONCAT14(uStack_1c,CONCAT22(uStack_1e,local_20)))));
    DAT_00481750 = CONCAT44(uStack_14,local_18);
    DAT_00481758 = CONCAT44(uStack_c,local_10);
    DAT_00481760 = local_8;
  } while( true );
}

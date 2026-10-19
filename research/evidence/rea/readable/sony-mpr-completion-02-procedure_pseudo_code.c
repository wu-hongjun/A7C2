// Decompiled pseudocode, not original source.
// REA Evidence: ev_bf9e8538962caf559fb80830cf2fcb44b4a2b2f8384e03440a7850a9702798b2
// Source record: research/evidence/rea/sony-mpr-completion/02-procedure_pseudo_code.json

void _ZN3mpr11stillrecord20SetFsysWriteFinishedEPNS_18recordStillParam_tEj
               (undefined8 param_1,undefined4 param_2)

{
  undefined8 uVar1;
  char *pcVar2;
  undefined1 *puVar3;
  long lVar4;
  undefined4 local_8c;
  undefined8 local_88;
  undefined1 auStack_80 [32];
  undefined1 auStack_60 [32];
  long local_40 [4];
  undefined2 local_20;
  undefined2 uStack_1e;
  undefined1 uStack_1c;
  undefined1 uStack_1b;
  undefined1 uStack_1a;
  undefined1 uStack_19;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;

  local_8c = param_2;
  local_88 = param_1;
  _ZN3mpr6thread10normalLock4lockEv(&_ZN3mpr24g_mutex_writeAndTransferE);
  uStack_1c = 1;
  uStack_1b = 0;
  uStack_1a = 0;
  uStack_19 = 0;
  local_20 = 0x2120;
  uStack_1e = 0x8050;
  local_18 = osal_thread_id();
  local_c = 0;
  local_14 = (undefined4)local_88;
  local_10 = local_8c;
  FUN_001df8d4(&local_20,0x18);
  _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(auStack_80,0x481540);
  _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(auStack_60,0x481560);
  _ZSt4findISt15_Deque_iteratorIPN3mpr18recordStillParam_tERS3_PS3_ES3_ET_S7_S7_RKT0_
            (local_40,auStack_80,auStack_60,&local_88);
  _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(&local_20,0x481560);
  if (local_40[0] ==
      CONCAT17(uStack_19,
               CONCAT16(uStack_1a,
                        CONCAT15(uStack_1b,CONCAT14(uStack_1c,CONCAT22(uStack_1e,local_20)))))) {
    uStack_1b = 0;
    uStack_1c = 4;
    uStack_1a = 0;
    uStack_1e = 0x8054;
    uStack_19 = 0;
    local_20 = 0x2120;
    FUN_001df8d4(&local_20,8);
                    /* WARNING: Subroutine does not return */
    __assert_fail("false","integRecordStill.cpp",0x1ee7,
                  "void mpr::stillrecord::SetFsysWriteFinished(mpr::recordStillParam_t*, uint32_t)")
    ;
  }
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr24g_fsysWriteFinishedCountE,&local_88);
  pcVar2 = (char *)_ZNSt8__detail9_Map_baseIjSt4pairIKjN3mpr18writeReqFinCount_tEESaIS5_ENS_10_Select1stESt8equal_toIjESt4hashIjENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS2_
                             (uVar1,&local_8c);
  if (*pcVar2 == '\0') {
    uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                      (&_ZN3mpr24g_fsysWriteFinishedCountE,&local_88);
    puVar3 = (undefined1 *)
             _ZNSt8__detail9_Map_baseIjSt4pairIKjN3mpr18writeReqFinCount_tEESaIS5_ENS_10_Select1stESt8equal_toIjESt4hashIjENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS2_
                       (uVar1,&local_8c);
    *puVar3 = 1;
  }
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjNS1_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS4_IKjS7_EEEESaISG_ENS_10_Select1stESA_IS3_ES8_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr24g_fsysWriteFinishedCountE,&local_88);
  lVar4 = _ZNSt8__detail9_Map_baseIjSt4pairIKjN3mpr18writeReqFinCount_tEESaIS5_ENS_10_Select1stESt8equal_toIjESt4hashIjENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS2_
                    (uVar1,&local_8c);
  *(int *)(lVar4 + 4) = *(int *)(lVar4 + 4) + 1;
  _ZN3mpr11stillrecord27ArbitrateProcessingSequenceEv();
  uStack_1b = 0;
  uStack_1a = 0;
  uStack_19 = 0;
  uStack_1c = 1;
  local_20 = 0x2120;
  uStack_1e = 0x8058;
  local_18 = osal_thread_id();
  local_c = 0;
  local_14 = (undefined4)local_88;
  local_10 = local_8c;
  FUN_001df8d4(&local_20,0x18);
  _ZN3mpr6thread10normalLock6unlockEv(&_ZN3mpr24g_mutex_writeAndTransferE);
  return;
}

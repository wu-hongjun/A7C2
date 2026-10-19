// Decompiled pseudocode, not original source.
// REA Evidence: ev_f5104db2db254dda88a9686536e28de3a9ab3783c5dee3b1f3f476e513847c4e
// Source record: research/evidence/rea/sony-mpr-events/01-procedure_pseudo_code.json

void _ZN3mpr11stillrecord15SetWriteDbParamEPNS_18recordStillParam_tEjRN9filequeue6exec_tE
               (undefined8 param_1,uint param_2,undefined4 *param_3)

{
  undefined8 uVar1;
  long lVar2;
  undefined4 *puVar3;
  uint local_1c;
  undefined8 local_18;
  undefined2 local_10;
  undefined2 local_e;
  undefined1 local_c;
  undefined1 local_b;
  undefined1 local_a;
  undefined1 local_9;
  undefined4 local_8;
  undefined4 local_4;

  local_1c = param_2;
  local_18 = param_1;
  _ZN3mpr6thread10normalLock4lockEv(&_ZN3mpr24g_mutex_writeAndTransferE);
  local_c = 2;
  local_b = 0;
  local_a = 0;
  local_9 = 0;
  local_10 = 0x2120;
  local_e = 0x8068;
  local_8 = osal_thread_id();
  local_4 = (undefined4)local_18;
  FUN_001df8d4(&local_10,0x10);
  if (3 < local_1c) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("slotId<MAX_MEDIA_NUM","integRecordStill.cpp",0x1f12,
                  "void mpr::stillrecord::SetWriteDbParam(mpr::recordStillParam_t*, uint32_t, filequeue::exec_t&)"
                 );
  }
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr9g_writeDbE,&local_18);
  lVar2 = FUN_001ef308(uVar1,&local_1c);
  *(undefined1 *)(lVar2 + 4) = *(undefined1 *)(param_3 + 1);
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr9g_writeDbE,&local_18);
  lVar2 = FUN_001ef308(uVar1,&local_1c);
  *(undefined8 *)(lVar2 + 0x15d8) = *(undefined8 *)(param_3 + 0x576);
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr9g_writeDbE,&local_18);
  lVar2 = FUN_001ef308(uVar1,&local_1c);
  memcpy((void *)(lVar2 + 0x10),param_3 + 4,0x58);
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr9g_writeDbE,&local_18);
  lVar2 = FUN_001ef308(uVar1,&local_1c);
  *(undefined4 *)(lVar2 + 0x15d0) = param_3[0x574];
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr9g_writeDbE,&local_18);
  puVar3 = (undefined4 *)FUN_001ef308(uVar1,&local_1c);
  *puVar3 = *param_3;
  uVar1 = _ZNSt8__detail9_Map_baseIPN3mpr18recordStillParam_tESt4pairIKS3_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS4_IKjS8_EEEESaISH_ENS_10_Select1stESB_IS3_ES9_IS3_ENS_18_Mod_range_hashingENS_20_Default_ranged_hashENS_20_Prime_rehash_policyENS_17_Hashtable_traitsILb0ELb0ELb1EEELb1EEixERS5_
                    (&_ZN3mpr9g_writeDbE,&local_18);
  lVar2 = FUN_001ef308(uVar1,&local_1c);
  *(undefined4 *)(lVar2 + 8) = param_3[2];
  _ZN3mpr11stillrecord27ArbitrateProcessingSequenceEv();
  local_c = 2;
  local_10 = 0x2120;
  local_b = 0;
  local_a = 0;
  local_9 = 0;
  local_e = 0x8074;
  local_8 = osal_thread_id();
  local_4 = (undefined4)local_18;
  FUN_001df8d4(&local_10,0x10);
  _ZN3mpr6thread10normalLock6unlockEv(&_ZN3mpr24g_mutex_writeAndTransferE);
  return;
}

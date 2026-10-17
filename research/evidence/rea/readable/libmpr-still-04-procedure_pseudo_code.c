// Decompiled pseudocode, not original source.
// REA Evidence: ev_dbf017251ceabf42069324ba357d79025ba07da84a28d1e8bf372bef11e115c0
// Source record: research/evidence/rea/libmpr-still/04-procedure_pseudo_code.json

void _ZN3mpr20IntegRecordStillInitEv(void)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 *puVar3;
  long lVar4;
  long lVar5;
  long lVar6;
  long lVar7;
  undefined8 local_b8;
  undefined4 local_b0;
  code *local_a8;
  undefined8 *local_a0;
  undefined2 local_98;
  undefined4 local_94;
  undefined8 local_90;
  undefined8 local_88;
  undefined8 local_80;
  undefined8 uStack_78;
  undefined8 local_70;
  undefined8 local_68;
  undefined8 local_60;
  undefined8 local_58;
  undefined8 local_50;
  undefined8 uStack_48;
  undefined8 local_40;
  undefined8 local_38;
  undefined8 local_30;
  undefined8 local_28;
  code *local_20;
  undefined8 *local_18;
  undefined8 local_10;
  undefined8 local_8;
  
  puVar2 = &local_30;
  local_30 = 0x435002120;
  FUN_001df8d4(puVar2,8);
  if (DAT_00407d32 == '\0') {
    local_30 = 0x435102120;
  }
  else {
    DAT_00407d32 = '\0';
    DAT_004818a0 = 0;
    DAT_004818a1 = 0;
    DAT_004818a2 = 0;
    DAT_004818a3 = 0;
    _ZN3mpr11stillrecord13s_burst_countE = 1;
    DAT_0048130c = 1;
    DAT_00481310 = 1;
    DAT_00481314 = 1;
    DAT_00481854 = 0;
    DAT_00481848 = 0;
    DAT_00481850 = 0;
    _ZN3mpr35InitCheckLastWriteDataFinishedParamEj(0xffff);
    local_90 = 0;
    local_88 = 0;
    local_80 = 0;
    uStack_78 = 0;
    local_70 = 0;
    local_68 = 0;
    _ZN3mpr10ThreadInfo13GetThreadInfoEiR18__osal_thread_info(0x75,&local_90);
    puVar3 = (undefined8 *)_Znwm(0xd8);
    *puVar3 = 0x3f75a0;
    puVar3[1] = 0;
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[5] = 0;
    puVar3[6] = 0;
    puVar3[7] = 0;
    puVar3[8] = 0;
    puVar3[9] = 0;
    puVar3[10] = 0;
    puVar3[2] = 8;
    lVar4 = _Znwm(0x40);
    puVar3[1] = lVar4;
    lVar6 = (puVar3[2] - 1 >> 1) * 8;
    lVar7 = lVar4 + lVar6;
    lVar5 = _Znwm(0x658);
    puVar3[4] = lVar5;
    puVar3[5] = lVar5 + 0x658;
    puVar3[6] = lVar7;
    puVar3[8] = lVar5;
    puVar3[9] = lVar5 + 0x658;
    puVar3[10] = lVar7;
    puVar3[3] = lVar5;
    puVar3[7] = lVar5;
    *(long *)(lVar4 + lVar6) = lVar5;
    *(undefined1 *)((long)puVar3 + 0xbc) = 1;
    *(undefined4 *)(puVar3 + 0x17) = 0;
    puVar3[0x18] = puVar3;
    puVar3[0x19] = 0;
    pthread_mutex_init((pthread_mutex_t *)(puVar3 + 0xb),(pthread_mutexattr_t *)0x0);
    pthread_cond_init((pthread_cond_t *)(puVar3 + 0x11),(pthread_condattr_t *)0x0);
    local_10 = CONCAT62(local_10._2_6_,0x3c);
    local_20 = _ZN9execqueueIN3mpr11stillrecord19make_header_input_tEE6threadEPv;
    local_18 = puVar3 + 0x18;
    local_30 = local_90;
    local_28._0_4_ = (undefined4)local_88;
    local_10 = CONCAT44(local_70._4_4_,(undefined4)local_10);
    puVar3[0x19] = local_68;
    osal_create_waitable_thread(puVar2,puVar3 + 0x1a);
    _ZN3mpr11stillrecord18make_header_threadE = puVar3;
    _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE5clearEv(_ZN3mpr19g_orderRegistrationE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_jESaIS5_ENSt8__detail10_Select1stESt8equal_toIS2_ESt4hashIS2_ENS7_18_Mod_range_hashingENS7_20_Default_ranged_hashENS7_20_Prime_rehash_policyENS7_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr18g_orderThreadCountE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_NS0_18writeReqFinCount_tEESaIS6_ENSt8__detail10_Select1stESt8equal_toIS2_ESt4hashIS2_ENS8_18_Mod_range_hashingENS8_20_Default_ranged_hashENS8_20_Prime_rehash_policyENS8_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr18g_finThreadStartedE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_NS0_18writeReqFinCount_tEESaIS6_ENSt8__detail10_Select1stESt8equal_toIS2_ESt4hashIS2_ENS8_18_Mod_range_hashingENS8_20_Default_ranged_hashENS8_20_Prime_rehash_policyENS8_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr19g_finThreadFinishedE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_St13unordered_mapIjN9execqueueINS0_11stillrecord33writeAndTransferSequencer_input_tEE6exec_tESt4hashIjESt8equal_toIjESaIS3_IKjSA_EEEESaISJ_ENSt8__detail10_Select1stESD_IS2_ESB_IS2_ENSL_18_Mod_range_hashingENSL_20_Default_ranged_hashENSL_20_Prime_rehash_policyENSL_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr19g_writerThreadParamE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_NS0_18writeReqFinCount_tEESaIS6_ENSt8__detail10_Select1stESt8equal_toIS2_ESt4hashIS2_ENS8_18_Mod_range_hashingENS8_20_Default_ranged_hashENS8_20_Prime_rehash_policyENS8_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr21g_writerThreadStartedE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_NS0_18writeReqFinCount_tEESaIS6_ENSt8__detail10_Select1stESt8equal_toIS2_ESt4hashIS2_ENS8_18_Mod_range_hashingENS8_20_Default_ranged_hashENS8_20_Prime_rehash_policyENS8_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr22g_writerThreadFinishedE);
    _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE5clearEv
              (_ZN3mpr35g_orderRegistrationEvtStillTransferE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_NS0_19trasnferDataBuilt_tEESaIS6_ENSt8__detail10_Select1stESt8equal_toIS2_ESt4hashIS2_ENS8_18_Mod_range_hashingENS8_20_Default_ranged_hashENS8_20_Prime_rehash_policyENS8_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr27g_evtStillTransferDataBuiltE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_St13unordered_mapIjNS0_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS3_IKjS6_EEEESaISF_ENSt8__detail10_Select1stES9_IS2_ES7_IS2_ENSH_18_Mod_range_hashingENSH_20_Default_ranged_hashENSH_20_Prime_rehash_policyENSH_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr23g_fsysWriteRequestCountE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_St13unordered_mapIjNS0_18writeReqFinCount_tESt4hashIjESt8equal_toIjESaIS3_IKjS6_EEEESaISF_ENSt8__detail10_Select1stES9_IS2_ES7_IS2_ENSH_18_Mod_range_hashingENSH_20_Default_ranged_hashENSH_20_Prime_rehash_policyENSH_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr24g_fsysWriteFinishedCountE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_St13unordered_mapIjNS0_16writeCountdown_tESt4hashIjESt8equal_toIjESaIS3_IKjS6_EEEESaISF_ENSt8__detail10_Select1stES9_IS2_ES7_IS2_ENSH_18_Mod_range_hashingENSH_20_Default_ranged_hashENSH_20_Prime_rehash_policyENSH_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr24g_evtStillWriteCountdownE);
    _ZNSt10_HashtableIPN3mpr18recordStillParam_tESt4pairIKS2_St13unordered_mapIjN9filequeue6exec_tESt4hashIjESt8equal_toIjESaIS3_IKjS7_EEEESaISG_ENSt8__detail10_Select1stESA_IS2_ES8_IS2_ENSI_18_Mod_range_hashingENSI_20_Default_ranged_hashENSI_20_Prime_rehash_policyENSI_17_Hashtable_traitsILb0ELb0ELb1EEEE5clearEv
              (&_ZN3mpr9g_writeDbE);
    _ZN3mpr15broadcast_queue4initEv();
    local_60 = 0;
    local_58 = 0;
    local_50 = 0;
    uStack_48 = 0;
    local_40 = 0;
    local_38 = 0;
    _ZN3mpr10ThreadInfo13GetThreadInfoEiR18__osal_thread_info(0x8d,&local_60);
    puVar3 = (undefined8 *)_Znwm(0xd8);
    *puVar3 = 0x3f7560;
    puVar3[1] = 0;
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[5] = 0;
    puVar3[6] = 0;
    puVar3[7] = 0;
    puVar3[8] = 0;
    puVar3[9] = 0;
    puVar3[10] = 0;
    puVar3[2] = 8;
    lVar4 = _Znwm(0x40);
    lVar7 = puVar3[2];
    puVar3[1] = lVar4;
    lVar5 = _Znwm(0x1e0);
    puVar3[4] = lVar5;
    puVar3[5] = lVar5 + 0x1e0;
    lVar6 = (lVar7 - 1U >> 1) * 8;
    puVar3[8] = lVar5;
    puVar3[9] = lVar5 + 0x1e0;
    lVar7 = lVar4 + lVar6;
    puVar3[6] = lVar7;
    puVar3[10] = lVar7;
    puVar3[3] = lVar5;
    puVar3[7] = lVar5;
    *(long *)(lVar4 + lVar6) = lVar5;
    *(undefined1 *)((long)puVar3 + 0xbc) = 1;
    *(undefined4 *)(puVar3 + 0x17) = 0;
    puVar3[0x18] = puVar3;
    puVar3[0x19] = 0;
    pthread_mutex_init((pthread_mutex_t *)(puVar3 + 0xb),(pthread_mutexattr_t *)0x0);
    pthread_cond_init((pthread_cond_t *)(puVar3 + 0x11),(pthread_condattr_t *)0x0);
    local_10 = CONCAT62(local_10._2_6_,0x3c);
    local_20 = _ZN9execqueueIN3mpr11stillrecord33writeAndTransferSequencer_input_tEE6threadEPv;
    local_18 = puVar3 + 0x18;
    local_30 = local_60;
    local_28._0_4_ = (undefined4)local_58;
    local_10 = CONCAT44(local_40._4_4_,(undefined4)local_10);
    puVar3[0x19] = local_38;
    osal_create_waitable_thread(puVar2,puVar3 + 0x1a);
    _ZN3mpr11stillrecord32writeAndTransferSequencer_threadE = puVar3;
    _ZN3mpr11stillrecord20InitHashRequestQueueEv();
    iVar1 = _ZN3mpr6saHash10InitSaHashEv();
    if (iVar1 != 0) {
      local_28 = CONCAT44(local_28._4_4_,iVar1);
      local_30 = 0x335202120;
      FUN_001df8d4(puVar2,0xc);
    }
    local_30 = 0;
    local_28 = 0;
    local_20 = (code *)0x0;
    local_18 = (undefined8 *)0x0;
    local_10 = 0;
    local_8 = 0;
    _ZN3mpr10ThreadInfo13GetThreadInfoEiR18__osal_thread_info(0x73,puVar2);
    puVar3 = (undefined8 *)_Znwm(0xd8);
    *puVar3 = 0x3f7580;
    puVar3[1] = 0;
    puVar3[3] = 0;
    puVar3[4] = 0;
    puVar3[5] = 0;
    puVar3[6] = 0;
    puVar3[7] = 0;
    puVar3[8] = 0;
    puVar3[9] = 0;
    puVar3[10] = 0;
    puVar3[2] = 8;
    lVar4 = _Znwm(0x40);
    lVar7 = puVar3[2];
    puVar3[1] = lVar4;
    lVar5 = _Znwm(0x200);
    puVar3[4] = lVar5;
    puVar3[5] = lVar5 + 0x200;
    lVar6 = (lVar7 - 1U >> 1) * 8;
    puVar3[8] = lVar5;
    puVar3[9] = lVar5 + 0x200;
    lVar7 = lVar4 + lVar6;
    puVar3[6] = lVar7;
    puVar3[10] = lVar7;
    puVar3[3] = lVar5;
    puVar3[7] = lVar5;
    *(long *)(lVar4 + lVar6) = lVar5;
    *(undefined4 *)(puVar3 + 0x17) = 0;
    puVar2 = &local_b8;
    *(undefined1 *)((long)puVar3 + 0xbc) = 1;
    puVar3[0x18] = puVar3;
    puVar3[0x19] = 0;
    pthread_mutex_init((pthread_mutex_t *)(puVar3 + 0xb),(pthread_mutexattr_t *)0x0);
    pthread_cond_init((pthread_cond_t *)(puVar3 + 0x11),(pthread_condattr_t *)0x0);
    local_a8 = _ZN9execqueueIN3mpr11stillrecord22write_finished_input_tEE6threadEPv;
    local_98 = 0x3c;
    local_a0 = puVar3 + 0x18;
    local_b8 = local_30;
    local_b0 = (undefined4)local_28;
    local_94 = local_10._4_4_;
    puVar3[0x19] = local_8;
    osal_create_waitable_thread(puVar2,puVar3 + 0x1a);
    local_b8 = 0x435f02120;
    _ZN3mpr11stillrecord21write_finished_threadE = puVar3;
  }
  FUN_001df8d4(puVar2,8);
  return;
}


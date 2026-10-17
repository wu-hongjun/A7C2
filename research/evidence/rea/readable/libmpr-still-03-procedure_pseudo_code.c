// Decompiled pseudocode, not original source.
// REA Evidence: ev_fe48144032987374c001aff81df2b98e27baa803334825f413e545ea1a28c72e
// Source record: research/evidence/rea/libmpr-still/03-procedure_pseudo_code.json

undefined4 _ZN3mpr16SetStillRecParamERKNS_13stillRecParamE(void *param_1)

{
  undefined4 uVar1;
  undefined4 local_2b0;
  undefined4 uStack_2ac;
  undefined1 auStack_2a8 [680];
  
  local_2b0 = 0xe;
  memcpy(auStack_2a8,param_1,0x2a8);
  FUN_001f29f4(&local_2b0,0x2b0);
  _ZNK3mpr13stillRecParam8DbgPrintEv(param_1);
  pthread_mutex_lock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
  uVar1 = _ZN3mpr24SetStillRecParamInternalERKNS_13stillRecParamEbb(param_1,1);
  pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
  local_2b0 = 0xf;
  uStack_2ac = uVar1;
  FUN_001f29f4(&local_2b0,8);
  return uVar1;
}


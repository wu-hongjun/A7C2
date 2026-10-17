// Decompiled pseudocode, not original source.
// REA Evidence: ev_e3ce5eb853f0f3eef12be42c521d1d70f5c646c266fdf1594bf29b0d3e7907f1
// Source record: research/evidence/rea/libmpr/01-procedure_pseudo_code.json

undefined4 _ZN3mpr10SetLutDataERKNS_17setLutDataInParamE(long param_1)

{
  undefined4 uVar1;
  undefined4 local_8;
  undefined4 uStack_4;
  
  FUN_001ca0d0(param_1,0x60);
  _ZNK3mpr17setLutDataInParam8DbgPrintEv(param_1);
  pthread_mutex_lock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
  if (*(char *)(param_1 + 4) == '\0') {
    _ZN3mpr12ResetLutDataEv();
    uVar1 = 0;
    pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
    local_8 = 0x6f;
    uStack_4 = 0;
    FUN_001ca0d0(&local_8,8);
  }
  else {
    uVar1 = _ZN3mpr17ExecuteSetLutDataERKNS_17setLutDataInParamE(param_1);
    pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
    local_8 = 0x6f;
    uStack_4 = uVar1;
    FUN_001ca0d0(&local_8,8);
  }
  return uVar1;
}


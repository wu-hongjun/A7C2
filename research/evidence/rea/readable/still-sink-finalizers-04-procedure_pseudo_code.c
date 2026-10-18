// Decompiled pseudocode, not original source.
// REA Evidence: ev_06ba83e65e6627e15cc6666fae414ba2e6ca0c50d3840687ce494bbb450243fc
// Source record: research/evidence/rea/still-sink-finalizers/04-procedure_pseudo_code.json

void _ZN3mpr11stillrecord19FinalizeStillHeaderEPv(long *param_1)

{
  char *pcVar1;
  long lVar2;
  long lVar3;
  int *piVar4;
  long local_460;
  undefined4 local_458;
  long local_450;
  undefined4 local_448;
  code *local_440;
  undefined1 auStack_438 [4];
  undefined4 local_434;
  undefined4 local_304;
  undefined1 auStack_300 [336];
  int local_1b0;
  undefined2 local_180;
  undefined2 local_17e;
  undefined1 local_17c;
  undefined1 local_17b;
  undefined1 local_17a;
  undefined1 local_179;
  undefined4 local_178;
  undefined4 local_174;
  uint local_170;
  undefined4 local_16c;
  int local_150;
  int local_30;
  
  _ZN3mpr11stillrecord19SetFinThreadStartedEPNS_18recordStillParam_tE(*param_1);
  if (*(int *)(*param_1 + (ulong)*(uint *)((long)param_1 + 0x2c) * 0x588 + 0x20c) == -1) {
    local_17b = 0;
    local_17a = 0;
    piVar4 = (int *)0x0;
    local_17c = 2;
    local_179 = 0;
    local_180 = 0x2120;
    local_17e = 0x6b07;
    local_178 = osal_thread_id();
    local_174 = 0;
    FUN_001df8d4(&local_180,0x10);
  }
  else {
    piVar4 = (int *)_Znwm(4);
    *piVar4 = 0;
    lVar3 = *param_1;
    lVar2 = (ulong)*(uint *)((long)param_1 + 0x2c) * 0x588;
    pcVar1 = (char *)(lVar3 + lVar2 + 0x23c);
    do {
      if (*pcVar1 != '\0') {
        *piVar4 = *piVar4 + 1;
      }
      pcVar1 = pcVar1 + 0x106;
    } while ((char *)(lVar3 + lVar2 + 0x654) != pcVar1);
  }
  local_17b = 0;
  local_17a = 0;
  local_17c = 1;
  local_179 = 0;
  local_180 = 0x2120;
  local_17e = 0x6b0a;
  local_178 = osal_thread_id();
  local_174 = *(undefined4 *)((long)param_1 + 0x1c);
  local_16c = 0;
  local_170 = (uint)*(byte *)(param_1 + 4);
  FUN_001df8d4(&local_180,0x18);
  memset(auStack_300,0,0x180);
  memset(&local_180,0,0x180);
  _ZN3mpr30evtStillTransferDataBuiltParamC2Ev(auStack_438);
  local_304 = 0;
  local_434 = *(undefined4 *)((long)param_1 + 0x1c);
  if ((char)param_1[4] == '\0') {
    if ((char)param_1[6] == '\0') {
      _ZN3mpr11stillrecord14_make_fin_if_tENS_9StillTypeEPNS_18recordStillParam_tEbbbPN5still8fin_if_tEP10dmm_munmap
                (*(undefined4 *)(*param_1 + (ulong)*(uint *)((long)param_1 + 0x2c) * 0x588 + 500),
                 *param_1,*(undefined1 *)((long)param_1 + 0x21),
                 *(undefined1 *)((long)param_1 + 0x23),*(undefined1 *)((long)param_1 + 0x25),
                 auStack_300,piVar4);
      FUN_00370f9c(param_1[1],auStack_300);
    }
    else {
      _ZN3mpr11stillrecord14_make_fin_if_tENS_9StillTypeEPNS_18recordStillParam_tEbbbPN5still8fin_if_tEP10dmm_munmap
                ((int)param_1[3],*param_1,*(undefined1 *)((long)param_1 + 0x22),
                 *(undefined1 *)((long)param_1 + 0x24),*(undefined1 *)((long)param_1 + 0x26),
                 &local_180,piVar4);
      FUN_00370f9c(param_1[2],&local_180);
      local_1b0 = local_30;
      if (local_150 != 0) {
        FUN_003b24f0();
        local_1b0 = local_30;
      }
    }
    if (local_1b0 != 0) {
      FUN_003b24f0();
    }
    if ((*(char *)(*param_1 + (ulong)*(uint *)((long)param_1 + 0x2c) * 0x588 + 0x675) == '\0') ||
       ((char)param_1[6] != '\0')) {
      _ZN3mpr11stillrecord20_make_transfer_imageEP8libstillS2_PNS_18recordStillParam_tEPNS_30evtStillTransferDataBuiltParamEj
                (param_1[1],param_1[2],*param_1,auStack_438);
      goto LAB_001f0b5c;
    }
  }
  else {
LAB_001f0b5c:
    if ((char)param_1[6] != '\0') goto LAB_001f0b94;
  }
  local_448 = *(undefined4 *)((long)param_1 + 0x1c);
  local_458 = *(undefined4 *)((long)param_1 + 0x2c);
  local_440 = _ZN3mpr11stillrecord26_writeAndTransferSequencerEPv;
  local_460 = *param_1;
  local_450 = param_1[1];
  _ZN3mpr11stillrecord20SetWriterThreadParamEPNS_18recordStillParam_tEjRN9execqueueINS0_33writeAndTransferSequencer_input_tEE6exec_tE
            (local_460,local_458,&local_460);
LAB_001f0b94:
  _ZN3mpr11stillrecord33SetEvtStillTransferDataBuiltParamEPNS_18recordStillParam_tEjbNS_5EVTIDERNS_30evtStillTransferDataBuiltParamEm
            (*param_1,*(undefined4 *)((long)param_1 + 0x2c),(char)param_1[6],0x102c,auStack_438,
             0x138);
  if ((char)param_1[6] == '\0') {
    _ZN3mpr11stillrecord20ShareStillInfoMemoryEibPNS_18recordStillParam_tEP10dmm_munmapPN5still8fin_if_tEbb
              (*(undefined4 *)((long)param_1 + 0x2c),(char)param_1[4],*param_1,piVar4,auStack_300,
               *(undefined1 *)((long)param_1 + 0x25),*(undefined1 *)((long)param_1 + 0x23));
    _ZN3mpr11stillrecord19FreeStillInfoMemoryEibPNS_18recordStillParam_tEPN5still8fin_if_tEbb
              (*(undefined4 *)((long)param_1 + 0x2c),(char)param_1[4],*param_1,auStack_300,
               *(undefined1 *)((long)param_1 + 0x25),*(undefined1 *)((long)param_1 + 0x23));
    _ZN3mpr11stillrecord13AddStillCacheEibPNS_18recordStillParam_tEP8libstillP10dmm_munmapPN5still8fin_if_tE
              (*(undefined4 *)((long)param_1 + 0x2c),(char)param_1[4],*param_1,param_1[1],piVar4,
               auStack_300);
    _ZN3mpr11stillrecord17ReleaseStillCacheEiPNS_18recordStillParam_tE
              (*(undefined4 *)((long)param_1 + 0x2c),*param_1);
  }
  _ZN3mpr11stillrecord20SetFinThreadFinishedEPNS_18recordStillParam_tE(*param_1);
  return;
}


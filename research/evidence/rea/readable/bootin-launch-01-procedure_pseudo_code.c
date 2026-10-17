// Decompiled pseudocode, not original source.
// REA Evidence: ev_04d8c0e701cb821c089eb3643817cdb4517bb89b05eede042a809aca1c14cf02
// Source record: research/evidence/rea/bootin-launch/01-procedure_pseudo_code.json

uint FUN_0040297c(undefined4 param_1,undefined8 param_2)

{
  int iVar1;
  uint uVar2;
  
  DAT_004151f8 = getpid();
  signal(0x11,(__sighandler_t)0x0);
  iVar1 = FUN_00402334(param_1,param_2);
  if (iVar1 == 0) {
    if ((DAT_004151c8 & 1) == 0) {
      uVar2 = osal_reg_sync_msg_queue_cb(0x94020a,&DAT_004151f0,&LAB_00401a6c,0);
      if (uVar2 != 0) {
        return uVar2;
      }
      FUN_004036b8(FUN_00401a38);
      return 0;
    }
    uVar2 = 2;
    FUN_00401e40(0x94020a,0x940002,0);
  }
  else {
    uVar2 = 0xf2;
    FUN_00401f14();
  }
  return uVar2 | 0x940000;
}


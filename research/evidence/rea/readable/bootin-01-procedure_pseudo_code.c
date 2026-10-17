// Decompiled pseudocode, not original source.
// REA Evidence: ev_ba0eacd4573bde764a26ae3bd3551c2215a1f57a2a93b01b7c36ed17ff4283c4
// Source record: research/evidence/rea/bootin/01-procedure_pseudo_code.json

undefined8 FUN_004017e0(undefined4 param_1,undefined8 param_2)

{
  int iVar1;
  undefined8 local_8;
  
  FUN_00401cf8(2,"Bb bootin");
  iVar1 = FUN_0040297c(param_1,param_2);
  if (iVar1 == 0) {
    FUN_00401cf8(2,"Bb ulogio");
    FUN_00402170();
    FUN_00401cf8(0,"Bb imelf");
    iVar1 = FUN_00402b90();
    if (iVar1 == 0) {
      FUN_00401cf8(1,"Bb imelf");
      FUN_00401cf8(1,"Bb bootin");
      while (DAT_004151cc != 3) {
        iVar1 = osal_rcv_msg_tmo(0x94020a,&local_8,0xffffffff);
        if (iVar1 == 0) {
          iVar1 = FUN_004030ec(local_8);
          if (iVar1 != 0) {
            sleep(1);
          }
          osal_free_msg(0x94020a,local_8);
        }
        else {
          sleep(1);
        }
      }
      return 0;
    }
  }
  else {
    FUN_00401cf8(2,"Bb bootin init failed");
  }
                    /* WARNING: Subroutine does not return */
  exit(iVar1);
}


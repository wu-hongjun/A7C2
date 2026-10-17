// Decompiled pseudocode, not original source.
// REA Evidence: ev_46f6a4316e682cdbc1b9802a70b60f8a3ff13c51bda85c46dab1ca9d326c5ef5
// Source record: research/evidence/rea/im/00-procedure_pseudo_code.json

ulong FUN_00402490(undefined4 param_1,undefined8 param_2)

{
  __pid_t _Var1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  long lVar6;
  ulong uVar7;
  code *pcVar8;
  undefined8 local_28;
  undefined8 local_20;
  undefined8 local_18;
  undefined8 local_10;
  ulong local_8;
  
  FUN_00405880(0,&DAT_00407553,0x94,"main()");
  signal(0x11,FUN_00402f4c);
  FUN_004071a8(FUN_00402f74);
  FUN_004055d8(3,FUN_00404480);
  _Var1 = getpid();
  FUN_00404d68(_Var1);
  iVar2 = FUN_00404d90(param_1,param_2);
  if (iVar2 != 0) {
    FUN_00404d8c();
    uVar5 = 0xf2;
LAB_00402514:
    return (ulong)(uVar5 | 0x940000);
  }
  iVar2 = FUN_004054c4();
  if (iVar2 != 0) {
    FUN_00402d68(0x94016a,0x940002,0,0,0);
    uVar5 = 2;
    goto LAB_00402514;
  }
  FUN_00403acc();
  FUN_00403b00();
  lVar6 = FUN_00404ce8();
  if (lVar6 == 0) {
    lVar6 = FUN_00404cf8("default");
  }
  FUN_004054d4();
  iVar2 = FUN_004038d4();
  if (iVar2 == 0) {
    FUN_004056bc(lVar6,DAT_0041970c);
    FUN_00404d80();
    FUN_004057b4();
    iVar2 = FUN_0040661c(0,0x8000001);
    if (-1 < iVar2) {
      uVar3 = FUN_00405554();
      iVar2 = FUN_0040661c(iVar2,0x8000002,uVar3);
      if (-1 < iVar2) {
        iVar4 = FUN_00404d74();
        iVar4 = osal_reg_sync_msg_queue_cb(0x94016a,&local_28,FUN_00402e08,(long)iVar4);
        if (iVar4 != 0) {
          iVar4 = FUN_004038d4();
          if (iVar4 != 0) goto LAB_00402790;
          usleep(10);
        }
        FUN_00404d20(local_28);
        FUN_00404d08(0x94016a);
        FUN_004071a8(FUN_00403414);
        FUN_00403b30();
        uVar3 = FUN_00405564();
        iVar2 = FUN_0040661c(iVar2,0x8000003,uVar3);
        if (-1 < iVar2) {
          uVar3 = FUN_00405574();
          iVar2 = FUN_0040661c(iVar2,0x8000004,uVar3);
          if (-1 < iVar2) {
            uVar3 = FUN_00405584();
            iVar2 = FUN_0040661c(iVar2,0x8000005,uVar3);
            if (-1 < iVar2) {
              local_20 = 0;
              local_18 = 0;
              local_10 = 0;
              local_8 = 0;
              FUN_00403be8(&local_20);
              FUN_00403c04(&local_18);
              FUN_00403c20(&local_10);
              osal_wait_thread_exit(local_20);
              osal_wait_thread_exit(local_18);
              osal_wait_thread_exit(local_10);
              iVar2 = FUN_0040661c(0,0x8000007,0);
              if (-1 < iVar2) {
                uVar3 = FUN_00405594();
                iVar2 = FUN_0040661c(iVar2,0x8000008,uVar3);
                if (-1 < iVar2) {
                  osal_wai_sem_tmo(0x83037a,0xffffffff);
                  FUN_00403bcc(&local_8);
                  osal_sig_sem(0x83037a);
                  osal_wait_thread_exit(local_8);
                  iVar2 = osal_reg_sync_msg_queue_cb(0x80920169,&local_8,FUN_00402e08,0);
                  if (iVar2 == 0) {
                    iVar2 = FUN_00402d68(0x80920021,0x92016e,0x80920169,0,0,0x94016a);
                    if (iVar2 == 0) {
                      FUN_00404d50(local_8);
                      FUN_00404d38(0x80920169);
                      FUN_004071a8(FUN_004039a4);
                      FUN_004055d8(3,FUN_00404480);
                      FUN_0040579c(2);
                      goto LAB_004028ac;
                    }
                    osal_unreg_msg_queue_cb(0x80920169,local_8);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
LAB_00402790:
  iVar2 = FUN_00405544();
  if ((((iVar2 == 0) && (iVar2 = FUN_00405554(), iVar2 == 0)) &&
      (iVar2 = FUN_00405564(), iVar2 == 0)) &&
     (((iVar2 = FUN_00405574(), iVar2 == 0 && (iVar2 = FUN_00405584(), iVar2 == 0)) &&
      ((iVar2 = FUN_00405594(), iVar2 == 0 && (iVar2 = FUN_004055a4(), iVar2 == 0)))))) {
    iVar2 = FUN_00405524();
    if (iVar2 == 0) {
      iVar2 = FUN_00405534();
      if (iVar2 == 0) {
        iVar2 = FUN_00405514();
        if (iVar2 == 0) {
          FUN_00403180(&local_8);
          FUN_00404d08(local_8 & 0xffffffff);
          FUN_004071a8(FUN_004034e4);
          FUN_004055b4(3);
          FUN_004055d8(5,FUN_00403524);
          _Var1 = getpid();
          FUN_00404d68(_Var1);
          uVar3 = FUN_00404d74();
          FUN_00402d68(0x94016a,0x94001a,uVar3,local_8 & 0xffffffff,3);
LAB_004028ac:
          iVar2 = FUN_004054e4();
          if (iVar2 != 0) {
            FUN_00402d68(0x94016a,0x9400f1,0,0,0);
          }
          uVar7 = FUN_00405880(1,&DAT_00407553,0x94,"main()");
LAB_004028ec:
          uVar7 = FUN_0040540c(uVar7);
          if ((int)uVar7 == 0) {
            return uVar7;
          }
          uVar3 = FUN_00404d14();
          pcVar8 = (code *)FUN_004055ec();
          iVar2 = osal_rcv_msg_tmo(uVar3,&local_8,0xffffffff);
          if (iVar2 == 0) goto code_r0x00402918;
          goto LAB_00402934;
        }
        FUN_00402d68(0x94016a,0x9400f5,0,0,0);
        sleep(1);
        goto LAB_00402ab0;
      }
      uVar5 = 0xf4;
    }
    else {
      uVar5 = 0xf3;
    }
  }
  else {
    iVar2 = FUN_00405544();
    if (iVar2 != 0) {
      FUN_00402d68(0x94016a,0x940031,0,0,0);
    }
    iVar2 = FUN_00405554();
    if (iVar2 != 0) {
      FUN_00402d68(0x94016a,0x940032,0,0,0);
    }
    iVar2 = FUN_00405564();
    if (iVar2 != 0) {
      FUN_00402d68(0x94016a,0x940033,0,0,0);
    }
    iVar2 = FUN_00405574();
    if (iVar2 != 0) {
      FUN_00402d68(0x94016a,0x940034,0,0,0);
    }
    iVar2 = FUN_00405584();
    if (iVar2 != 0) {
      FUN_00402d68(0x94016a,0x940035,0,0,0);
    }
    iVar2 = FUN_00405594();
    if (iVar2 != 0) {
      FUN_00402d68(0x94016a,0x940036,0,0,0);
    }
    iVar2 = FUN_004055a4();
    if (iVar2 == 0) goto LAB_00402ab0;
    uVar5 = 0x37;
  }
  FUN_00402d68(0x94016a,uVar5 | 0x940000,0,0,0);
LAB_00402ab0:
                    /* WARNING: Subroutine does not return */
  exit(0);
code_r0x00402918:
  iVar2 = (*pcVar8)(local_8);
  uVar7 = osal_free_msg(uVar3,local_8);
  if (iVar2 != 0) {
LAB_00402934:
    FUN_004054d4();
    uVar5 = sleep(1);
    uVar7 = (ulong)uVar5;
  }
  goto LAB_004028ec;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_c3eb7acbd815d3c9575f42673b4985dc695efcecabdf8b4f0f07a86fb98d9c0d
// Source record: research/evidence/rea/jiritsu-exact/03-procedure_pseudo_code.json

undefined8 _ZN10IM_JIRITSU10CImJiritsu10jit_threadEPv(undefined8 *param_1,undefined4 param_2)

{
  int *piVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  undefined8 uVar6;
  char *pcVar7;
  size_t sVar8;
  code *pcVar9;
  long lVar10;
  FILE *pFVar11;
  int *local_2c08;
  undefined1 auStack_2c00 [48];
  long local_2bd0;
  undefined1 auStack_2b80 [16];
  uint local_2b70;
  undefined1 auStack_2b00 [128];
  undefined1 auStack_2a80 [128];
  undefined1 auStack_2a00 [24];
  undefined4 local_29e8;
  undefined4 local_29e0;
  char acStack_11c0 [4544];
  
  FUN_0010b878(0,"[JIT] jit_thread B. ");
  *(undefined4 *)(param_1 + 1) = 2;
  _Z18jiritsu_cpp_logrechPKc(0xffffff84,"jit_thread:");
  _Z18jiritsu_cpp_logrechPKcj(0xffffff84,"begin. qid=0x%08x",param_2);
  uVar6 = osal_thread_id();
  osal_set_cpu(uVar6,1);
  do {
    while( true ) {
      while( true ) {
        if (*(int *)(param_1 + 1) == 7) {
          *(undefined4 *)(param_1 + 1) = 10;
          FUN_0010b878(1,"[JIT] jit_thread E. ");
          return 0;
        }
        _Z18jiritsu_cpp_logrechPKcj(0xffffff84,"rcv (qid:0x%x)",param_2);
        iVar3 = osal_rcv_msg_tmo(param_2,&local_2c08,0xffffffff);
        piVar1 = local_2c08;
        if (iVar3 == 0) break;
        _Z18jiritsu_cpp_logrechPKcj(0xffffff81,"rcv failed(%d).",iVar3);
      }
      *(undefined4 *)(param_1 + 1) = 9;
      if (local_2c08 != (int *)0x0) break;
      _Z18jiritsu_cpp_logrechPKc(0xffffff81,"packet failed.");
    }
    iVar3 = *local_2c08;
    if (iVar3 == 2) {
      *(undefined4 *)(param_1 + 1) = 0x19;
      memset(auStack_2a00,0,0x2a00);
      *param_1 = auStack_2a00;
      strcpy(acStack_11c0,"/tmp/jiritsu/jit.log");
      local_29e8 = 1;
      local_29e0 = 0x20170213;
      FUN_0010b878(0,"[JIT] libJit_so method load RunScriptI B. ");
      pcVar9 = (code *)dlsym(param_1[3],"RunScriptI");
      pcVar7 = (char *)dlerror();
      if (pcVar7 != (char *)0x0) {
        sVar8 = strlen(pcVar7);
        _Z22jiritsu_cppdump_logrechPKcPhj(0xffffff81,"sym load err pErr",pcVar7,sVar8 & 0xffffffff);
        uVar4 = 0x1c;
        goto LAB_0010befc;
      }
      FUN_0010b878(1,"[JIT] libJit_so method load RunScriptI E. ");
      *(undefined4 *)(param_1 + 1) = 0x1a;
      iVar3 = open("/tmp/jbusy",0x41,0x1ed);
      if (iVar3 < 0) {
        _Z18jiritsu_cpp_logrechPKc(0xffffff81,"jbusy oepn fail");
        uVar4 = 0x1b;
        goto LAB_0010befc;
      }
      close(iVar3);
      FUN_0010b878(0,"[JIT] libJit_so method run RunScriptI B. ");
      (*pcVar9)(param_1[4],auStack_2a00,*(undefined8 *)(piVar1 + 6));
      unlink("/tmp/jbusy");
      *param_1 = 0;
      pcVar7 = "none";
      if (*(char **)(piVar1 + 6) != (char *)0x0) {
        pcVar7 = *(char **)(piVar1 + 6);
      }
      sVar8 = strlen(pcVar7);
      _Z22jiritsu_cppdump_logrechPKcPhj(0xffffff84,"run done Func",pcVar7,sVar8 & 0xffffffff);
      pcVar7 = "[JIT] libJit_so method run RunScriptI E. ";
      *(undefined4 *)(param_1 + 1) = 0x1d;
LAB_0010c39c:
      FUN_0010b878(1,pcVar7);
    }
    else {
      if (iVar3 == 3) {
        _Z18jiritsu_cpp_logrechPKc(0xffffff81,"exiting");
        _ZN10IM_JIRITSU10CImJiritsu7releaseEv(param_1);
LAB_0010bdc4:
        uVar4 = 7;
      }
      else if (iVar3 == 1) {
        FUN_0010b878(0,"[JIT] libJit_so load B. ");
        iVar3 = FUN_001122f0("/tmp/jmode",auStack_2c00);
        if (iVar3 != 0) {
          pcVar7 = "not jmode";
LAB_0010bdbc:
          _Z18jiritsu_cpp_logrechPKc(0xffffff84,pcVar7);
          goto LAB_0010bdc4;
        }
        iVar3 = FUN_001122f0("/tmp/jiritsu",auStack_2b80);
        if ((iVar3 == 0) && ((local_2b70 & 0xf000) == 0x4000)) {
          FUN_0010b878(0,"[JIT] sys_mount B. ");
          iVar3 = sys_mount("/dev/nflasha6","/tmp/jiritsu");
          if (iVar3 == 0) {
            FUN_0010b878(1,"[JIT] sys_mount E. ");
            iVar3 = FUN_001122f0("/tmp/jiritsu/jinit",auStack_2a80);
            if (iVar3 == 0) {
              pcVar7 = "libJit.so crashed.";
              goto LAB_0010bdbc;
            }
            _ZN10IM_JIRITSU10CImJiritsu7releaseEv(param_1);
            iVar3 = sys_file_chk_exist("/tmp/jiritsu.bak.tar");
            if (iVar3 == 0) {
              iVar3 = sys_file_chk_exist("/tmp/jiritsu.bak.hash");
              if (iVar3 == 0) {
                _Z18jiritsu_cpp_logrechPKc(0xffffff84,"Recovery mode execute.");
                UI_PrintString(0,0x80ff00ff,"Recovery mode execute.");
                iVar3 = sys_tar_extract("/tmp/jiritsu.bak.tar","/tmp/jiritsu");
                if (iVar3 != 0) {
                  _Z18jiritsu_cpp_logrechPKc(0xffffff81,"Recovery file extract error. ");
                  UI_PrintString(0,0x80ff00ff,"Recovery error.(extract failed)");
                  uVar4 = 0x16;
                  goto LAB_0010befc;
                }
              }
              iVar3 = sys_file_remove("/tmp/jiritsu.bak.tar");
              iVar5 = sys_file_remove("/tmp/jiritsu.bak.hash");
              if (iVar3 + iVar5 != 0) {
                _Z18jiritsu_cpp_logrechPKc(0xffffff81,"Recovery file remove error.");
                UI_PrintString(0,0x80ff00ff,"Recovery error.(backup delete failed)");
                uVar4 = 0x17;
                goto LAB_0010befc;
              }
              UI_PrintString(0,0,"");
            }
            FUN_0010b878(0,"[JIT] ramdisk_mount B. ");
            iVar3 = ramdisk_mount("/tmp/jiritsu/ramdsk_libJit","size=5M");
            if (iVar3 != 0) {
              _Z18jiritsu_cpp_logrechPKc(0xffffff81,"RAM_DISK mount error.");
              goto LAB_0010be60;
            }
            FUN_0010b878(1,"[JIT] ramdisk_mount E. ");
            FUN_0010b878(0,"[JIT] libJit_so copy B. ");
            iVar3 = sys_file_copy(*(undefined8 *)(piVar1 + 2),"/tmp/jiritsu/ramdsk_libJit/libJit.so"
                                 );
            if (iVar3 == 0) {
              FUN_0010b878(1,"[JIT] libJit_so copy E. ");
              FUN_0010b878(1,"[JIT] libJit_so load E. ");
              FUN_0010b878(0,"[JIT] libJit_so dll open B. ");
              uVar6 = dlopen("/tmp/jiritsu/ramdsk_libJit/libJit.so",1);
              param_1[3] = uVar6;
              pcVar7 = (char *)dlerror();
              if (param_1[3] == 0) {
                sVar8 = strlen(pcVar7);
                _Z22jiritsu_cppdump_logrechPKcPhj
                          (0xffffff81,"dlopen err pErr",pcVar7,sVar8 & 0xffffffff);
                uVar4 = 0x12;
              }
              else {
                FUN_0010b878(1,"[JIT] libJit_so dll open E. ");
                FUN_0010b878(0,"[JIT] libJit_so method load CreateScriptI B. ");
                pcVar9 = (code *)dlsym(param_1[3],"CreateScriptI");
                pcVar7 = (char *)dlerror();
                if (pcVar7 == (char *)0x0) {
                  FUN_0010b878(1,"[JIT] libJit_so method load CreateScriptI E. ");
                  FUN_0010b878(0,"[JIT] libJit_so method run CreateScriptI B. ");
                  lVar10 = (*pcVar9)();
                  param_1[4] = lVar10;
                  if (lVar10 == 0) {
                    _Z18jiritsu_cpp_logrechPKc(0xffffff81,"create fail");
                    uVar4 = 0x10;
                  }
                  else {
                    FUN_0010b878(1,"[JIT] libJit_so method run CreateScriptI E. ");
                    FUN_0010b878(0,"[JIT] libJit_so method load LoadScriptI B. ");
                    pcVar9 = (code *)dlsym(param_1[3],"LoadScriptI");
                    pcVar7 = (char *)dlerror();
                    if (pcVar7 == (char *)0x0) {
                      FUN_0010b878(1,"[JIT] libJit_so method load LoadScriptI E. ");
                      if (1 < local_2bd0) {
                        FUN_0010b878(0,"[JIT] jmode open B. ");
                        iVar3 = open("/tmp/jmode",0);
                        if (iVar3 < 0) {
                          _Z18jiritsu_cpp_logrechPKc(0xffffff81,"jmode open fail");
                          uVar4 = 0xf;
                          goto LAB_0010befc;
                        }
                        lVar10 = local_2bd0;
                        if (0x40 < local_2bd0) {
                          lVar10 = 0x40;
                        }
                        read(iVar3,param_1 + 5,(long)(int)lVar10);
                        close(iVar3);
                        FUN_0010b878(1,"[JIT] jmode open E. ");
                      }
                      if ((*(char *)((long)param_1 + 0x2a) == 'E') &&
                         (iVar3 = FUN_001122f0("/tmp/jbusy",auStack_2b00), iVar3 == 0)) {
                        pcVar7 = *(char **)(piVar1 + 2);
                        sVar8 = strlen(pcVar7);
                        _Z22jiritsu_cppdump_logrechPKcPhj
                                  (0xffffff84,"load emergency file",pcVar7,sVar8 & 0xffffffff);
                        iVar3 = FUN_001122f0("/tmp/jiritsu/tmmc.jit",auStack_2a00);
                        if (iVar3 == 0) {
                          cVar2 = (*pcVar9)(param_1[4],"/tmp/jiritsu/tmmc.jit");
                          if (cVar2 != '\0') {
                            iVar3 = _ZN10IM_JIRITSU10CImJiritsu3reqEjNS0_11IMJ_REQUESTEPKcS3_S3_j
                                              (param_1,*(undefined4 *)((long)param_1 + 0xc),2,
                                               "/tmp/jiritsu/ramdsk_libJit/libJit.so",
                                               "/tmp/jiritsu/tmmc.jit","_EMERGENCY()");
                            if (iVar3 != 0) {
                              pcVar7 = "send emergency run fail";
                              goto LAB_0010c300;
                            }
                            goto LAB_0010c3a4;
                          }
                          pcVar7 = *(char **)(param_1[4] + 0x38);
                          sVar8 = strlen(pcVar7);
                          _Z22jiritsu_cppdump_logrechPKcPhj
                                    (0xffffff81,"load emergency err",pcVar7,sVar8 & 0xffffffff);
                          pFVar11 = fopen("/tmp/jiritsu/jit.log","a");
                          if (pFVar11 != (FILE *)0x0) {
                            pcVar7 = "";
                            if (*(char **)(param_1[4] + 0x38) != (char *)0x0) {
                              pcVar7 = *(char **)(param_1[4] + 0x38);
                            }
                            fprintf(pFVar11,"jit_thread: load emergency error: %s\n",pcVar7);
                            fflush(pFVar11);
                            fclose(pFVar11);
                          }
                          UI_PrintString(0,0x800000ff,"Jiritsu load error.[Emergency]");
                          pcVar7 = (char *)0x0;
                          goto LAB_0010c628;
                        }
                      }
                      else {
                        FUN_0010b878(0,"[JIT] libJit_so method run LoadScriptI B. ");
                        cVar2 = (*pcVar9)(param_1[4],*(undefined8 *)(piVar1 + 4));
                        if (cVar2 != '\0') {
                          if (*(char *)(param_1 + 5) == 'J') {
                            if (*(char *)((long)param_1 + 0x29) != 'J') {
                              unlink("/tmp/jmode");
                            }
                            pcVar7 = strchr((char *)(param_1 + 5),0x3a);
                            if (pcVar7 != (char *)0x0) {
                              *(char **)(piVar1 + 6) = pcVar7 + 1;
                            }
                            iVar3 = _ZN10IM_JIRITSU10CImJiritsu3reqEjNS0_11IMJ_REQUESTEPKcS3_S3_j
                                              (param_1,*(undefined4 *)((long)param_1 + 0xc),2,
                                               "/tmp/jiritsu/ramdsk_libJit/libJit.so",
                                               "/tmp/jiritsu/tmmc.jit",*(undefined8 *)(piVar1 + 6));
                            if (iVar3 != 0) {
                              pcVar7 = "send run fail";
LAB_0010c300:
                              _Z18jiritsu_cpp_logrechPKc(0xffffff81,pcVar7);
                              uVar4 = 0x18;
                              goto LAB_0010befc;
                            }
                          }
                          _Z18jiritsu_cpp_logrechPKc(0xffffff84,"load done");
                          pcVar7 = "[JIT] libJit_so method run LoadScriptI E. ";
                          *(undefined4 *)(param_1 + 1) = 0xe;
                          goto LAB_0010c39c;
                        }
                        pcVar7 = "unknown";
                        if (*(char **)(param_1[4] + 0x38) != (char *)0x0) {
                          pcVar7 = *(char **)(param_1[4] + 0x38);
                        }
                        sVar8 = strlen(pcVar7);
                        _Z22jiritsu_cppdump_logrechPKcPhj
                                  (0xffffff81,"load err",pcVar7,sVar8 & 0xffffffff);
                        pFVar11 = fopen("/tmp/jiritsu/jit.log","a");
                        if (pFVar11 != (FILE *)0x0) {
                          fprintf(pFVar11,"jit_thread: load error: %s\n",pcVar7);
                          fflush(pFVar11);
                          fclose(pFVar11);
                        }
                        UI_PrintString(0,0x800000ff,"Jiritsu load error.");
LAB_0010c628:
                        UI_PrintString(1,0x800000ff,"%s",pcVar7);
                      }
                      uVar4 = 0x11;
                    }
                    else {
                      sVar8 = strlen(pcVar7);
                      _Z22jiritsu_cppdump_logrechPKcPhj
                                (0xffffff81,"sym load err pErr",pcVar7,sVar8 & 0xffffffff);
                      uVar4 = 0x13;
                    }
                  }
                }
                else {
                  sVar8 = strlen(pcVar7);
                  _Z22jiritsu_cppdump_logrechPKcPhj
                            (0xffffff81,"sym create err pErr",pcVar7,sVar8 & 0xffffffff);
                  uVar4 = 0x14;
                }
              }
            }
            else {
              _Z18jiritsu_cpp_logrechPKc(0xffffff81,"libJit.so copy error.");
              uVar4 = 0x15;
            }
          }
          else {
            _Z22jiritsu_cppdump_logrechPKcPhj(0xffffff81,"mount dir","/tmp/jiritsu",0xc);
LAB_0010be60:
            uVar4 = 0xb;
          }
        }
        else {
          _Z22jiritsu_cppdump_logrechPKcPhj(0xffffff81,"not dir","/tmp/jiritsu",0xc);
          uVar4 = 0xc;
        }
      }
      else {
        uVar4 = 2;
      }
LAB_0010befc:
      *(undefined4 *)(param_1 + 1) = uVar4;
    }
LAB_0010c3a4:
    osal_free_msg(param_2,local_2c08);
  } while( true );
}


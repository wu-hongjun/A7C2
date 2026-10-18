// Decompiled pseudocode, not original source.
// REA Evidence: ev_57294acea3a0e799077c15f80a97a6f706c2de0d009d2b27455985bd854ba5e8
// Source record: research/evidence/rea/avbbc-encoder-bridge/14-procedure_pseudo_code.json

undefined4
_ZN34ExecCmd_SDF_EncodeJpeg_Without_Pin6ResCmdEPKht(long param_1,long param_2,short param_3)

{
  undefined4 uVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  
  if (param_3 == -0x7cf0) {
    if (*(int *)(param_2 + 8) == 0) {
      uVar3 = 0x200;
      _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x1000b,0,0,0);
    }
    else {
      if (0 < avbizPrintLevel) {
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,1,0x79,
                   "[InfStillEncoder]ExecCmd_SDF_EncodeJpeg_Without_Pin Prm Err. error : 0x%x");
      }
      uVar3 = 0x201;
      _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d006,0x7a,0x1000a,*(undefined4 *)(param_2 + 8));
      (**(code **)(**(long **)(param_1 + 0x18) + 0x48))
                (*(long **)(param_1 + 0x18),0x104c,*(undefined4 *)(param_2 + 8));
    }
  }
  else if (param_3 == -0x7cef) {
    uVar2 = 0x1050;
    if (*(int *)(param_1 + 0x20) != 0x104f) {
      uVar2 = 0x1048;
    }
    (**(code **)(**(long **)(param_1 + 0x18) + 0xa8))(*(long **)(param_1 + 0x18),uVar2);
    if (*(int *)(param_2 + 8) == 0) {
      _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x201000c,0x8c,0,0);
      uVar3 = 0;
      uVar1 = *(undefined4 *)(param_2 + 0xc);
      if (*(char *)(param_1 + 0x24) == '\0') {
        uRam000000000000005c = *(undefined4 *)(param_2 + 0xc);
        uVar1 = uRam0000000000000068;
      }
    }
    else {
      if (0 < avbizPrintLevel) {
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,1,0x95,
                   "[InfStillEncoder]ExecCmd_SDF_EncodeJpeg_Without_Pin Prm Err. error : 0x%x",
                   *(int *)(param_2 + 8));
      }
      _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d006,0x96,0x1000a,*(undefined4 *)(param_2 + 8));
      (**(code **)(**(long **)(param_1 + 0x18) + 0x48))
                (*(long **)(param_1 + 0x18),0x104c,*(undefined4 *)(param_2 + 8));
      if (*(char *)(param_1 + 0x24) == '\0') {
        uRam000000000000005c = 0;
      }
      else {
        uRam0000000000000068 = 0;
      }
      uVar3 = 0x201;
      uVar1 = uRam0000000000000068;
    }
    uRam0000000000000068 = uVar1;
    _ZN13InfraAvbbcLog7SetLog2Ejjttttt
              (0x11,0x2010048,uRam0000000000000054,uRam0000000000000060,
               (undefined2)uRam000000000000005c,(undefined2)uRam0000000000000068,0);
  }
  else {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,1,0xa7,
                 "[InfStillEncoder]ExecCmd_SDF_EncodeJpeg_Without_Pin Err. cmnMid : 0x%x",param_3);
    }
    uVar3 = 1;
    _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d005,0xa8,0x1000a,param_3);
  }
  return uVar3;
}


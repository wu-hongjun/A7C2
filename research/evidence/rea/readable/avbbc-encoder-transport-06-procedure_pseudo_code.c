// Decompiled pseudocode, not original source.
// REA Evidence: ev_8ef606ad8695f9c2a172e9739100c195ef3121b5bbf8af74add5da325b929111
// Source record: research/evidence/rea/avbbc-encoder-transport/06-procedure_pseudo_code.json

undefined8 _ZN28ExecCmd_MC_StillEncOpenClose7ExecCmdEPKh(long param_1,long param_2)

{
  undefined2 uVar1;
  undefined4 uVar3;
  char *__assertion;
  uint uVar4;
  ulong uVar5;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  undefined8 local_10;
  undefined4 local_8;
  undefined2 uVar2;
  
  if (param_2 == 0) {
    _ZN17AvbbcBaseClassLog7SetLog3Ejjtjj(0x201e0ad,0x55,0x2a,0,0);
    __assertion = "p_msg";
    uVar4 = 0x2a;
LAB_002206f0:
                    /* WARNING: Subroutine does not return */
    __assert_fail(__assertion,"ExecCmd_MC_StillEncOpenClose.cpp",uVar4,
                  "virtual uint32_t ExecCmd_MC_StillEncOpenClose::ExecCmd(const uint8_t*)");
  }
  if (*(short *)(param_1 + 0x20) == 0x532) {
    local_18 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),5);
    local_16 = _ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt(*(undefined8 *)(param_1 + 0x10),5,0x532);
    local_14 = 0x10;
    local_12 = 1;
    local_10 = CONCAT44(local_10._4_4_,*(undefined4 *)(param_1 + 0x24));
    uVar3 = _ZN20InfraAvbbcMsgManager9GetOsalIdEv(*(undefined8 *)(param_1 + 0x10));
    uVar5 = local_10;
    local_10 = CONCAT44(uVar3,(undefined4)local_10);
    uVar5 = uVar5 & 0xffff;
    uVar4 = 0x51;
    uVar1 = local_18;
    uVar2 = local_16;
  }
  else {
    if (*(short *)(param_1 + 0x20) != 0x533) {
      _ZN17AvbbcBaseClassLog7SetLog3Ejjtjj(0x201e0ad,0x55,0x42,0,0);
      uVar4 = 0x42;
      __assertion = "__null";
      goto LAB_002206f0;
    }
    (**(code **)(**(long **)(param_1 + 0x18) + 0xa8))(*(long **)(param_1 + 0x18),0x1060);
    local_18 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),5);
    local_16 = _ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt(*(undefined8 *)(param_1 + 0x10),5,0x533);
    local_14 = 0x18;
    local_12 = 1;
    local_10 = 0;
    local_8 = _ZN20InfraAvbbcMsgManager9GetOsalIdEv(*(undefined8 *)(param_1 + 0x10));
    uVar5 = 0;
    uVar4 = 0x52;
    uVar1 = local_16;
    uVar2 = (undefined2)local_10;
  }
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt(0x11,uVar4 | 0x2010000,uVar1,uVar2,uVar5,0,0);
  _ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj
            (*(undefined8 *)(param_1 + 0x10),0x8057060a,&local_18,local_14);
  return 0;
}


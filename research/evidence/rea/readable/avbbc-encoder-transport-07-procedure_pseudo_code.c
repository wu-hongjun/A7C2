// Decompiled pseudocode, not original source.
// REA Evidence: ev_5dae537c96efd840dfd25ae99920fbf9db2c06bc3783cf315e185e8df44ab2c1
// Source record: research/evidence/rea/avbbc-encoder-transport/07-procedure_pseudo_code.json

undefined8 _ZN28ExecCmd_MC_StillEncOpenClose6ResCmdEPKht(long param_1,long param_2,short param_3)

{
  undefined8 uVar1;
  
  if (param_3 == -0x7a9d) {
    uVar1 = 0;
    if (*(int *)(param_2 + 0x10) == 0) {
      (**(code **)(**(long **)(param_1 + 0x18) + 0x50))
                (*(long **)(param_1 + 0x18),0x1060,*(undefined8 *)(param_2 + 8));
      _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x2010053,*(undefined4 *)(param_2 + 8),0,0);
    }
  }
  else if (param_3 == -0x7a9c) {
    uVar1 = 0;
    if (*(int *)(param_2 + 0x10) == 0) {
      _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x2010054,0,0,0);
      (**(code **)(**(long **)(param_1 + 0x18) + 0x40))(*(long **)(param_1 + 0x18),0x1060,0);
    }
  }
  else {
    uVar1 = 1;
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,4,1,0x5c,"[InfDecoder]ExecCmd_MC_Decode_Frame Err. cmnMid : 0x%x");
    }
  }
  return uVar1;
}


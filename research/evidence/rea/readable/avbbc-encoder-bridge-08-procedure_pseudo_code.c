// Decompiled pseudocode, not original source.
// REA Evidence: ev_d45ba0946d75ff175426f3f2d2caa9a1ed963693d68ab79c1d78ce31261e3121
// Source record: research/evidence/rea/avbbc-encoder-bridge/08-procedure_pseudo_code.json

void _ZN21ExecCmd_MC_EncodeHeifC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjhjb
               (undefined8 *param_1,undefined8 param_2,undefined8 param_3,undefined4 param_4,
               undefined4 param_5,undefined1 param_6,undefined4 param_7,undefined1 param_8)

{
  _ZN12ExecCmd_BaseC1EP20InfraAvbbcMsgManagerj(param_1,param_2,param_4);
  *(undefined1 *)((long)param_1 + 0x24) = param_6;
  param_1[3] = param_3;
  *(undefined4 *)(param_1 + 4) = param_5;
  *(undefined4 *)(param_1 + 5) = param_7;
  *(undefined1 *)((long)param_1 + 0x2c) = param_8;
  *param_1 = 0x2af6a0;
  return;
}


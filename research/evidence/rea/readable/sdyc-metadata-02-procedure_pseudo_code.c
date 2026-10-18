// Decompiled pseudocode, not original source.
// REA Evidence: ev_b7bf30774a85fbb1ce9d0773f80329e06a09100b14f92cba21a7d07cdf862a6d
// Source record: research/evidence/rea/sdyc-metadata/02-procedure_pseudo_code.json

void _ZN28C_DataflowInfra_StdData_BaseC2Emjj
               (undefined8 *param_1,undefined8 param_2,undefined4 param_3,undefined4 param_4)

{
  *param_1 = 0x158d20;
  _ZN29C_DataflowInfra_StdData_MakerC2Ev(param_1 + 1);
  _ZN30C_DataflowInfra_StdData_ParserC2Ev(param_1 + 2);
  param_1[5] = param_2;
  *(undefined4 *)(param_1 + 6) = param_4;
  *(undefined4 *)((long)param_1 + 0x34) = param_3;
  param_1[3] = 0;
  return;
}


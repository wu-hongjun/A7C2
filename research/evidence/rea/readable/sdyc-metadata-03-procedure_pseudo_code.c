// Decompiled pseudocode, not original source.
// REA Evidence: ev_6104d19d7bbd325912d2261388711a6242024257f683b91d858c9976ee8478fb
// Source record: research/evidence/rea/sdyc-metadata/03-procedure_pseudo_code.json

void _ZN26C_DataflowInfra_StdData_YCC1Emjj(undefined8 *param_1)

{
  _ZN28C_DataflowInfra_StdData_BaseC2Emjj();
  *param_1 = 0x158d58;
  param_1[3] = &DAT_0013aefd;
  *(undefined4 *)(param_1 + 4) = 1;
  *(undefined4 *)((long)param_1 + 0x24) = 1;
  _ZN28C_DataflowInfra_StdData_Base12CreateHeaderEv(param_1);
  return;
}


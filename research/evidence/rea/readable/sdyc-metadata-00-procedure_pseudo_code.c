// Decompiled pseudocode, not original source.
// REA Evidence: ev_487dcba2c44e40c8cd3f4ba6e1ae1a0e5059bb383e962b05e0a570f8251c879c
// Source record: research/evidence/rea/sdyc-metadata/00-procedure_pseudo_code.json

void _ZN28C_DataflowInfra_StdData_Base12CreateHeaderEv(long param_1)

{
  undefined8 local_10;
  long lStack_8;
  
  local_10 = *(undefined8 *)(param_1 + 0x18);
  lStack_8 = (ulong)*(uint *)(param_1 + 0x20) << 0x20;
  _ZN29C_DataflowInfra_StdData_Maker12CreateHeaderEmjPKNS_11HEADER_INFOE
            ((ulong)*(uint *)(param_1 + 0x30) + *(long *)(param_1 + 0x28),
             *(undefined4 *)(param_1 + 0x34),&local_10);
  return;
}


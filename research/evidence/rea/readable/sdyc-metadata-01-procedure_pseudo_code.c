// Decompiled pseudocode, not original source.
// REA Evidence: ev_73fe9ea37f3b926bdd33406af5b5b6c7d98a5e2f3b9bb4412cd4370cc08eaa42
// Source record: research/evidence/rea/sdyc-metadata/01-procedure_pseudo_code.json

undefined8
_ZN28C_DataflowInfra_StdData_Base12SetEntryDataEP26C_DataflowInfra_Entry_Base
          (long param_1,undefined8 param_2)

{
  undefined8 uVar1;
  undefined8 local_20;
  undefined4 local_18;
  undefined4 local_14;
  undefined8 local_10;
  undefined4 local_8;
  
  if (*(long *)(param_1 + 0x28) != 0) {
    local_20 = *(undefined8 *)(param_1 + 0x18);
    local_18 = _ZN26C_DataflowInfra_Entry_Base9getCateIDEv(param_2);
    local_14 = _ZN26C_DataflowInfra_Entry_Base10getVersionEv(param_2);
    local_10 = _ZN26C_DataflowInfra_Entry_Base6getAdrEv(param_2);
    local_8 = _ZN26C_DataflowInfra_Entry_Base7getSizeEv(param_2);
    uVar1 = _ZN29C_DataflowInfra_StdData_Maker12SetEntryDataEmjPKNS_15ENTRY_DATA_INFOE
                      ((ulong)*(uint *)(param_1 + 0x30) + *(long *)(param_1 + 0x28),
                       *(undefined4 *)(param_1 + 0x34),&local_20);
    return uVar1;
  }
  return 0xfffffff0;
}


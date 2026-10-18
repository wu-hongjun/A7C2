// Decompiled pseudocode, not original source.
// REA Evidence: ev_5926e898594f680f2d8e4f0a9694ae8f51efc36b7c8a9157294540b036d7112e
// Source record: research/evidence/rea/sdyc-entry-serializer/00-procedure_pseudo_code.json

undefined8
_ZN29C_DataflowInfra_StdData_Maker12CreateHeaderEmjPKNS_11HEADER_INFOE
          (undefined8 param_1,uint param_2,undefined8 *param_3)

{
  char cVar1;
  undefined8 uVar2;
  
  if (param_2 < 0x20) {
    uVar2 = 0xfffffff3;
  }
  else {
    cVar1 = _ZN30C_DataflowInfra_StdData_Parser9isStdDataEPK21DATA_STRUCTURE_HEADER();
    uVar2 = 0;
    if (cVar1 == '\0') {
      _ZN29C_DataflowInfra_StdData_Maker9SetMarkerEP21DATA_STRUCTURE_HEADERPKa(param_1,*param_3);
      _ZN29C_DataflowInfra_StdData_Maker11SetSerialIdEP21DATA_STRUCTURE_HEADERj
                (param_1,*(undefined4 *)(param_3 + 1));
      _ZN29C_DataflowInfra_StdData_Maker20SetDataFormatVersionEP21DATA_STRUCTURE_HEADERj
                (param_1,*(undefined4 *)((long)param_3 + 0xc));
      _ZN29C_DataflowInfra_StdData_Maker11SetEntrySumEP21DATA_STRUCTURE_HEADERj(param_1,0);
      _ZN29C_DataflowInfra_StdData_Maker19SetFirstEntryOffsetEP21DATA_STRUCTURE_HEADER(param_1);
    }
  }
  return uVar2;
}


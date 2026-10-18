// Decompiled pseudocode, not original source.
// REA Evidence: ev_cd5a7022aa7c5cddef061b56f99c0ec58ac7cc7be6ed6b3edfda4cb71b160e6c
// Source record: research/evidence/rea/sdyc-entry-serializer/01-procedure_pseudo_code.json

int _ZN29C_DataflowInfra_StdData_Maker12SetEntryDataEmjPKNS_15ENTRY_DATA_INFOE
              (long param_1,undefined8 param_2,undefined8 *param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  long lVar5;
  ulong uVar6;
  undefined8 uVar7;
  
  if (param_1 == 0) {
    return -0x10;
  }
  iVar1 = _ZN30C_DataflowInfra_StdData_Parser11CheckMarkerEPK21DATA_STRUCTURE_HEADERPKa
                    (param_1,*param_3);
  if (iVar1 != 0) {
    return iVar1;
  }
  uVar2 = _ZN30C_DataflowInfra_StdData_Parser19GetFirstEntryOffsetEPK21DATA_STRUCTURE_HEADER
                    (param_1);
  iVar1 = _ZN30C_DataflowInfra_StdData_Parser11GetEntrySumEPK21DATA_STRUCTURE_HEADER(param_1);
  if (iVar1 == 0) {
    lVar5 = param_1 + (ulong)uVar2;
    _ZN29C_DataflowInfra_StdData_Maker11SetEntrySumEP21DATA_STRUCTURE_HEADERj(param_1,1);
    _ZN29C_DataflowInfra_StdData_Maker19SetFirstEntryOffsetEP21DATA_STRUCTURE_HEADER(param_1);
    _ZN29C_DataflowInfra_StdData_Maker14SetEntryMarkerEP27DATA_STRUCTURE_ENTRY_HEADER(lVar5);
    _ZN29C_DataflowInfra_StdData_Maker14SetEntryCateIdEP27DATA_STRUCTURE_ENTRY_HEADERj
              (lVar5,*(undefined4 *)(param_3 + 1));
    _ZN29C_DataflowInfra_StdData_Maker19SetEntryCateVersionEP27DATA_STRUCTURE_ENTRY_HEADERj
              (lVar5,*(undefined4 *)((long)param_3 + 0xc));
  }
  else {
    lVar5 = _ZN30C_DataflowInfra_StdData_Parser17GetEntryHeaderAdrEPK21DATA_STRUCTURE_HEADERPKaj
                      (param_1,*param_3,*(undefined4 *)(param_3 + 1));
    if (lVar5 != 0) {
      if (*(uint *)(lVar5 + 0x10) < *(uint *)(param_3 + 3)) {
        return -0xd;
      }
      goto LAB_00134b64;
    }
    uVar7 = _ZN30C_DataflowInfra_StdData_Parser21GetLastEntryHeaderAdrEPK21DATA_STRUCTURE_HEADERPKa
                      (param_1,*param_3);
    iVar3 = _ZN30C_DataflowInfra_StdData_Parser18GetEntryDataOffsetEPK27DATA_STRUCTURE_ENTRY_HEADER
                      ();
    iVar4 = _ZN30C_DataflowInfra_StdData_Parser16GetEntryDataSizeEPK27DATA_STRUCTURE_ENTRY_HEADER
                      (uVar7);
    _ZN29C_DataflowInfra_StdData_Maker18SetNextEntryOffsetEP27DATA_STRUCTURE_ENTRY_HEADERj
              (uVar7,iVar3 + iVar4);
    _ZN29C_DataflowInfra_StdData_Maker11SetEntrySumEP21DATA_STRUCTURE_HEADERj(param_1,iVar1 + 1);
    uVar2 = _ZN30C_DataflowInfra_StdData_Parser18GetNextEntryOffsetEPK27DATA_STRUCTURE_ENTRY_HEADER
                      (uVar7);
    lVar5 = param_1 + (ulong)uVar2;
    _ZN29C_DataflowInfra_StdData_Maker14SetEntryMarkerEP27DATA_STRUCTURE_ENTRY_HEADER(lVar5);
    _ZN29C_DataflowInfra_StdData_Maker14SetEntryCateIdEP27DATA_STRUCTURE_ENTRY_HEADERj
              (lVar5,*(undefined4 *)(param_3 + 1));
    _ZN29C_DataflowInfra_StdData_Maker19SetEntryCateVersionEP27DATA_STRUCTURE_ENTRY_HEADERj
              (lVar5,*(undefined4 *)((long)param_3 + 0xc));
  }
  _ZN29C_DataflowInfra_StdData_Maker18SetEntryDataOffsetEP27DATA_STRUCTURE_ENTRY_HEADERj
            (lVar5,uVar2 + 0x20);
  _ZN29C_DataflowInfra_StdData_Maker16SetEntryDataSizeEP27DATA_STRUCTURE_ENTRY_HEADERj
            (lVar5,*(undefined4 *)(param_3 + 3));
  _ZN29C_DataflowInfra_StdData_Maker18SetNextEntryOffsetEP27DATA_STRUCTURE_ENTRY_HEADERj(lVar5,0);
LAB_00134b64:
  uVar6 = _ZN30C_DataflowInfra_StdData_Parser18GetEntryDataOffsetEPK27DATA_STRUCTURE_ENTRY_HEADER
                    (lVar5);
  memcpy((void *)(param_1 + (uVar6 & 0xffffffff)),(void *)param_3[2],(ulong)*(uint *)(param_3 + 3));
  return 0;
}


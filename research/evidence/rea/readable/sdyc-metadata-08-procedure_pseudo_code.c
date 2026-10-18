// Decompiled pseudocode, not original source.
// REA Evidence: ev_4ff5a9dc3aadc87f6c206dcaa0d5621ccb7708b059dcb7a586bbbb1c8ff36173
// Source record: research/evidence/rea/sdyc-metadata/08-procedure_pseudo_code.json

void _ZN37C_DataflowInfra_Entry_ImageOutTimInfo11SetData_AllEPN19DataflowInfra_StdYc25PIN_YC_IMAGE_OUT_TIM_INFOE
               (long param_1,undefined8 *param_2)

{
  undefined8 uVar1;
  
  uVar1 = param_2[1];
  *(undefined8 *)(param_1 + 0x1c) = *param_2;
  *(undefined8 *)(param_1 + 0x24) = uVar1;
  uVar1 = param_2[3];
  *(undefined8 *)(param_1 + 0x2c) = param_2[2];
  *(undefined8 *)(param_1 + 0x34) = uVar1;
  uVar1 = param_2[5];
  *(undefined8 *)(param_1 + 0x3c) = param_2[4];
  *(undefined8 *)(param_1 + 0x44) = uVar1;
  uVar1 = param_2[7];
  *(undefined8 *)(param_1 + 0x4c) = param_2[6];
  *(undefined8 *)(param_1 + 0x54) = uVar1;
  return;
}


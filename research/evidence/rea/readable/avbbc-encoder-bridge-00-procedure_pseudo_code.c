// Decompiled pseudocode, not original source.
// REA Evidence: ev_1d7464a6dea7cf1feb734d64a2c94f6c34af2d149cbf23a4cf287476efe05eb5
// Source record: research/evidence/rea/avbbc-encoder-bridge/00-procedure_pseudo_code.json

void _ZN26ExecCmd_SDF_EncodeJpegMain18MakeInputYcStdDataEPN17StillEncoderParam23RESIZEENC_PARAM_PAYLOADE
               (undefined8 param_1,long param_2)

{
  uint uVar1;
  uint6 uVar2;
  undefined8 uVar3;
  undefined1 uVar4;
  undefined8 *puVar5;
  undefined8 uVar6;
  undefined8 local_1f0;
  undefined1 auStack_1e8 [40];
  undefined1 auStack_1c0 [56];
  undefined1 auStack_188 [56];
  undefined8 local_150;
  undefined8 uStack_148;
  undefined8 local_140;
  undefined8 local_138;
  undefined8 local_130;
  undefined8 uStack_128;
  undefined8 local_120;
  undefined8 uStack_118;
  undefined8 local_110;
  undefined8 uStack_108;
  undefined8 local_100;
  undefined8 local_f8;
  undefined8 local_f0;
  long lStack_e8;
  undefined8 local_e0;
  long local_d8;
  ulong local_d0;
  undefined1 auStack_c8 [96];
  undefined1 auStack_68 [104];
  
  uStack_108 = 0;
  local_100 = 0;
  local_f0 = 0;
  local_110 = *(undefined8 *)(param_2 + 0x30);
  local_f8 = (ulong)CONCAT14(1,(int)*(undefined8 *)(param_2 + 0x48));
  lStack_e8 = (ulong)*(ushort *)(param_2 + 0x54) << 0x30;
  local_e0 = *(undefined8 *)(param_2 + 0x56);
  local_d8 = (ulong)*(ushort *)(param_2 + 0x5c) << 0x30;
  local_d8 = CONCAT62(local_d8._2_6_,*(ushort *)(param_2 + 0x5e));
  local_d0 = (ulong)*(ushort *)(param_2 + 0x5e);
  uVar4 = _ZN9DefCommon9getAspectEtt();
  uVar6 = local_f0;
  local_f8._0_7_ = CONCAT16(uVar4,(undefined6)local_f8);
  local_f0 = CONCAT35(local_f0._5_3_,0x200010000);
  uVar3 = local_f0;
  local_f0._0_2_ = CONCAT11(*(char *)(param_2 + 99),(char)uVar6);
  uVar6 = local_f0;
  local_f0._4_4_ = SUB84(uVar3,4);
  local_f0._0_4_ = CONCAT13(1,(int3)uVar6);
  local_1f0._0_2_ = CONCAT11(*(undefined1 *)(param_2 + 0x61),*(char *)(param_2 + 0x60) == '\0');
  uVar1 = CONCAT22(0x100,(undefined2)local_1f0);
  local_1f0._0_6_ = CONCAT15(4,(uint5)uVar1);
  uVar2 = (uint6)local_1f0;
  local_1f0 = (ulong)(uint6)local_1f0;
  if (*(char *)(param_2 + 99) != '\0') {
    local_1f0 = (ulong)CONCAT16(1,uVar2);
  }
  uStack_148 = 0;
  local_140 = 0;
  local_130 = 0;
  uStack_128 = 0;
  local_120 = 0;
  uStack_118 = 0;
  local_150 = 0xffffffff;
  local_138 = 0xffffffff;
  _ZN35C_DataflowInfra_Entry_ImageSizeInfoC1Ev(auStack_68);
  _ZN33C_DataflowInfra_Entry_YcStateInfoC1Ev(auStack_1e8);
  _ZN37C_DataflowInfra_Entry_ImageOutTimInfoC1Ev(auStack_c8);
  _ZN35C_DataflowInfra_Entry_ImageSizeInfo11SetData_AllEPN19DataflowInfra_StdYc22PIN_YC_IMAGE_SIZE_INFOE
            (auStack_68,&local_110);
  _ZN33C_DataflowInfra_Entry_YcStateInfo11SetData_AllEPN19DataflowInfra_StdYc20PIN_YC_YC_STATE_INFOE
            (auStack_1e8,&local_1f0);
  _ZN37C_DataflowInfra_Entry_ImageOutTimInfo11SetData_AllEPN19DataflowInfra_StdYc25PIN_YC_IMAGE_OUT_TIM_INFOE
            (auStack_c8,&local_150);
  _ZN12AvbbcPhycMemC2EmjNS_10CACHE_MODEEj
            (auStack_1c0,*(long *)(param_2 + 0x18) + *(long *)(param_2 + 0x20),0x400,0,0x29);
  puVar5 = (undefined8 *)_ZN12AvbbcPhycMem10GetLogAddrEv(auStack_1c0);
  *puVar5 = 0;
  puVar5[1] = 0;
  puVar5[2] = 0;
  puVar5[3] = 0;
  uVar6 = _ZN12AvbbcPhycMem10GetLogAddrEv(auStack_1c0);
  _ZN26C_DataflowInfra_StdData_YCC1Emjj(auStack_188,uVar6,0x400,0);
  _ZN26C_DataflowInfra_StdData_YC22SetEntry_ImageSizeInfoEP35C_DataflowInfra_Entry_ImageSizeInfo
            (auStack_188,auStack_68);
  _ZN26C_DataflowInfra_StdData_YC20SetEntry_YcStateInfoEP33C_DataflowInfra_Entry_YcStateInfo
            (auStack_188,auStack_1e8);
  _ZN26C_DataflowInfra_StdData_YC24SetEntry_ImageOutTimInfoEP37C_DataflowInfra_Entry_ImageOutTimInfo
            (auStack_188,auStack_c8);
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x205,"[InfStillEncoder][INPUT_YC_STD_DATA]-----START-----");
  }
  _ZN26ExecCmd_SDF_EncodeJpegMain14PrintYcStdDataEP26C_DataflowInfra_StdData_YC(param_1,auStack_188)
  ;
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x207,"[InfStillEncoder][INPUT_YC_STD_DATA]------END------");
  }
  _ZN28C_DataflowInfra_StdData_Base11SetAddrInfoEmjj(auStack_188,0,0,0);
  _ZN26C_DataflowInfra_StdData_YCD1Ev(auStack_188);
  _ZN12AvbbcPhycMemD2Ev(auStack_1c0);
  _ZN37C_DataflowInfra_Entry_ImageOutTimInfoD1Ev(auStack_c8);
  _ZN33C_DataflowInfra_Entry_YcStateInfoD1Ev(auStack_1e8);
  _ZN35C_DataflowInfra_Entry_ImageSizeInfoD1Ev(auStack_68);
  return;
}


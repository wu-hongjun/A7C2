// Decompiled pseudocode, not original source.
// REA Evidence: ev_8118aec7730f246795128ff128a8b28623f322fd74f17a80d6a4796df8359a87
// Source record: research/evidence/rea/avbbc-encoder-bridge/12-procedure_pseudo_code.json

undefined8
_ZN34ExecCmd_SDF_EncodeJpeg_Without_Pin19makeParamEncodeJpegERN3SDF25ENCODE_JPEG_COMMAND_PARAME
          (long param_1,long param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  long lVar6;
  undefined1 uVar7;
  undefined2 uVar8;
  char local_11;
  long local_10;
  undefined4 *local_8;
  
  local_10 = 0;
  _ZN24ExcludionEnvironmentUtil21ClearCommonMemFmtDataEPN14AvbbcParamCtrl16ParameterControlEj
            (*(undefined8 *)(param_1 + 0x18),0x1054);
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))
            (*(long **)(param_1 + 0x18),*(undefined4 *)(param_1 + 0x20),&local_10);
  lVar6 = local_10;
  (**(code **)(**(long **)(param_1 + 0x18) + 0x80))(*(long **)(param_1 + 0x18),0x1049,&local_11);
  *(undefined2 *)(param_2 + 8) = 7;
  uVar8 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  *(undefined2 *)(param_2 + 0xc) = uVar8;
  *(undefined2 *)(param_2 + 0xe) = 0x8310;
  uVar8 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  *(undefined2 *)(param_2 + 0x10) = uVar8;
  *(undefined2 *)(param_2 + 0x12) = 0x8311;
  local_8 = (undefined4 *)0x0;
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))(*(long **)(param_1 + 0x18),0x1053);
  *(undefined4 *)(param_2 + 0x18) = *local_8;
  uVar1 = local_8[1];
  uVar2 = local_8[3];
  *(undefined1 *)(param_2 + 0x14) = 0;
  *(undefined4 *)(param_2 + 0x1c) = uVar1;
  *(undefined4 *)(param_2 + 0x20) = uVar2;
  cVar5 = *(char *)(param_1 + 0x24);
  *(undefined4 *)(param_2 + 0x24) = 0;
  if (cVar5 == '\0') {
    iVar3 = *(int *)(lVar6 + 0x5c);
    *(undefined4 *)(param_2 + 0x28) = *(undefined4 *)(lVar6 + 0x54);
    *(undefined4 *)(param_2 + 0x2c) = 0;
    iVar4 = *(int *)(lVar6 + 0x58);
    *(undefined1 *)(param_2 + 0x4b) = 0;
    *(int *)(param_2 + 0x38) = iVar4;
    *(int *)(param_2 + 0x3c) = iVar3;
    *(int *)(param_2 + 0x34) = iVar4 + iVar3;
    uVar7 = 3;
  }
  else {
    iVar3 = *(int *)(lVar6 + 0x68);
    *(undefined4 *)(param_2 + 0x28) = *(undefined4 *)(lVar6 + 0x60);
    *(undefined4 *)(param_2 + 0x2c) = 0;
    iVar4 = *(int *)(lVar6 + 100);
    *(int *)(param_2 + 0x38) = iVar4;
    *(int *)(param_2 + 0x3c) = iVar3;
    *(int *)(param_2 + 0x34) = iVar4 + iVar3;
    *(undefined1 *)(param_2 + 0x4b) = 1;
    uVar7 = 4;
  }
  *(undefined1 *)(param_2 + 0x49) = uVar7;
  uVar8 = 0x550;
  *(undefined2 *)(param_2 + 0x40) = *(undefined2 *)(param_1 + 0x26);
  *(undefined2 *)(param_2 + 0x42) = *(undefined2 *)(param_1 + 0x28);
  if (local_11 != '\x05') {
    uVar8 = 0x500;
  }
  *(undefined2 *)(param_2 + 0x46) = 0x2d0;
  *(undefined2 *)(param_2 + 0x44) = uVar8;
  if (local_11 == '@') {
    local_11 = '\x02';
  }
  *(undefined1 *)(param_2 + 0x48) = 0;
  *(char *)(param_2 + 0x4a) = local_11;
  *(undefined1 *)(param_2 + 0x4c) = 0;
  *(undefined1 *)(param_2 + 0x4d) = 0;
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x10b,"[InfStillEncoder][OUTPUT_YC_STD_DATA]-----START-----");
  }
  _ZN34ExecCmd_SDF_EncodeJpeg_Without_Pin20PrintEncodeJpegParamEPKN3SDF25ENCODE_JPEG_COMMAND_PARAME
            (param_1,param_2);
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x10d,"[InfStillEncoder][OUTPUT_YC_STD_DATA]------END------");
  }
  return 0;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_68b1a6c0f3e338e0cc1acfe193315e118d5846c387965f59615c5a4a0237eaa3
// Source record: research/evidence/rea/avbbc-encoder-bridge/01-procedure_pseudo_code.json

undefined8
_ZN26ExecCmd_SDF_EncodeJpegMain23makeParamEncodeJpegMainERN3SDF25ENCODE_JPEG_COMMAND_PARAME
          (long param_1,long param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  long lVar5;
  long lVar6;
  undefined1 uVar7;
  undefined2 uVar8;
  long local_10;
  long local_8;
  
  local_10 = 0;
  local_8 = 0;
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))(*(long **)(param_1 + 0x18),0x1047,&local_10);
  lVar5 = local_10;
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))(*(long **)(param_1 + 0x18),0x104a,&local_8);
  lVar6 = local_8;
  *(undefined2 *)(param_2 + 8) = 7;
  uVar8 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  *(undefined2 *)(param_2 + 0xc) = uVar8;
  *(undefined2 *)(param_2 + 0xe) = 0x8310;
  uVar8 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  *(undefined1 *)(param_2 + 0x14) = 0;
  *(undefined2 *)(param_2 + 0x10) = uVar8;
  *(undefined2 *)(param_2 + 0x12) = 0x8311;
  *(undefined4 *)(param_2 + 0x18) = *(undefined4 *)(lVar6 + 8);
  *(undefined4 *)(param_2 + 0x1c) = *(undefined4 *)(lVar6 + 0xc);
  *(int *)(param_2 + 0x20) = (int)*(undefined8 *)(lVar6 + 0x20);
  _ZN26ExecCmd_SDF_EncodeJpegMain18MakeInputYcStdDataEPN17StillEncoderParam23RESIZEENC_PARAM_PAYLOADE
            (param_1,lVar6);
  uVar1 = *(undefined4 *)(lVar5 + 0x54);
  iVar2 = *(int *)(lVar5 + 0x5c);
  *(undefined4 *)(param_2 + 0x24) = 0;
  *(undefined4 *)(param_2 + 0x28) = uVar1;
  iVar3 = *(int *)(lVar5 + 0x58);
  *(undefined4 *)(param_2 + 0x2c) = 0;
  *(int *)(param_2 + 0x38) = iVar3;
  *(int *)(param_2 + 0x3c) = iVar2;
  uVar8 = *(undefined2 *)(lVar6 + 0x6a);
  *(int *)(param_2 + 0x34) = iVar3 + iVar2;
  uVar4 = *(undefined2 *)(lVar6 + 0x68);
  *(undefined2 *)(param_2 + 0x42) = uVar8;
  *(undefined2 *)(param_2 + 0x40) = uVar4;
  *(undefined2 *)(param_2 + 0x44) = uVar4;
  *(undefined2 *)(param_2 + 0x46) = uVar8;
  *(undefined1 *)(param_2 + 0x48) = 0;
  switch(*(undefined1 *)(lVar6 + 0x70)) {
  case 0:
    *(undefined1 *)(param_2 + 0x49) = 0;
    goto LAB_00221cb8;
  case 1:
    uVar7 = 1;
    break;
  default:
    uVar7 = 2;
    break;
  case 3:
    uVar7 = 6;
  }
  *(undefined1 *)(param_2 + 0x49) = uVar7;
LAB_00221cb8:
  uVar7 = _ZN9DefCommon9getAspectEtt();
  *(undefined1 *)(param_2 + 0x4b) = 0;
  *(undefined1 *)(param_2 + 0x4a) = uVar7;
  *(undefined1 *)(param_2 + 0x4c) = *(undefined1 *)(lVar6 + 0x71);
  *(undefined1 *)(param_2 + 0x4d) = *(undefined1 *)(lVar6 + 0x72);
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xdf,
               "[InfStillEncoder]================== JpgInfo Param [MainJpeg] ============");
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe0,"[InfStillEncoder]cmdId                 :0x%08x",
               *(undefined2 *)(param_2 + 8));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe1,"[InfStillEncoder]streamArea.handle     :0x%08x",
               *(undefined4 *)(param_2 + 0x28));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe2,"[InfStillEncoder]streamArea.dataOffset :0x%08x",
               *(undefined4 *)(param_2 + 0x2c));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe3,"[InfStillEncoder]streamAreaLength      :0x%08x",
               *(undefined4 *)(param_2 + 0x34));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe4,"[InfStillEncoder]streamValidLength     :0x%08x",
               *(undefined4 *)(param_2 + 0x3c));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe5,"[InfStillEncoder]ImageSizeX            :0x%08x",
               *(undefined2 *)(param_2 + 0x40));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe6,"[InfStillEncoder]ImageSizeY            :0x%08x",
               *(undefined2 *)(param_2 + 0x42));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe7,"[InfStillEncoder]PrimaryImageSizeX     :0x%08x",
               *(undefined2 *)(param_2 + 0x44));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe8,"[InfStillEncoder]PrimaryImageSizeY     :0x%08x",
               *(undefined2 *)(param_2 + 0x46));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe9,
               "[InfStillEncoder]================= JpgInfo Param end ====================");
  }
  _ZN13InfraAvbbcLog7SetLog1Ejjjttt
            (0x11,0x1000d,*(undefined2 *)(param_2 + 8),*(undefined2 *)(param_2 + 0x28),
             *(undefined2 *)(param_2 + 0x2c),0);
  _ZN13InfraAvbbcLog7SetLog0Ejjjjt
            (0x11,0x1000e,*(undefined4 *)(param_2 + 0x34),*(undefined4 *)(param_2 + 0x3c),0);
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt
            (0x11,0x201003a,*(undefined2 *)(param_2 + 0x40),*(undefined2 *)(param_2 + 0x42),
             *(undefined2 *)(param_2 + 0x44),*(undefined2 *)(param_2 + 0x46),
             CONCAT11(*(undefined1 *)(param_2 + 0x49),*(undefined1 *)(param_2 + 0x4d)));
  return 0;
}


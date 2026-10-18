// Decompiled pseudocode, not original source.
// REA Evidence: ev_3dc64dcdc9c00bd7acb916d8f2ce2aaec6a9dd29d9d542622199ad2831d4d7d5
// Source record: research/evidence/rea/avbbc-encoder-bridge/09-procedure_pseudo_code.json

undefined8
_ZN22ExecCmd_SDF_EncodeJpeg19makeParamEncodeJpegERN3SDF25ENCODE_JPEG_COMMAND_PARAME
          (long param_1,long param_2)

{
  int iVar1;
  int iVar2;
  byte bVar3;
  long lVar4;
  bool bVar5;
  undefined2 uVar6;
  undefined1 local_9;
  long local_8;
  
  local_8 = 0;
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))
            (*(long **)(param_1 + 0x18),*(undefined4 *)(param_1 + 0x20));
  lVar4 = local_8;
  (**(code **)(**(long **)(param_1 + 0x18) + 0x80))
            (*(long **)(param_1 + 0x18),*(undefined4 *)(param_1 + 0x24),&local_9);
  *(undefined2 *)(param_2 + 8) = 7;
  uVar6 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  *(undefined2 *)(param_2 + 0xc) = uVar6;
  *(undefined2 *)(param_2 + 0xe) = 0x8310;
  uVar6 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  *(undefined2 *)(param_2 + 0x10) = uVar6;
  *(undefined2 *)(param_2 + 0x12) = 0x8311;
  *(undefined1 *)(param_2 + 0x14) = *(undefined1 *)(param_1 + 0x28);
  bVar3 = *(byte *)(param_1 + 0x29);
  *(byte *)(param_2 + 0x16) = bVar3;
  *(ushort *)(param_2 + 10) = (ushort)bVar3;
  iVar1 = *(int *)(lVar4 + 0x54);
  *(undefined4 *)(param_2 + 0x24) = 0;
  *(undefined4 *)(param_2 + 0x2c) = 0;
  if (iVar1 == 0) {
    iVar1 = *(int *)(lVar4 + 0x68);
    *(undefined4 *)(param_2 + 0x28) = *(undefined4 *)(lVar4 + 0x60);
    iVar2 = *(int *)(lVar4 + 100);
    *(int *)(param_2 + 0x38) = iVar2;
    *(int *)(param_2 + 0x3c) = iVar1;
    *(int *)(param_2 + 0x34) = iVar2 + iVar1;
    *(undefined1 *)(param_2 + 0x4b) = 1;
  }
  else {
    iVar2 = *(int *)(lVar4 + 0x5c);
    *(int *)(param_2 + 0x28) = iVar1;
    iVar1 = *(int *)(lVar4 + 0x58);
    *(undefined1 *)(param_2 + 0x4b) = 0;
    *(int *)(param_2 + 0x38) = iVar1;
    *(int *)(param_2 + 0x3c) = iVar2;
    *(int *)(param_2 + 0x34) = iVar1 + iVar2;
  }
  *(undefined1 *)(param_2 + 0x48) = 0;
  *(undefined1 *)(param_2 + 0x4c) = 0;
  *(undefined2 *)(param_2 + 0x40) = 0xa0;
  *(undefined1 *)(param_2 + 0x4d) = 0;
  *(undefined2 *)(param_2 + 0x42) = 0x78;
  *(undefined2 *)(param_2 + 0x44) = 0x780;
  *(undefined2 *)(param_2 + 0x46) = 0x438;
  *(undefined1 *)(param_2 + 0x49) = 4;
  *(undefined1 *)(param_2 + 0x4a) = local_9;
  bVar5 = *(char *)(param_1 + 0x2a) != '\0';
  *(bool *)(param_2 + 0x4e) = bVar5;
  _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201004b,0xcd,bVar5,*(undefined2 *)(param_2 + 6));
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd0,
               "[InfStillEncoder]================== JpgInfo Param(Thumnail) =============");
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd1,"[InfStillEncoder]cmdId                 :0x%08x",
               *(undefined2 *)(param_2 + 8));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd2,"[InfStillEncoder]ackCateId             :0x%08x",
               *(undefined2 *)(param_2 + 0xc));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd3,"[InfStillEncoder]ackMsgId              :0x%08x",
               *(undefined2 *)(param_2 + 0xe));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd4,"[InfStillEncoder]cmpCateId             :0x%08x",
               *(undefined2 *)(param_2 + 0x10));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd5,"[InfStillEncoder]pinConnectIn          :0x%08x",
               *(undefined1 *)(param_2 + 0x14));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd6,"[InfStillEncoder]pinNo         \t\t  :0x%08x",
               *(undefined1 *)(param_2 + 0x16));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd7,"[InfStillEncoder]streamArea.handle     :0x%08x",
               *(undefined4 *)(param_2 + 0x28));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd8,"[InfStillEncoder]streamArea.dataOffset :0x%08x",
               *(undefined4 *)(param_2 + 0x2c));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xd9,"[InfStillEncoder]streamAreaLength      :0x%08x",
               *(undefined4 *)(param_2 + 0x34));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xda,"[InfStillEncoder]streamOffset          :0x%08x",
               *(undefined4 *)(param_2 + 0x38));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xdb,"[InfStillEncoder]streamValidLength     :0x%08x",
               *(undefined4 *)(param_2 + 0x3c));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xdc,"[InfStillEncoder]jpegImageSizeX        :0x%08x",
               *(undefined2 *)(param_2 + 0x40));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xdd,"[InfStillEncoder]jpegImageSizeY        :0x%08x",
               *(undefined2 *)(param_2 + 0x42));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xde,"[InfStillEncoder]primaryImageSizeX     :0x%08x",
               *(undefined2 *)(param_2 + 0x44));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xdf,"[InfStillEncoder]primaryImageSizeY     :0x%08x",
               *(undefined2 *)(param_2 + 0x46));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe0,"[InfStillEncoder]sameAreaProcess       :0x%08x",
               *(undefined1 *)(param_2 + 0x48));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe1,"[InfStillEncoder]encodeQuality         :0x%08x",
               *(undefined1 *)(param_2 + 0x49));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe2,"[InfStillEncoder]outputImageAspect     :0x%08x",
               *(undefined1 *)(param_2 + 0x4a));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe3,"[InfStillEncoder]addSoi                :0x%08x",
               *(undefined1 *)(param_2 + 0x4b));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe4,"[InfStillEncoder]outputYRange          :0x%08x",
               *(undefined1 *)(param_2 + 0x4c));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe5,"[InfStillEncoder]outputYuvColorSpace   :0x%08x",
               *(undefined1 *)(param_2 + 0x4d));
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0xe6,
               "[InfStillEncoder]================= JpgInfo Param end ====================");
  }
  _ZN13InfraAvbbcLog7SetLog1Ejjjttt
            (0x11,0x1000d,*(undefined4 *)(param_2 + 0x28),*(undefined2 *)(param_2 + 8),
             *(undefined2 *)(param_2 + 0x2c),0);
  _ZN13InfraAvbbcLog7SetLog0Ejjjjt
            (0x11,0x1000e,*(undefined4 *)(param_2 + 0x34),*(undefined4 *)(param_2 + 0x3c),0);
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt
            (0x11,0x201003a,*(undefined2 *)(param_2 + 0x40),*(undefined2 *)(param_2 + 0x42),
             *(undefined2 *)(param_2 + 0x44),*(undefined2 *)(param_2 + 0x46),
             CONCAT11(*(undefined1 *)(param_2 + 0x49),*(undefined1 *)(param_2 + 0x4a)));
  return 0;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_11d94a2862655df60369a187bb9e66aa98236d1e7b0cceb65737c90f4fdcca37
// Source record: research/evidence/rea/avbbc-encoder-bridge/02-procedure_pseudo_code.json

undefined8 _ZN26ExecCmd_SDF_EncodeJpegMain7ExecCmdEPKh(long param_1,long param_2)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  undefined2 local_150;
  undefined2 local_14e;
  undefined2 local_14c;
  undefined2 local_14a;
  undefined2 local_148;
  undefined2 local_146;
  undefined2 local_144;
  undefined2 local_142;
  undefined2 local_140;
  undefined2 local_13e;
  undefined1 local_13c;
  undefined1 local_13b;
  undefined1 local_13a;
  undefined1 local_139;
  undefined4 local_138;
  undefined4 uStack_134;
  undefined4 local_130;
  undefined4 uStack_12c;
  undefined4 local_128;
  undefined4 uStack_124;
  undefined4 local_120;
  undefined4 uStack_11c;
  undefined4 local_118;
  undefined4 uStack_114;
  undefined2 local_110;
  undefined2 local_10e;
  undefined2 local_10c;
  undefined2 local_10a;
  undefined1 local_108;
  undefined1 local_107;
  undefined1 local_106;
  undefined1 local_105;
  undefined1 local_104;
  undefined1 local_103;
  undefined1 local_102;
  undefined1 local_101;
  undefined2 local_100;
  undefined2 local_fe;
  undefined2 local_fc;
  undefined2 local_fa;
  undefined1 auStack_f8 [248];
  
  local_100 = 0;
  local_fe = 0;
  local_fc = 0;
  local_fa = 0;
  memset(auStack_f8,0,0xf8);
  local_150 = 0;
  local_14e = 0;
  local_14c = 0;
  local_14a = 0;
  local_148 = 0;
  local_146 = 0;
  local_144 = 0;
  local_142 = 0;
  local_140 = 0;
  local_13e = 0;
  local_13c = 0;
  local_13b = 0;
  local_13a = 0;
  local_139 = 0;
  local_110 = 0;
  local_10e = 0;
  local_10c = 0;
  local_10a = 0;
  local_108 = 0;
  local_107 = 0;
  local_106 = 0;
  local_105 = 0;
  local_138 = 0;
  uStack_134 = 0;
  local_130 = 0;
  uStack_12c = 0;
  local_128 = 0;
  uStack_124 = 0;
  local_120 = 0;
  uStack_11c = 0;
  local_118 = 0;
  uStack_114 = 0;
  local_104 = 0;
  local_103 = 0;
  local_102 = 0;
  local_101 = 0;
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x3d,"[InfStillEncoder][SeqResizeEnc] Execute Encode Jpg (Main Jpeg)");
  }
  local_100 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  local_fe = _ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt(*(undefined8 *)(param_1 + 0x10),3,0x304);
  local_fc = 0x50;
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x46,"[InfStillEncoder]catId    :0x%08x",local_100);
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x47,"[InfStillEncoder]msgId    :0x%08x",local_fe);
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x48,"[InfStillEncoder]msgLen   :0x%08x",local_fc);
  }
  if (*(short *)(param_2 + 2) != 0) {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x4c,"[InfStillEncoder][SeqResizeEnc][ERR] DMM_OPEN_UNIT COMP err = 0x%x")
      ;
    }
    _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d004,0x4d,(long)*(short *)(param_2 + 2),0);
    return 1;
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x51,"[InfStillEncoder][SeqResizeEnc]DMM unit Phy Addr = 0x%x",
               *(undefined8 *)(param_2 + 0x18));
  }
  cVar1 = *(char *)(param_1 + 0x20);
  if (cVar1 == '\x01') {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x5a,"[InfStillEncoder]MakeParam(ThmJpeg)\n");
    }
    iVar3 = _ZN26ExecCmd_SDF_EncodeJpegMain22makeParamEncodeJpegThmERN3SDF25ENCODE_JPEG_COMMAND_PARAME
                      (param_1,&local_150);
  }
  else if (cVar1 == '\0') {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x56,"[InfStillEncoder]MakeParam(MainJpeg)\n");
    }
    iVar3 = _ZN26ExecCmd_SDF_EncodeJpegMain23makeParamEncodeJpegMainERN3SDF25ENCODE_JPEG_COMMAND_PARAME
                      (param_1,&local_150);
  }
  else {
    if (cVar1 != '\x02') {
      iVar3 = 1;
      if (0 < avbizPrintLevel) {
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,3,0x62,"[InfStillEncoder]MakeParam(NO TARGET!)\n");
      }
      goto LAB_00222994;
    }
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x5e,"[InfStillEncoder]MakeParam(ScnJpeg)\n");
    }
    iVar3 = _ZN26ExecCmd_SDF_EncodeJpegMain22makeParamEncodeJpegScnERN3SDF25ENCODE_JPEG_COMMAND_PARAME
                      (param_1,&local_150);
  }
  if (iVar3 == 0) {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x70,"[InfStillEncoder]ExecCmd_SDF_EncodeJpegMain ExecCmd. Msg = %p",
                 &local_100);
    }
    _ZN13InfraAvbbcLog7SetLog2Ejjttttt(0x11,0x1000a,local_100,local_fe,0,0,0);
    memcpy(auStack_f8,&local_148,0x48);
    _ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj
              (*(undefined8 *)(param_1 + 0x10),0x82830752,&local_100,local_fc);
    return 0;
  }
LAB_00222994:
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x67,"[InfStillEncoder]ExecCmd_SDF_EncodeJpegMain makeParam err = 0x%x",
               iVar3);
  }
  _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d004,0x68,(long)*(short *)(param_2 + 2),0);
  uVar2 = _ZN22InfraAvbbcErrConverter7ConvertEjj(8,0x120);
  (**(code **)(**(long **)(param_1 + 0x18) + 0x48))(*(long **)(param_1 + 0x18),0x1043,uVar2);
  return 0x201;
}


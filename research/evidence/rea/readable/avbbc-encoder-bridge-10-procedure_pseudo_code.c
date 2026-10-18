// Decompiled pseudocode, not original source.
// REA Evidence: ev_182f1b66b4d149c80551c07f61c6c75f14c86cabce7dc89b5181cd61b4325940
// Source record: research/evidence/rea/avbbc-encoder-bridge/10-procedure_pseudo_code.json

undefined4 _ZN22ExecCmd_SDF_EncodeJpeg7ExecCmdEPKh(long param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
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
              (0x55,3,3,0x41,"[InfStillEncoder][SeqInternalEnc] Execute Encode Jpg");
  }
  local_100 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),3);
  local_fe = _ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt(*(undefined8 *)(param_1 + 0x10),3,0x304);
  local_fc = 0x50;
  local_fa = 1;
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x4e,"[InfStillEncoder]catId    :0x%08x",local_100);
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x4f,"[InfStillEncoder]msgId    :0x%08x",local_fe);
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x50,"[InfStillEncoder]msgLen   :0x%08x",local_fc);
  }
  iVar1 = _ZN22ExecCmd_SDF_EncodeJpeg19makeParamEncodeJpegERN3SDF25ENCODE_JPEG_COMMAND_PARAME
                    (param_1,&local_150);
  if (iVar1 == 0) {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x60,"[InfStillEncoder]ExecCmd_SDF_EncodeJpeg ExecCmd. Msg = %p",
                 &local_100);
    }
    _ZN13InfraAvbbcLog7SetLog2Ejjttttt(0x11,0x1000a,local_100,local_fe,local_fa,0,0);
    memcpy(auStack_f8,&local_148,0x48);
    _ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj
              (*(undefined8 *)(param_1 + 0x10),0x82830752,&local_100,local_fc);
    uVar3 = 0;
  }
  else {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0x58,"[InfStillEncoder]ExecCmd_SDF_EncodeJpeg makeParam err = 0x%x",iVar1)
      ;
    }
    uVar3 = 0x201;
    _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d004,0x59,iVar1,0);
    uVar2 = _ZN22InfraAvbbcErrConverter7ConvertEjj(8,0x120);
    (**(code **)(**(long **)(param_1 + 0x18) + 0x48))(*(long **)(param_1 + 0x18),0x1043,uVar2);
  }
  return uVar3;
}


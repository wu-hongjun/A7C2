// Decompiled pseudocode, not original source.
// REA Evidence: ev_3f3ac57575ded305e4e9a0239be2d9a206fc792446a649300660ba258e1408e8
// Source record: research/evidence/rea/avbbc-encoder-bridge/05-procedure_pseudo_code.json

void _ZN21ExecCmd_MC_EncodeHeif22MakeMcEncodeFrameParamEPKN17StillEncoderParam23RESIZEENC_PARAM_PAYLOADEPK18dmm_comp_open_unitPN2MC29MCODEC_STILL_ENC_ENCODE_FRAMEE
               (long param_1,long param_2,long param_3,undefined2 *param_4)

{
  char cVar1;
  ushort uVar2;
  uint uVar3;
  undefined1 uVar4;
  undefined2 uVar5;
  undefined4 uVar6;
  ulong uVar7;
  undefined8 uVar8;
  undefined2 uVar9;
  
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))
            (*(long **)(param_1 + 0x18),*(undefined4 *)(param_1 + 0x20));
  uVar5 = _ZN20InfraAvbbcMsgManager10ConvertCidEt(*(undefined8 *)(param_1 + 0x10),5);
  *param_4 = uVar5;
  uVar5 = _ZN20InfraAvbbcMsgManager13ConvertCmdMidEtt(*(undefined8 *)(param_1 + 0x10),5,0x534);
  param_4[1] = uVar5;
  param_4[2] = 0x60;
  param_4[3] = 1;
  (**(code **)(**(long **)(param_1 + 0x18) + 0xa8))(*(long **)(param_1 + 0x18),0x1060);
  *(undefined8 *)(param_4 + 4) = 0;
  uVar7 = _ZN20InfraAvbbcMsgManager9GetOsalIdEv(*(undefined8 *)(param_1 + 0x10));
  *(ulong *)(param_4 + 8) = uVar7 & 0xffffffff;
  uVar6 = _ZN20InfraAvbbcMsgManager9GetOsalIdEv(*(undefined8 *)(param_1 + 0x10));
  *(undefined4 *)(param_4 + 0xc) = uVar6;
  switch(*(undefined1 *)(param_2 + 0x70)) {
  case 0:
    uVar6 = 0x100;
    break;
  case 1:
    uVar6 = 0x101;
    break;
  default:
    uVar6 = 0x102;
    break;
  case 3:
    uVar6 = 0x103;
  }
  *(undefined4 *)(param_4 + 0x10) = uVar6;
  if (*(char *)(param_1 + 0x24) == '\x04') {
    *(undefined1 *)(param_4 + 0x12) = 0;
  }
  else {
    uVar4 = 0;
    if (*(char *)(param_2 + 0x74) == '\0') {
      uVar4 = 2;
    }
    *(undefined1 *)(param_4 + 0x12) = uVar4;
  }
  uVar4 = _ZN21ExecCmd_MC_EncodeHeif23ConvertAvbbcToMcYOffsetEhh
                    (param_1,*(undefined1 *)(param_2 + 0x60),*(undefined1 *)(param_2 + 0x71));
  *(undefined1 *)((long)param_4 + 0x25) = uVar4;
  uVar4 = _ZN21ExecCmd_MC_EncodeHeif26ConvertAvbbcToMcColorspaceEhh
                    (param_1,*(undefined1 *)(param_2 + 0x61),*(undefined1 *)(param_2 + 0x72));
  *(undefined1 *)(param_4 + 0x13) = uVar4;
  *(undefined8 *)(param_4 + 0x28) = *(undefined8 *)(param_3 + 0x18);
  *(ulong *)(param_4 + 0x2c) = (ulong)*(uint *)(param_3 + 0x20);
  cVar1 = *(char *)(param_1 + 0x24);
  if (cVar1 == '\x04') {
    *(undefined4 *)(param_4 + 0xe) = 2;
    *(undefined4 *)(param_4 + 0x14) = uRam0000000000000004;
    *(undefined8 *)(param_4 + 0x18) = uRam0000000000000010;
    *(undefined1 *)(param_4 + 0x1c) = *(undefined1 *)(param_2 + 0x62);
    uVar4 = _ZN21ExecCmd_MC_EncodeHeif24ConvertAvbbcToMcBitDepthEh
                      (param_1,*(undefined1 *)(param_2 + 99));
    *(undefined1 *)((long)param_4 + 0x39) = uVar4;
    uVar4 = _ZN21ExecCmd_MC_EncodeHeif28ConvertAvbbcToMcCaptureGammaEh
                      (param_1,*(undefined1 *)(param_2 + 0x61));
    *(undefined1 *)(param_4 + 0x1d) = uVar4;
    uVar4 = _ZN21ExecCmd_MC_EncodeHeif28ConvertAvbbcToMcColorimetoryEh
                      (param_1,*(undefined1 *)(param_2 + 0x61));
    *(undefined1 *)((long)param_4 + 0x3b) = uVar4;
    uVar2 = *(ushort *)(param_2 + 0x5e);
    param_4[0x1e] = 0x280;
    param_4[0x1f] = 0x1e0;
    if (uVar2 != 0) {
      uVar3 = 0;
      if (uVar2 != 0) {
        uVar3 = ((uint)*(ushort *)(param_2 + 0x5c) * 10) / (uint)uVar2;
      }
      if (8 < uVar3 - 10) goto switchD_00220414_default;
      switch((&DAT_00263e28)[uVar3 - 10]) {
      case 0:
        param_4[0x20] = 0;
        uVar5 = 0xf0;
        param_4[0x21] = 0;
        param_4[0x22] = 0x140;
        break;
      case 1:
        param_4[0x20] = 0;
        uVar5 = 0xd4;
        param_4[0x21] = 0;
        param_4[0x22] = 0x140;
        break;
      case 2:
        param_4[0x20] = 0;
        uVar5 = 0xb4;
        param_4[0x21] = 0;
        param_4[0x22] = 0x140;
        break;
      case 3:
        param_4[0x20] = 0;
        param_4[0x21] = 0;
        goto LAB_00220510;
      default:
        goto switchD_00220414_default;
      }
      param_4[0x23] = uVar5;
      param_4[0x24] = 0;
      param_4[0x25] = 0;
      param_4[0x26] = 0x140;
      param_4[0x27] = uVar5;
      goto LAB_0022043c;
    }
switchD_00220414_default:
    param_4[0x20] = 0;
    param_4[0x21] = 0;
LAB_00220510:
    uVar5 = 0x140;
    param_4[0x22] = 0x140;
    param_4[0x23] = 0x140;
    param_4[0x24] = 0;
    param_4[0x25] = 0;
    param_4[0x26] = 0x140;
  }
  else {
    if (cVar1 == '\x05') {
      *(undefined4 *)(param_4 + 0xe) = 1;
      *(undefined4 *)(param_4 + 0x14) = uRam0000000000000004;
      *(undefined8 *)(param_4 + 0x18) = uRam0000000000000010;
      *(undefined1 *)(param_4 + 0x1c) = *(undefined1 *)(param_2 + 0x62);
      uVar4 = _ZN21ExecCmd_MC_EncodeHeif24ConvertAvbbcToMcBitDepthEh
                        (param_1,*(undefined1 *)(param_2 + 99));
      *(undefined1 *)((long)param_4 + 0x39) = uVar4;
      uVar4 = _ZN21ExecCmd_MC_EncodeHeif28ConvertAvbbcToMcCaptureGammaEh
                        (param_1,*(undefined1 *)(param_2 + 0x61));
      *(undefined1 *)(param_4 + 0x1d) = uVar4;
      uVar4 = _ZN21ExecCmd_MC_EncodeHeif28ConvertAvbbcToMcColorimetoryEh
                        (param_1,*(undefined1 *)(param_2 + 0x61));
      *(undefined1 *)((long)param_4 + 0x3b) = uVar4;
      param_4[0x20] = 0;
      uVar9 = *(undefined2 *)(param_2 + 0x6c);
      param_4[0x1e] = 0x780;
      param_4[0x21] = 0;
      param_4[0x1f] = 0x440;
      param_4[0x22] = uVar9;
      uVar5 = *(undefined2 *)(param_2 + 0x6e);
    }
    else {
      if (cVar1 != '\x03') goto LAB_0022043c;
      cVar1 = *(char *)(param_1 + 0x2c);
      *(undefined4 *)(param_4 + 0xe) = 0;
      if (cVar1 == '\0') {
        *(undefined4 *)(param_4 + 0x14) = *(undefined4 *)(param_2 + 0x34);
        uVar8 = *(undefined8 *)(param_2 + 0x40);
      }
      else {
        *(undefined4 *)(param_4 + 0x14) = uRam0000000000000004;
        uVar8 = uRam0000000000000010;
      }
      *(undefined8 *)(param_4 + 0x18) = uVar8;
      *(undefined1 *)(param_4 + 0x1c) = *(undefined1 *)(param_2 + 0x62);
      uVar4 = _ZN21ExecCmd_MC_EncodeHeif24ConvertAvbbcToMcBitDepthEh
                        (param_1,*(undefined1 *)(param_2 + 99));
      *(undefined1 *)((long)param_4 + 0x39) = uVar4;
      uVar4 = _ZN21ExecCmd_MC_EncodeHeif28ConvertAvbbcToMcCaptureGammaEh
                        (param_1,*(undefined1 *)(param_2 + 0x61));
      *(undefined1 *)(param_4 + 0x1d) = uVar4;
      uVar4 = _ZN21ExecCmd_MC_EncodeHeif28ConvertAvbbcToMcColorimetoryEh
                        (param_1,*(undefined1 *)(param_2 + 0x61));
      *(undefined1 *)((long)param_4 + 0x3b) = uVar4;
      uVar9 = *(undefined2 *)(param_2 + 0x68);
      param_4[0x1e] = *(undefined2 *)(param_2 + 0x54);
      uVar5 = *(undefined2 *)(param_2 + 0x56);
      param_4[0x20] = 0;
      param_4[0x1f] = uVar5;
      uVar5 = *(undefined2 *)(param_2 + 0x6a);
      param_4[0x21] = 0;
      param_4[0x22] = uVar9;
    }
    param_4[0x23] = uVar5;
    param_4[0x24] = 0;
    param_4[0x25] = 0;
    param_4[0x26] = uVar9;
  }
  param_4[0x27] = uVar5;
LAB_0022043c:
  _ZN13InfraAvbbcLog7SetLog7Ejjmt(0x11,0x2010055,*(undefined8 *)(param_4 + 4),0);
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt
            (0x11,0x2010056,param_4[0xe],param_4[0x10],*(undefined1 *)(param_4 + 0x12),
             *(undefined1 *)((long)param_4 + 0x25),*(undefined1 *)(param_4 + 0x13));
  _ZN13InfraAvbbcLog7SetLog7Ejjmt(0x11,0x2010057,*(undefined8 *)(param_4 + 0x18),param_4[0x14]);
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt
            (0x11,0x2010058,*(undefined1 *)(param_4 + 0x1c),*(undefined1 *)((long)param_4 + 0x39),
             *(undefined1 *)(param_4 + 0x1d),*(undefined1 *)((long)param_4 + 0x3b),0);
  _ZN13InfraAvbbcLog7SetLog7Ejjmt(0x11,0x2010059,*(undefined8 *)(param_4 + 0x28),param_4[0x2c]);
  return;
}


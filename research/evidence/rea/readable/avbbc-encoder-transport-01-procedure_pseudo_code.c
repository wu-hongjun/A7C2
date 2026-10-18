// Decompiled pseudocode, not original source.
// REA Evidence: ev_dfc12508463cd1b4958c2ca1fadbc574a68f26d6f22537e795d06191ee30c32d
// Source record: research/evidence/rea/avbbc-encoder-transport/01-procedure_pseudo_code.json

undefined8
_ZN11ObjAvbbcCmn13ConvertCmdMidEtt(undefined8 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined8 uVar1;
  
  switch(param_2) {
  case 0:
    uVar1 = _ZN11ObjAvbbcCmn18ConvertCmdMidToVdfEt();
    return uVar1;
  case 1:
    uVar1 = _ZN11ObjAvbbcCmn18ConvertCmdMidToVefEt();
    return uVar1;
  case 2:
    uVar1 = _ZN11ObjAvbbcCmn18ConvertCmdMidToAdfEt();
    return uVar1;
  case 3:
    uVar1 = _ZN11ObjAvbbcCmn18ConvertCmdMidToSdfEt();
    return uVar1;
  case 4:
    uVar1 = _ZN11ObjAvbbcCmn19ConvertCmdMidToMmgrEt();
    return uVar1;
  case 5:
    uVar1 = _ZN11ObjAvbbcCmn17ConvertCmdMidToMcEt();
    return uVar1;
  case 6:
    uVar1 = _ZN11ObjAvbbcCmn18ConvertCmdMidToPinEt();
    return uVar1;
  case 7:
    uVar1 = _ZN11ObjAvbbcCmn19ConvertCmdMidToHdmiEt();
    return uVar1;
  default:
    return 0xffffffff;
  case 10:
    uVar1 = _ZN11ObjAvbbcCmn19ConvertCmdMidToTcubEt();
    return uVar1;
  case 0xb:
    uVar1 = _ZN11ObjAvbbcCmn22ConvertCmdMidToImagingEt();
    return uVar1;
  case 0xc:
    uVar1 = _ZN11ObjAvbbcCmn18ConvertCmdMidToSdiEt();
    return uVar1;
  case 0xd:
    uVar1 = _ZN11ObjAvbbcCmn19ConvertCmdMidToHapiEt();
    return uVar1;
  case 0xf:
    uVar1 = _ZN11ObjAvbbcCmn17ConvertCmdMidToSaEt();
    return uVar1;
  case 0x10:
    uVar1 = _ZN11ObjAvbbcCmn26ConvertCmdMidToScalarInfraEt(param_1,param_3);
    return uVar1;
  }
}


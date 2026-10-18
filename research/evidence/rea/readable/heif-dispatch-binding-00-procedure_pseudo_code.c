// Decompiled pseudocode, not original source.
// REA Evidence: ev_1e1ee4ab09f0fdab1f531b16e9729780223a5bdfa4ca0d0c056201651c07dc85
// Source record: research/evidence/rea/heif-dispatch-binding/00-procedure_pseudo_code.json

void _ZN17InfraStillEncoder14ExecuteCommandEPKhj(long param_1,long param_2,undefined4 param_3)

{
  int iVar1;
  undefined8 uVar2;
  
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x9a,"[InfStillEncoder]ExecuteCommand. osal_id = 0x%x mid = 0x%x cid = 0x%x"
               ,param_3,*(undefined4 *)(param_2 + 0x2c),*(undefined2 *)(param_2 + 10));
  }
  if (*(int *)(param_2 + 0x2c) == 0x1500) {
    switch(*(undefined2 *)(param_2 + 10)) {
    case 1:
      uVar2 = _Znwm(0x1c0);
      _ZN34InfraStillEncoderSeqStillBufConfigC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
                (uVar2,*(undefined8 *)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),param_3);
      break;
    case 2:
      if (*(char *)(param_2 + 0x24) == '\x02') {
        uVar2 = _Znwm(0x1c8);
        _ZN36InfraStillEncoderSeqResizeEncForHeifC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
                  (uVar2,*(undefined8 *)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),param_3);
      }
      else {
        uVar2 = _Znwm(0x1c8);
        _ZN29InfraStillEncoderSeqResizeEncC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
                  (uVar2,*(undefined8 *)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),param_3);
      }
      break;
    default:
      goto switchD_00217964_caseD_3;
    case 6:
      uVar2 = _Znwm(0x1c8);
      _ZN34InfraStillEncoderSeqInternalEncodeC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
                (uVar2,*(undefined8 *)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),param_3);
      break;
    case 7:
      uVar2 = _Znwm(0x1c0);
      _ZN35InfraStillEncoderSeqStillBufConfig2C1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
                (uVar2,*(undefined8 *)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),param_3);
      break;
    case 8:
      uVar2 = _Znwm(0x1c0);
      _ZN35InfraStillEncoderSeqInternalEncode2C2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
                (uVar2,*(undefined8 *)(param_1 + 0x28),*(undefined8 *)(param_1 + 0x30),param_3);
    }
  }
  else {
    if (*(int *)(param_2 + 0x2c) == 0x1630) {
      uVar2 = 0;
      if (1 < *(ushort *)(param_2 + 10)) goto LAB_002178dc;
      if (0 < avbizPrintLevel) {
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,3,0xc1,"[InfStillEncoder]Recive Resume Msg");
      }
      _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x201002e,0xc2,*(undefined2 *)(param_2 + 10),0);
    }
switchD_00217964_caseD_3:
    uVar2 = 0;
  }
LAB_002178dc:
  _ZN20InfraAvbbcSeqManager14SetSeqInstanceEP14IInfraAvbbcSeq(*(undefined8 *)(param_1 + 0x20),uVar2)
  ;
  iVar1 = _ZN20InfraAvbbcSeqManager13StartSequenceEPKh(*(undefined8 *)(param_1 + 0x20),param_2);
  if (iVar1 == 0x111) {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0xd6,"[InfStillEncoder][InfraStillEncoder] SequenceEnd");
    }
  }
  else {
    if (iVar1 == 0) {
      return;
    }
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,3,3,0xda,"[InfStillEncoder][InfraStillEncoder] SequenceEnd Error!! err=0x%X",
                 iVar1);
    }
    _ZN13InfraAvbbcLog7SetLog3Ejjtjj(0x11,0x201d001,0xdb,iVar1,0);
  }
  _ZN20InfraAvbbcSeqManager11EndSequenceEv(*(undefined8 *)(param_1 + 0x20));
  return;
}


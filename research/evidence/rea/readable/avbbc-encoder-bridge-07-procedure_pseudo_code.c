// Decompiled pseudocode, not original source.
// REA Evidence: ev_970c5d08731e021eb489c763f615b36fc39636ec96659e7e2304029355eed271
// Source record: research/evidence/rea/avbbc-encoder-bridge/07-procedure_pseudo_code.json

undefined8 _ZN21ExecCmd_MC_EncodeHeif6ResCmdEPKht(long param_1,long param_2,short param_3)

{
  char cVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  char *pcVar4;
  long local_8;
  
  if (param_3 != -0x7a9b) {
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,4,1,0x7f,"[InfDecoder]ExecCmd_MC_Decode_Frame Err. cmnMid : 0x%x");
      return 1;
    }
    return 1;
  }
  _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x2010050,0,0,0);
  (**(code **)(**(long **)(param_1 + 0x18) + 0xa8))(*(long **)(param_1 + 0x18),0x105f,&local_8);
  cVar1 = *(char *)(param_1 + 0x24);
  if (*(int *)(param_2 + 0x10) == 0) {
    if (cVar1 == '\x04') {
      *(int *)(local_8 + 0x68) = (int)*(undefined8 *)(param_2 + 0x18);
      if (avbizPrintLevel < 1) {
        return 0;
      }
      uVar3 = 0x5a;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size [0x%x](ThmJpg)\n";
    }
    else if (cVar1 == '\x05') {
      *(int *)(local_8 + 0x74) = (int)*(undefined8 *)(param_2 + 0x18);
      if (avbizPrintLevel < 1) {
        return 0;
      }
      uVar3 = 0x5e;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size [0x%x](screenNail)\n";
    }
    else {
      if (cVar1 != '\x03') {
        if (avbizPrintLevel < 1) {
          return 0;
        }
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,3,0x61,"[InfStillEncoder][SeqResizeEnc][ERROR] Encode Size (NO TARGET!)\n"
                  );
        return 0;
      }
      *(int *)(local_8 + 0x5c) = (int)*(undefined8 *)(param_2 + 0x18);
      if (avbizPrintLevel < 1) {
        return 0;
      }
      uVar3 = 0x56;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size [0x%x](MainJpg)\n";
    }
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz(0x55,3,3,uVar3,pcVar4);
    return 0;
  }
  if (cVar1 == '\x04') {
    *(undefined4 *)(local_8 + 0x68) = 0;
    if (avbizPrintLevel < 1) goto LAB_0021ff94;
    pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size Error [0x%x](ThmJpg)\n";
    uVar3 = 0x6e;
LAB_0021ff58:
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz(0x55,3,3,uVar3,pcVar4,0);
  }
  else {
    if (cVar1 == '\x05') {
      *(undefined4 *)(local_8 + 0x74) = 0;
      if (avbizPrintLevel < 1) goto LAB_0021ff94;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size Error [0x%x](screenNail)\n";
      uVar3 = 0x72;
      goto LAB_0021ff58;
    }
    if (cVar1 == '\x03') {
      *(undefined4 *)(local_8 + 0x5c) = 0;
      if (avbizPrintLevel < 1) goto LAB_0021ff94;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size Error [0x%x](MainJpg)\n";
      uVar3 = 0x6a;
      goto LAB_0021ff58;
    }
    if (avbizPrintLevel < 1) goto LAB_0021ff94;
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x75,"[InfStillEncoder][SeqResizeEnc][ERROR] Encode Size (NO TARGET!)\n");
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,4,1,0x78,"[InfDecoder]ExecCmd_MC_EncodeHeif Prm Err. error : 0x%x",
               *(undefined4 *)(param_2 + 0x10));
  }
LAB_0021ff94:
  uVar2 = _ZN22InfraAvbbcErrConverter7ConvertEjj(5,*(undefined4 *)(param_2 + 0x10));
  (**(code **)(**(long **)(param_1 + 0x18) + 0x48))
            (*(long **)(param_1 + 0x18),*(undefined4 *)(param_1 + 0x28),uVar2);
  return 0x201;
}


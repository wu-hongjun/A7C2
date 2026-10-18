// Decompiled pseudocode, not original source.
// REA Evidence: ev_b4037a12dafc552cfd0aa0ac8316e172c2bf27ec03c143523152ef6f8ce435de
// Source record: research/evidence/rea/avbbc-encoder-bridge/03-procedure_pseudo_code.json

undefined4 _ZN30ExecCmd_SDF_EncodeJpegMainComp6ResCmdEPKht(long param_1,long param_2)

{
  char cVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  char *pcVar4;
  long local_8;
  
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x49,
               "[InfStillEncoder][SeqResizeEnc] Receive Response from SDF [Encode Jpg Comp]");
  }
  _ZN13InfraAvbbcLog7SetLog0Ejjjjt(0x11,0x201000c,0,0,0);
  (**(code **)(**(long **)(param_1 + 0x18) + 0x48))
            (*(long **)(param_1 + 0x18),0x104c,*(undefined4 *)(param_2 + 8));
  (**(code **)(**(long **)(param_1 + 0x18) + 0xa8))(*(long **)(param_1 + 0x18),0x1048,&local_8);
  if (*(int *)(param_2 + 8) == 0) {
    cVar1 = *(char *)(param_1 + 0x20);
    if (cVar1 == '\x01') {
      *(undefined4 *)(local_8 + 0x68) = *(undefined4 *)(param_2 + 0xc);
      if (avbizPrintLevel < 1) {
        return 0;
      }
      uVar3 = 0x79;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size [0x%x](ThmJpg)\n";
    }
    else if (cVar1 == '\0') {
      *(undefined4 *)(local_8 + 0x5c) = *(undefined4 *)(param_2 + 0xc);
      if (avbizPrintLevel < 1) {
        return 0;
      }
      uVar3 = 0x75;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size [0x%x](MainJpg)\n";
    }
    else {
      if (cVar1 != '\x02') {
        if (avbizPrintLevel < 1) {
          return 1;
        }
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,3,0x80,"[InfStillEncoder][SeqResizeEnc][ERROR] Encode Size (NO TARGET!)\n"
                  );
        return 1;
      }
      *(undefined4 *)(local_8 + 0x74) = *(undefined4 *)(param_2 + 0xc);
      if (avbizPrintLevel < 1) {
        return 0;
      }
      uVar3 = 0x7d;
      pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size [0x%x](screenNail)\n";
    }
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz(0x55,3,3,uVar3,pcVar4);
    return 0;
  }
  if (0 < avbizPrintLevel) {
    _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
              (0x55,3,3,0x59,"[InfStillEncoder][SeqInternalEnc] SDF Error [ErrCode: %x]\n",
               *(int *)(param_2 + 8));
  }
  cVar1 = *(char *)(param_1 + 0x20);
  if (cVar1 == '\x01') {
    *(undefined4 *)(local_8 + 0x68) = 0;
    if (avbizPrintLevel < 1) goto LAB_00222c94;
    pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size Error [0x%x](ThmJpg)\n";
    uVar3 = 99;
  }
  else if (cVar1 == '\0') {
    *(undefined4 *)(local_8 + 0x5c) = 0;
    if (avbizPrintLevel < 1) goto LAB_00222c94;
    pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size Error [0x%x](MainJpg)\n";
    uVar3 = 0x5f;
  }
  else {
    if (cVar1 != '\x02') {
      if (0 < avbizPrintLevel) {
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,3,3,0x6a,"[InfStillEncoder][SeqResizeEnc][ERROR] Encode Size (NO TARGET!)\n"
                  );
      }
      goto LAB_00222c94;
    }
    *(undefined4 *)(local_8 + 0x74) = 0;
    if (avbizPrintLevel < 1) goto LAB_00222c94;
    pcVar4 = "[InfStillEncoder][SeqResizeEnc] Encode Size Error [0x%x](screenNail)\n";
    uVar3 = 0x67;
  }
  _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz(0x55,3,3,uVar3,pcVar4,0);
LAB_00222c94:
  uVar2 = _ZN22InfraAvbbcErrConverter7ConvertEjj(3,*(undefined4 *)(param_2 + 8));
  (**(code **)(**(long **)(param_1 + 0x18) + 0x48))(*(long **)(param_1 + 0x18),0x1043,uVar2);
  return 0x201;
}


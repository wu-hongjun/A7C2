// Decompiled pseudocode, not original source.
// REA Evidence: ev_5a7b060589883f4149344b9ddb06b7078cc8db42b84e0ab1880aaa9eaaa50fb9
// Source record: research/evidence/rea/sony-fsys-completion/00-procedure_pseudo_code.json

uint Fsys_WriteDirectFileAsyncV2(char *param_1)

{
  char *pcVar1;
  short sVar2;
  uint uVar3;

  sVar2 = DAT_001b75e0;
  DAT_001b75e0 = DAT_001b75e0 + 1;
  if (DAT_001b75e0 == 0) {
    DAT_001b75e0 = 1;
  }
  pcVar1 = "";
  if (param_1 != (char *)0x0) {
    pcVar1 = param_1;
  }
  fsysDepTrace_Printf(1," Fsys_WriteDirectFileAsyncV2 - %s",pcVar1);
  if ((g_fsysStatus & 1) == 0) {
    fsysDepTrace_Printf(3,"*Fsys_WriteDirectFileAsyncV2 - 0x%08x",0x80000010);
    return 0x80000010;
  }
  fsysDepLog_FuncIn__Path(0x31,sVar2,pcVar1);
  if (param_1 == (char *)0x0) {
    uVar3 = 0x30;
  }
  else {
    if ((*param_1 != '\\') &&
       (uVar3 = fsysDepAsync_DriveQueueRequest(*param_1,0,&LAB_0012f7e4,param_1,0x508,1,sVar2),
       uVar3 != 0x80000040)) goto LAB_00132f6c;
    uVar3 = 0x41;
  }
  uVar3 = uVar3 | 0x80000000;
LAB_00132f6c:
  fsysDepLog_FuncOut_Path(0x31,sVar2,pcVar1,uVar3);
  fsysDepTrace_Printf(3,"*Fsys_WriteDirectFileAsyncV2 - 0x%08x",uVar3);
  return uVar3;
}

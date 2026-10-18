// Decompiled pseudocode, not original source.
// REA Evidence: ev_8c619d6404d8b34a38494d275b6666be9bc792765dc63cb91342c81976ec8e45
// Source record: research/evidence/rea/avbbc-encoder-transport/02-procedure_pseudo_code.json

void _ZN11ObjAvbbcCmn18ConvertCmdMidToSdfEt(undefined8 param_1,short param_2)

{
  ulong uVar1;
  
  for (uVar1 = 0;
      (*(short *)(&DAT_0026bf0a + uVar1 * 4) != -1 &&
      ((&_ZN16ObjAvbbcCmnTable17midConvTableToSdfE)[uVar1 * 2] != param_2));
      uVar1 = (ulong)((int)uVar1 + 1)) {
  }
  return;
}


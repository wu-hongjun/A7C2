// Decompiled pseudocode, not original source.
// REA Evidence: ev_820a234584e502dd6c3dca97d9513ebe82af4c73bfb6dc283a76a59661ba794b
// Source record: research/evidence/rea/avbbc-encoder-transport/03-procedure_pseudo_code.json

void _ZN11ObjAvbbcCmn17ConvertCmdMidToMcEt(undefined8 param_1,short param_2)

{
  ulong uVar1;
  
  for (uVar1 = 0;
      (*(short *)(&DAT_0026bd82 + uVar1 * 4) != -1 &&
      ((&_ZN16ObjAvbbcCmnTable16midConvTableToMcE)[uVar1 * 2] != param_2));
      uVar1 = (ulong)((int)uVar1 + 1)) {
  }
  return;
}


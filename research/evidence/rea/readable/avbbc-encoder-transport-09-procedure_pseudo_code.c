// Decompiled pseudocode, not original source.
// REA Evidence: ev_3992e221aaafa33eb7ab32c525d7a1dac7872e0b372a560a375c828eaa707483
// Source record: research/evidence/rea/avbbc-encoder-transport/09-procedure_pseudo_code.json

void _ZN11ObjAvbbcCmn18ConvertCmdMidToVdfEt(undefined8 param_1,short param_2)

{
  ulong uVar1;
  
  for (uVar1 = 0;
      (*(short *)(&DAT_0026bf5a + uVar1 * 4) != -1 &&
      ((&_ZN16ObjAvbbcCmnTable17midConvTableToVdfE)[uVar1 * 2] != param_2));
      uVar1 = (ulong)((int)uVar1 + 1)) {
  }
  return;
}


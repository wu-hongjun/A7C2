// Decompiled pseudocode, not original source.
// REA Evidence: ev_45482d741eff37996627461da1c08ece7d75a7fe1b893d0b97df06e9be4d92e8
// Source record: research/evidence/rea/avbbc-encoder-transport/08-procedure_pseudo_code.json

void _ZN11ObjAvbbcCmn18ConvertCmdMidToAdfEt(undefined8 param_1,short param_2)

{
  ulong uVar1;
  
  for (uVar1 = 0;
      (*(short *)(&DAT_0026be4a + uVar1 * 4) != -1 &&
      ((&_ZN16ObjAvbbcCmnTable17midConvTableToAdfE)[uVar1 * 2] != param_2));
      uVar1 = (ulong)((int)uVar1 + 1)) {
  }
  return;
}


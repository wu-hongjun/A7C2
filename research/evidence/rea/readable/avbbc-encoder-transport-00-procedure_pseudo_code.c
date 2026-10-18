// Decompiled pseudocode, not original source.
// REA Evidence: ev_fe083b5d503204a616c1adda6ca7929b58e7648dc9ff3f054659ccdcb141b711
// Source record: research/evidence/rea/avbbc-encoder-transport/00-procedure_pseudo_code.json

undefined2 _ZN11ObjAvbbcCmn10ConvertCidEt(undefined8 param_1,ushort param_2)

{
  undefined2 uVar1;
  
  uVar1 = 0;
  if (param_2 < 0x11) {
    uVar1 = *(undefined2 *)(&DAT_0026bd58 + (ulong)param_2 * 2);
  }
  return uVar1;
}


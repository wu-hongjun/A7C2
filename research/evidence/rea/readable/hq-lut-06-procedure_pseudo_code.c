// Decompiled pseudocode, not original source.
// REA Evidence: ev_c715d1797185e86729974e9633ddd0f9fda81d203cb876db7e75e764f71eae50
// Source record: research/evidence/rea/hq-lut/06-procedure_pseudo_code.json

void FUN_037f0b8c(int *param_1,undefined8 *param_2,undefined4 param_3,undefined8 param_4,
                 long param_5,undefined8 param_6)

{
  undefined8 *puVar1;
  
  if (*param_1 - 4U < 3) {
    puVar1 = (undefined8 *)param_2[1];
    for (param_2 = (undefined8 *)*param_2; puVar1 != param_2; param_2 = param_2 + 3) {
      if (*(int *)*param_2 == 9) {
        FUN_037f092c(param_1,param_5,param_6,param_3,((int *)*param_2)[1],
                     *(undefined4 *)(*(long *)(param_5 + 0x108) + 0x10),*param_1);
      }
    }
    return;
  }
  return;
}


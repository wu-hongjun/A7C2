// Decompiled pseudocode, not original source.
// REA Evidence: ev_171a892e87007414a550a10c7571df6682d828d74fce31f795626631ee495012
// Source record: research/evidence/rea/libmpr-still-sink/03-procedure_pseudo_code.json

undefined4 FUN_003715c0(long *param_1,undefined8 param_2,undefined8 param_3)

{
  undefined4 uVar1;
  
  FUN_00370acc();
  if (*(int *)((long)param_1 + 0x13e4) == 0) {
    uVar1 = (**(code **)(*param_1 + 0x40))(param_1,param_2,param_3);
    *(undefined4 *)((long)param_1 + 0x13e4) = 4;
  }
  else {
    uVar1 = 0xffffffff;
  }
  FUN_00370b0c(param_1);
  return uVar1;
}


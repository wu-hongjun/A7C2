// Decompiled pseudocode, not original source.
// REA Evidence: ev_af5b4de1a8823d10db49971d872cca98f479d4634e3a1beb378ca8e2c00e790e
// Source record: research/evidence/rea/hq-lut/01-procedure_pseudo_code.json

void FUN_037fcfb0(long param_1,undefined1 param_2,undefined1 param_3,int param_4,undefined4 param_5,
                 undefined8 param_6,undefined8 param_7,undefined8 param_8,undefined8 param_9,
                 undefined8 param_10)

{
  int iVar1;
  int iVar2;
  undefined1 auStack_10 [2];
  undefined1 local_e;
  undefined1 local_d;
  undefined1 local_8;
  undefined1 local_7;
  undefined1 local_6;
  undefined1 local_5;
  undefined1 local_4;
  
  FUN_037f8af8(&local_8);
  FUN_037f8b74(auStack_10);
  local_7 = 0;
  local_6 = 1;
  local_5 = param_3;
  local_4 = param_2;
  if (param_4 == 9) {
    iVar1 = *(int *)(param_1 + 0x28);
    local_8 = 0;
    local_e = 4;
    local_d = 0;
    for (iVar2 = 0; iVar2 < iVar1; iVar2 = iVar2 + 1) {
      FUN_037fcf94(param_1,9,iVar2,param_5,&local_8,auStack_10,param_6,param_7,param_8,param_9,
                   param_10);
      *(int *)(param_1 + 0xc) = *(int *)(param_1 + 0xc) + 1;
      *(int *)(param_1 + 0x10) = *(int *)(param_1 + 0x10) + 1;
    }
  }
  FUN_037f8b88(auStack_10);
  return;
}


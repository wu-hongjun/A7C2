// Decompiled pseudocode, not original source.
// REA Evidence: ev_54e93330ecd546d8e3c7d88a8731bd083b51c48c741bfa94b641bafb1b97a73a
// Source record: research/evidence/rea/still-buffer-ownership/02-procedure_pseudo_code.json

uint _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
               (long param_1,uint param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  ulong uVar3;
  
  iVar1 = *param_3;
  if (iVar1 != 0) {
    uVar3 = 0;
    do {
      if (param_2 == uVar3) {
        *(int *)(param_1 + (ulong)param_2 * 4) = iVar1;
        FUN_003b24f0(*param_3);
        return param_2 + 1;
      }
      iVar2 = *(int *)(param_1 + uVar3 * 4);
      uVar3 = uVar3 + 1;
    } while (iVar2 == 0 || iVar1 != iVar2);
  }
  return param_2;
}


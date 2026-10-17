// Decompiled pseudocode, not original source.
// REA Evidence: ev_502bca800b6b9d3936e1c7768b86520beb9f16ce22e5c21e8bd0781070783ed2
// Source record: research/evidence/rea/im/03-procedure_pseudo_code.json

int FUN_004064fc(int param_1,int param_2,int param_3,uint param_4)

{
  long lVar1;
  int *piVar2;
  char local_8 [8];
  
  if (DAT_00419748 == 0xffffffff) {
    param_1 = -1;
  }
  else {
    local_8[0] = (char)param_2 + '/';
    local_8[1] = 0;
    FUN_00405880(0,&DAT_00407553,0x94,local_8);
    lVar1 = IMDB_get_entries();
    for (piVar2 = (int *)(lVar1 + (long)param_1 * 0x48); *piVar2 != 0x28 && param_2 != *piVar2;
        piVar2 = piVar2 + 0x12) {
      if (((param_3 != 0) && ((piVar2[2] & DAT_00419748) != 0)) && ((param_4 & piVar2[1]) != 0)) {
        FUN_00405880(0,&DAT_00407deb,(short)piVar2[3],*(undefined8 *)(piVar2 + 4));
        FUN_00406374((short)piVar2[3],3,0x94);
        FUN_00405880(1,&DAT_00407deb,(short)piVar2[3],*(undefined8 *)(piVar2 + 4));
      }
      param_1 = param_1 + 1;
    }
    FUN_00405880(1,&DAT_00407553,0x94,local_8);
  }
  return param_1;
}


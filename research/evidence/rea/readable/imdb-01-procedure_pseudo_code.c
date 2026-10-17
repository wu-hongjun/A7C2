// Decompiled pseudocode, not original source.
// REA Evidence: ev_d0f48e2edd18e50291eba7b4de2a5a150c9098bff78319bbe24b765adb548b4d
// Source record: research/evidence/rea/imdb/01-procedure_pseudo_code.json

long IMDB_find_entry(short param_1,uint param_2)

{
  int iVar1;
  int *piVar2;
  
  if (param_2 != 0xffffffff) {
    iVar1 = 0;
    for (piVar2 = (int *)(imdb_raw + 0x100); *piVar2 != 0x28; piVar2 = piVar2 + 0x12) {
      if (((short)piVar2[3] == param_1) && ((param_2 & piVar2[2]) != 0)) {
        return (long)iVar1 * 0x48 + 0x11db20;
      }
      iVar1 = iVar1 + 1;
    }
  }
  return 0;
}


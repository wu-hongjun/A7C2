// Decompiled pseudocode, not original source.
// REA Evidence: ev_159f5d292afed40aeb41f1e446418474af28b7832480b8a08fb9b10e5f57ae5d
// Source record: research/evidence/rea/libmpr/02-procedure_pseudo_code.json

undefined8 _ZN3mpr17ExecuteSetLutDataERKNS_17setLutDataInParamE(long param_1)

{
  undefined8 *puVar1;
  int iVar2;
  undefined8 uVar3;
  long *plVar4;
  undefined1 auStack_10 [4];
  undefined4 local_c;
  undefined2 local_8;
  undefined2 local_6;
  undefined1 local_4;
  undefined1 local_3;
  undefined1 local_2;
  undefined1 local_1;
  
  if (*(char *)(param_1 + 0x1f) == '\0') {
    local_6 = 0x1107;
  }
  else {
    if (_ZN3mpr14sp_lutDataAddrE == (void *)0x0) {
      _ZN3mpr14sp_lutDataAddrE = malloc(0x6a1b9);
    }
    if (_ZN3mpr14sp_lutDataAddrE != (void *)0x0) {
      puVar1 = (undefined8 *)(param_1 + 0x1f);
      FUN_0027dc40(auStack_10);
      local_c = 0;
      iVar2 = FUN_0027e880(auStack_10,(undefined8 *)(param_1 + 5),puVar1,_ZN3mpr14sp_lutDataAddrE);
      if (iVar2 == 0) {
        FUN_0027dc48(auStack_10,_ZN3mpr14sp_lutDataAddrE,local_c);
        plVar4 = &DAT_00478c20;
        do {
          if (*plVar4 != 0) {
            FUN_00219de4(*plVar4,puVar1,_ZN3mpr14sp_lutDataAddrE,local_c);
          }
          plVar4 = plVar4 + 0x358;
        } while (plVar4 != (long *)0x47f720);
        DAT_00478aa0 = *(undefined8 *)(param_1 + 5);
        DAT_00478aa8 = *(undefined8 *)(param_1 + 0xd);
        DAT_00478ab0 = *(undefined8 *)(param_1 + 0x15);
        DAT_00478ab8 = *(undefined2 *)(param_1 + 0x1d);
        DAT_00478aba = *puVar1;
        DAT_00478ac2 = *(undefined8 *)(param_1 + 0x27);
        DAT_00478aca = *(undefined8 *)(param_1 + 0x2f);
        DAT_00478ad2 = *(undefined8 *)(param_1 + 0x37);
        DAT_00478ada = *(undefined8 *)(param_1 + 0x3f);
        DAT_00478ae2 = *(undefined8 *)(param_1 + 0x47);
        DAT_00478aea = *(undefined8 *)(param_1 + 0x4f);
        DAT_00478af2 = *(undefined8 *)(param_1 + 0x57);
        uVar3 = 0;
      }
      else {
        if (_ZN3mpr14sp_lutDataAddrE != (void *)0x0) {
          free(_ZN3mpr14sp_lutDataAddrE);
          _ZN3mpr14sp_lutDataAddrE = (void *)0x0;
        }
        uVar3 = 2;
        local_3 = 0;
        local_4 = 4;
        local_2 = 0;
        local_8 = 0x2110;
        local_1 = 0;
        local_6 = 0x110a;
        FUN_001ca0d0(&local_8,8);
      }
      FUN_0027dc44(auStack_10);
      return uVar3;
    }
    local_6 = 0x1108;
  }
  local_1 = 0;
  local_2 = 0;
  local_3 = 0;
  local_4 = 4;
  local_8 = 0x2290;
  FUN_001ca0d0(&local_8,8);
  return 2;
}


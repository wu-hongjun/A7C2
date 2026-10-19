// Decompiled pseudocode, not original source.
// REA Evidence: ev_0e2ff81ec33c89ddf5b3ede91f77294280d904a6463f2e044c96e6e05aa9b2b0
// Source record: research/evidence/rea/sony-capture-allocation/01-procedure_pseudo_code.json

undefined4
_ZN3mpr11stillrecord12_check_errorEPNS_18recordStillParam_tERKNS_18recordStillInParamE
          (long param_1,long param_2)

{
  uint uVar1;
  int iVar2;
  long lVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  undefined2 local_18;
  undefined2 local_16;
  undefined1 local_14;
  undefined1 local_13;
  undefined1 local_12;
  undefined1 local_11;
  int local_10;
  int iStack_c;
  int local_8;
  int iStack_4;

  if (param_1 == 0) {
    uVar5 = 0xffffffff;
    local_13 = 0;
    local_14 = 4;
    local_12 = 0;
    local_18 = 0x2120;
    local_11 = 0;
    local_16 = 0x3020;
    FUN_001df8d4(&local_18,8);
  }
  else {
    uVar5 = 0;
    if (*(int *)(param_1 + 0xc) != 3) {
      piVar6 = (int *)(param_1 + 500);
      do {
        uVar1 = piVar6[6];
        if (uVar1 != 0xffffffff) {
          iVar4 = *piVar6;
          if (iVar4 == 1) {
            lVar3 = *(long *)(param_2 + (ulong)uVar1 * 8 + 8);
            if (lVar3 == 0) {
              uVar5 = 0xffffffeb;
              goto LAB_001e3988;
            }
            if (((*(int *)(lVar3 + 0x98) == 0) || (local_10 = *(int *)(lVar3 + 0x90), local_10 == 0)
                ) || (*(int *)(lVar3 + 0x9c) == 0)) {
              uVar5 = 0xfffffffe;
              goto LAB_001e3988;
            }
            if (((*(int *)(lVar3 + 0xa8) == 0) || (iVar4 = *(int *)(lVar3 + 0xa0), iVar4 == 0)) ||
               (iVar2 = *(int *)(lVar3 + 0xac), iVar2 == 0)) {
              uVar5 = 0xfffffffd;
              goto LAB_001e3988;
            }
            local_16 = 0x3021;
            iStack_c = *(int *)(lVar3 + 0x9c);
          }
          else if (iVar4 == 2) {
            lVar3 = *(long *)(param_2 + (ulong)uVar1 * 8 + 0x18);
            if (lVar3 == 0) {
              uVar5 = 0xffffffea;
              goto LAB_001e3988;
            }
            if (((*(int *)(lVar3 + 0x98) == 0) || (local_10 = *(int *)(lVar3 + 0x90), local_10 == 0)
                ) || (*(int *)(lVar3 + 0x9c) == 0)) {
              uVar5 = 0xfffffffb;
              goto LAB_001e3988;
            }
            if (((*(int *)(lVar3 + 0x128) == 0) || (iVar4 = *(int *)(lVar3 + 0x120), iVar4 == 0)) ||
               (iVar2 = *(int *)(lVar3 + 300), iVar2 == 0)) {
              uVar5 = 0xfffffff9;
              goto LAB_001e3988;
            }
            local_16 = 0x3022;
            iStack_c = *(int *)(lVar3 + 0x9c);
          }
          else {
            if (iVar4 != 4) {
              uVar5 = 0xfffffff2;
              goto LAB_001e3988;
            }
            lVar3 = *(long *)(param_2 + (ulong)uVar1 * 8 + 0x28);
            if (lVar3 == 0) {
              uVar5 = 0xffffffe9;
              goto LAB_001e3988;
            }
            if (((*(int *)(lVar3 + 0x98) == 0) || (local_10 = *(int *)(lVar3 + 0x90), local_10 == 0)
                ) || (*(int *)(lVar3 + 0x9c) == 0)) {
              uVar5 = 0xfffffff7;
              goto LAB_001e3988;
            }
            if (((*(int *)(lVar3 + 0xa8) == 0) || (iVar4 = *(int *)(lVar3 + 0xa0), iVar4 == 0)) ||
               (iVar2 = *(int *)(lVar3 + 0xac), iVar2 == 0)) {
              uVar5 = 0xfffffff6;
              goto LAB_001e3988;
            }
            local_16 = 0x3023;
            iStack_c = *(int *)(lVar3 + 0x9c);
          }
          local_14 = 1;
          local_11 = 0;
          local_12 = 0;
          local_13 = 0;
          local_18 = 0x2120;
          local_8 = iVar4;
          iStack_4 = iVar2;
          FUN_001df8d4(&local_18,0x18);
        }
        piVar6 = piVar6 + 0x162;
      } while (piVar6 != (int *)(param_1 + 0xd04));
      uVar5 = 0;
    }
  }
LAB_001e3988:
  local_14 = 3;
  local_13 = 0;
  local_18 = 0x2120;
  local_12 = 0;
  local_16 = 0x2301;
  local_11 = 0;
  local_10 = uVar5;
  FUN_001df8d4(&local_18,0xc);
  return uVar5;
}

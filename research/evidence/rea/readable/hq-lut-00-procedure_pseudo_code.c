// Decompiled pseudocode, not original source.
// REA Evidence: ev_0aa1c369bc9807ba3e4e1121118a2c3824d91604af6e9c8560908c7683aa9917
// Source record: research/evidence/rea/hq-lut/00-procedure_pseudo_code.json

void FUN_037f6900(undefined8 param_1,byte *param_2,long *param_3)

{
  bool bVar1;
  byte *__s;
  byte *pbVar2;
  long *plVar3;
  byte bVar4;
  byte bVar5;
  int iVar6;
  int *piVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  long *plVar10;
  long lVar11;
  long lVar12;
  long lVar13;
  long lVar14;
  long lVar15;
  long lVar16;
  long lVar17;
  long lVar18;
  undefined1 auStack_210 [8];
  undefined4 local_208;
  undefined4 local_204;
  undefined1 auStack_200 [24];
  long local_1e8 [3];
  undefined1 auStack_1d0 [24];
  size_t *local_1b8;
  size_t local_1b0;
  size_t local_1a8 [2];
  undefined1 auStack_198 [32];
  undefined1 auStack_178 [8];
  undefined1 auStack_170 [40];
  undefined1 auStack_148 [48];
  undefined1 auStack_118 [72];
  undefined1 auStack_d0 [96];
  size_t local_70 [3];
  undefined1 auStack_58 [48];
  int local_28;
  undefined4 local_10;
  
  __s = param_2 + 1;
  local_1b8 = local_1a8;
  local_70[0] = strlen((char *)__s);
  pbVar2 = __s + local_70[0];
  if (0xf < local_70[0]) {
    local_1b8 = (size_t *)
                _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm
                          (&local_1b8,local_70,0);
    local_1a8[0] = local_70[0];
  }
  _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_S_copy_charsEPcPKcS7_
            (local_1b8,__s,pbVar2);
  local_1b0 = local_70[0];
  *(undefined1 *)((long)local_1b8 + local_70[0]) = 0;
  iVar6 = _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc(&local_1b8,"LosGatos");
  if (((iVar6 == 0) ||
      (iVar6 = _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc(&local_1b8,"IPSX")
      , iVar6 == 0)) ||
     (iVar6 = _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc
                        (&local_1b8,&DAT_052a04d1), iVar6 == 0)) {
    bVar4 = *param_2;
    bVar5 = param_2[0x23];
    bVar1 = bVar4 < 2;
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(local_70,&local_1b8);
    FUN_037f74d8(auStack_178,bVar4,0,bVar1,local_70,0);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_70);
    FUN_037f79b0(local_70,auStack_178);
    FUN_037f7c34(auStack_200,local_70);
    FUN_037f66a4(local_1e8,local_70);
    piVar7 = (int *)_Znwm(0xf0);
    FUN_037ed9b4();
    thunk_FUN_037ef028(piVar7,local_28,2,bVar5,3);
    if (local_28 - 4U < 3) {
      iVar6 = *piVar7;
      lVar16 = *(long *)(param_2 + 0x28);
      lVar14 = 0;
      for (lVar13 = 0; (int)lVar13 < iVar6; lVar13 = lVar13 + 1) {
        plVar10 = *(long **)(*(long *)(piVar7 + 0x36) + lVar13 * 0x18);
        plVar3 = plVar10 + (ulong)(iVar6 - 1) * 3 + 3;
        lVar15 = lVar14;
        do {
          lVar17 = *plVar10;
          lVar11 = 0;
          do {
            lVar18 = *(long *)(lVar17 + lVar11 * 0x18);
            lVar12 = 0;
            do {
              *(undefined4 *)(lVar18 + lVar12) =
                   *(undefined4 *)(lVar16 + (lVar15 + lVar11 * 4) * 4 + lVar12);
              lVar12 = lVar12 + 4;
            } while (lVar12 != 0xc);
            lVar11 = lVar11 + 1;
          } while ((int)lVar11 < iVar6);
          plVar10 = plVar10 + 3;
          lVar15 = lVar15 + (iVar6 << 2);
        } while (plVar3 != plVar10);
        lVar14 = lVar14 + iVar6 * iVar6 * 4;
      }
    }
    param_2[0x28] = 0;
    param_2[0x29] = 0;
    param_2[0x2a] = 0;
    param_2[0x2b] = 0;
    param_2[0x2c] = 0;
    param_2[0x2d] = 0;
    param_2[0x2e] = 0;
    param_2[0x2f] = 0;
    FUN_037f5238(auStack_148,local_28);
    FUN_037f520c(auStack_210,bVar4,0,bVar1);
    plVar10 = (long *)_Znwm(0x18);
    *plVar10 = 0;
    plVar10[1] = 0;
    plVar10[2] = 0;
    FUN_037f62b8(auStack_148,local_28,auStack_210,piVar7,plVar10);
    FUN_037edc90(piVar7);
    _ZdlPvm(piVar7,0xf0);
    uVar8 = _Znwm(0x390);
    FUN_037fe2f4();
    FUN_037f8990(local_70,plVar10,uVar8);
    FUN_037f6810(auStack_1d0,auStack_58);
    lVar14 = plVar10[1];
    for (lVar13 = *plVar10; lVar14 != lVar13; lVar13 = lVar13 + 0xf0) {
      FUN_037edc90(lVar13);
    }
    if (*plVar10 != 0) {
      _ZdlPv();
    }
    _ZdlPvm(plVar10,0x18);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_198,&local_1b8);
    uVar9 = _Znwm(0x7c8);
    FUN_037fd8e4(uVar9,auStack_198);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(auStack_198);
    FUN_037ef784(auStack_118,local_28);
    FUN_037f0b8c(auStack_118,auStack_1d0,bVar4,bVar1,uVar8,uVar9);
    FUN_037fe410(uVar8);
    _ZdlPvm(uVar8,0x390);
    FUN_037f3d30(auStack_118,local_28,uVar9);
    local_208 = 0;
    local_204 = 0;
    FUN_037f9d00(auStack_d0,local_28);
    FUN_037fd0d8(auStack_d0,local_10,local_1e8,auStack_200,uVar9,*(undefined8 *)*param_3,&local_208,
                 *(undefined8 *)param_3[1],&local_204);
    FUN_037fd518(uVar9);
    _ZdlPvm(uVar9,0x7c8);
    *(undefined4 *)(*param_3 + 8) = local_208;
    *(undefined4 *)(param_3[1] + 8) = local_204;
    FUN_037f9658(auStack_d0);
    FUN_037f6778(auStack_1d0);
    FUN_037f52e0(auStack_148);
    if (local_1e8[0] != 0) {
      _ZdlPv();
    }
    FUN_037f67b4(auStack_200);
    FUN_037f7714(local_70);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(auStack_170);
  }
  _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_1b8);
  return;
}


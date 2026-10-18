// Decompiled pseudocode, not original source.
// REA Evidence: ev_4afcf105b8223d8a526e2d1e49cd046320e6a8d66b95247512023cb6283dc887
// Source record: research/evidence/rea/hq-lut/04-procedure_pseudo_code.json

int FUN_037fc5a4(long param_1,long *param_2,int param_3,long param_4,int param_5,int param_6)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  long lVar5;
  uint *puVar6;
  byte *pbVar7;
  long local_1b0;
  long local_1a8;
  uint *local_198;
  undefined8 uStack_190;
  undefined8 local_188;
  undefined1 *local_180 [2];
  undefined1 auStack_170 [16];
  undefined1 *local_160 [2];
  undefined1 auStack_150 [16];
  undefined1 *local_140 [2];
  undefined1 auStack_130 [16];
  undefined1 *local_120 [2];
  undefined1 auStack_110 [16];
  undefined1 *local_100 [2];
  undefined1 auStack_f0 [16];
  undefined1 *local_e0 [2];
  undefined1 auStack_d0 [16];
  undefined1 *local_c0 [2];
  undefined1 auStack_b0 [16];
  undefined1 *local_a0 [2];
  undefined1 auStack_90 [16];
  undefined1 auStack_80 [32];
  undefined1 auStack_60 [32];
  undefined1 auStack_40 [32];
  undefined4 local_20 [8];
  
  local_198 = (uint *)0x0;
  uStack_190 = 0;
  local_188 = 0;
  if (*(int *)(param_1 + 4) - 4U < 3) {
    if (param_3 != 0) {
      iVar2 = FUN_037fe634(param_3);
      local_20[0] = 0;
      FUN_037fbe88(&local_198,(long)iVar2,local_20);
      if (param_6 == 0) {
        puVar6 = local_198;
        for (lVar5 = 0; (int)lVar5 < param_3 / 4; lVar5 = lVar5 + 1) {
          pbVar7 = *(byte **)(*param_2 + lVar5 * 0x18);
          *puVar6 = (uint)*pbVar7;
          puVar6[1] = (uint)pbVar7[1];
          puVar6[2] = (uint)pbVar7[2];
          puVar6[3] = (uint)pbVar7[3];
          puVar6 = puVar6 + 4;
        }
      }
      else {
        local_1b0 = 0xc;
        for (local_1a8 = 0; (int)local_1a8 < param_3 / 4; local_1a8 = local_1a8 + 1) {
          lVar5 = local_1a8 * 0x18;
          local_180[0] = auStack_170;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_180,1,**(undefined1 **)(*param_2 + lVar5));
          local_160[0] = auStack_150;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_160,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 1));
          local_140[0] = auStack_130;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_140,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 2));
          local_120[0] = auStack_110;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_120,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 3));
          local_100[0] = auStack_f0;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_100,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 4));
          local_e0[0] = auStack_d0;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_e0,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 5));
          local_c0[0] = auStack_b0;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_c0,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 6));
          local_a0[0] = auStack_90;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructEmc
                    (local_a0,1,*(undefined1 *)(*(long *)(*param_2 + lVar5) + 7));
          puVar6 = local_198;
          FUN_037ef920(auStack_80,local_c0,local_a0);
          uVar3 = FUN_037feb68(auStack_80);
          puVar6[local_1a8 * 4] = uVar3;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(auStack_80);
          puVar6 = local_198;
          FUN_037ef920(auStack_60,local_100,local_e0);
          uVar4 = FUN_037feb68(auStack_60);
          *(undefined4 *)((long)puVar6 + local_1b0 + -8) = uVar4;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(auStack_60);
          puVar6 = local_198;
          FUN_037ef920(auStack_40,local_140,local_120);
          uVar4 = FUN_037feb68(auStack_40);
          *(undefined4 *)((long)puVar6 + local_1b0 + -4) = uVar4;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(auStack_40);
          puVar1 = (undefined4 *)((long)local_198 + local_1b0);
          FUN_037ef920(local_20,local_180,local_160);
          uVar4 = FUN_037feb68(local_20);
          *puVar1 = uVar4;
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_20);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_a0);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_c0);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_e0);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_100);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_120);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_140);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_160);
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_180);
          local_1b0 = local_1b0 + 0x10;
        }
      }
      for (lVar5 = 0; (int)lVar5 < param_3; lVar5 = lVar5 + 1) {
        *(char *)(param_4 + param_5 + lVar5) = (char)local_198[lVar5];
      }
    }
    iVar2 = param_3;
    if (param_6 == 0) {
      iVar2 = FUN_037fe634(param_3);
    }
    if (0 < iVar2 - param_3) {
      lVar5 = 0;
      do {
        *(undefined1 *)(param_4 + param_3 + lVar5) = 0;
        lVar5 = lVar5 + 1;
      } while ((int)lVar5 < iVar2 - param_3);
    }
  }
  else {
    iVar2 = 0;
  }
  FUN_037f7580(&local_198);
  return iVar2;
}


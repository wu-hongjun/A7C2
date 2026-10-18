// Decompiled pseudocode, not original source.
// REA Evidence: ev_7f2467b9e9ae298652da5ef78151627a377a277a2b8a4215187f65f6a1047637
// Source record: research/evidence/rea/still-sink-virtual-r2/11-procedure_pseudo_code.json

undefined8 _ZN7libheif3getEPN5still8get_if_tEN3mpr10StillImageE(long param_1,long param_2)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  long lVar5;
  uint *puVar6;
  ulong *puVar7;
  undefined4 local_284;
  undefined4 local_280;
  uint local_27c;
  undefined1 local_270 [108];
  undefined4 local_204;
  undefined4 local_200;
  undefined4 local_1f4;
  undefined4 local_1f0;
  undefined4 local_1e8;
  uint local_1e4;
  uint local_1d8 [27];
  undefined4 local_16c;
  undefined4 local_168;
  undefined4 local_150;
  uint local_14c;
  uint local_140 [27];
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_b8;
  uint local_b4;
  uint local_a8 [27];
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_20;
  
  if (*(long **)(param_1 + 0x1548) != (long *)0x0) {
    (**(code **)(**(long **)(param_1 + 0x1548) + 0x60))();
  }
  if ((*(long *)(param_1 + 0x1550) != 0) &&
     (iVar3 = FUN_00379cf4(*(long *)(param_1 + 0x1550),&local_280), iVar3 == 0)) {
    lVar5 = *(long *)(param_2 + 8);
    if (lVar5 != 0) {
      *(undefined1 *)(lVar5 + 0x1d) = (undefined1)local_280;
      *(undefined1 *)(lVar5 + 0x1e) = local_280._1_1_;
      *(undefined1 *)(lVar5 + 0x1c) = local_270[0];
    }
    *(undefined1 *)(param_2 + 0x3fa) = 1;
  }
  memset(&local_280,0,0x27c);
  FUN_0035d348(*(undefined8 *)(param_1 + 0x1488),1,&local_280);
  local_284 = 0;
  *(uint *)(param_2 + 0x10) = local_27c;
  puVar6 = (uint *)local_270;
  for (puVar7 = (ulong *)(param_2 + 0x18);
      puVar7 != (ulong *)(param_2 + (ulong)local_27c * 0x10 + 0x18); puVar7 = puVar7 + 2) {
    *puVar7 = (ulong)*puVar6;
    puVar7[1] = (ulong)puVar6[1];
    puVar6 = puVar6 + 3;
  }
  *(undefined4 *)(param_2 + 0x3b4) = local_204;
  *(undefined4 *)(param_2 + 0x3b8) = local_200;
  uVar4 = FUN_0036a150(param_1,local_1e8);
  *(undefined4 *)(param_2 + 0x3bc) = uVar4;
  lVar5 = *(long *)(*(long *)(param_1 + 0x1548) + 0x38);
  if (lVar5 != 0) {
    cVar2 = FUN_001c77a4(lVar5,0x213,0,&local_284);
    if (cVar2 == '\0') {
      uVar4 = 2;
    }
    else {
      uVar4 = _ZN8libstill18to_mpr_ChromaPhaseEj(param_1,local_284);
    }
    *(undefined4 *)(param_2 + 0x3c0) = uVar4;
  }
  *(uint *)(param_2 + 0xd8) = local_1e4;
  if (local_1e4 != 0) {
    *(undefined1 *)(param_2 + 0x3f5) = 1;
    puVar6 = local_1d8;
    for (puVar7 = (ulong *)(param_2 + 0xe8);
        puVar7 != (ulong *)(param_2 + (ulong)local_1e4 * 0x10 + 0xe8); puVar7 = puVar7 + 2) {
      puVar7[-1] = (ulong)*puVar6;
      *puVar7 = (ulong)puVar6[1];
      puVar6 = puVar6 + 3;
    }
  }
  *(undefined4 *)(param_2 + 0x3c4) = local_16c;
  *(undefined4 *)(param_2 + 0x3c8) = local_168;
  uVar4 = FUN_0036a150(param_1,local_150);
  *(undefined4 *)(param_2 + 0x3cc) = uVar4;
  lVar5 = *(long *)(*(long *)(param_1 + 0x1548) + 0x38);
  if (lVar5 != 0) {
    cVar2 = FUN_001c77a4(lVar5,0x213,0,&local_284);
    if (cVar2 == '\0') {
      uVar4 = 2;
    }
    else {
      uVar4 = _ZN8libstill18to_mpr_ChromaPhaseEj(param_1,local_284);
    }
    *(undefined4 *)(param_2 + 0x3d0) = uVar4;
  }
  *(uint *)(param_2 + 0x2a0) = local_b4;
  puVar6 = local_a8;
  for (puVar7 = (ulong *)(param_2 + 0x2a8);
      puVar7 != (ulong *)(param_2 + (ulong)local_b4 * 0x10 + 0x2a8); puVar7 = puVar7 + 2) {
    *puVar7 = (ulong)*puVar6;
    puVar7[1] = (ulong)puVar6[1];
    puVar6 = puVar6 + 3;
  }
  *(undefined4 *)(param_2 + 0x3e4) = local_3c;
  *(undefined4 *)(param_2 + 1000) = local_38;
  uVar4 = FUN_0036a150(param_1,local_20);
  *(undefined4 *)(param_2 + 0x3ec) = uVar4;
  *(undefined4 *)(param_2 + 0x3f0) = 0;
  *(uint *)(param_2 + 0x208) = local_14c;
  puVar6 = local_140;
  for (puVar7 = (ulong *)(param_2 + 0x218);
      (ulong *)(param_2 + (ulong)local_14c * 0x10 + 0x218) != puVar7; puVar7 = puVar7 + 2) {
    puVar7[-1] = (ulong)*puVar6;
    *puVar7 = (ulong)puVar6[1];
    puVar6 = puVar6 + 3;
  }
  *(undefined4 *)(param_2 + 0x3d4) = local_d4;
  *(undefined4 *)(param_2 + 0x3d8) = local_d0;
  uVar4 = FUN_0036a150(param_1,local_b8);
  *(undefined4 *)(param_2 + 0x3dc) = uVar4;
  *(undefined4 *)(param_2 + 0x3e0) = 1;
  uVar4 = FUN_0036a130(param_1,local_280);
  *(undefined4 *)(param_2 + 0x380) = uVar4;
  lVar5 = *(long *)(param_2 + 8);
  if (lVar5 != 0) {
    FUN_0036a250(param_1,local_1f4,lVar5 + 0x1f,lVar5 + 0x20);
  }
  lVar5 = *(long *)(param_2 + 8);
  if (lVar5 != 0) {
    FUN_0036a308(param_1,local_1f0,lVar5 + 0x14,lVar5 + 0x18);
  }
  *(undefined4 *)(param_2 + 0x358) = *(undefined4 *)(param_1 + 0x1490);
  *(undefined8 *)(param_2 + 0x360) = *(undefined8 *)(param_1 + 0x1498);
  *(ulong *)(param_2 + 0x348) = (ulong)*(uint *)(param_1 + 0x1520);
  *(ulong *)(param_2 + 0x350) = (ulong)*(uint *)(param_1 + 0x1524);
  lVar5 = *(long *)(param_2 + 8);
  if (lVar5 != 0) {
    uVar1 = *(undefined1 *)(param_1 + 0x14a1);
    *(undefined1 *)(lVar5 + 0x25) = *(undefined1 *)(param_1 + 0x14a0);
    *(undefined1 *)(lVar5 + 0x27) = uVar1;
    _ZN3mpr8timeconv33ConvTrustUniversalTimeToLocalTimeEPN9DefStruct13UniversalTimeEsPNS1_9LocalTimeE
              (param_1 + 0x1528,*(undefined2 *)(lVar5 + 0x36),lVar5 + 0x28);
  }
  return 0;
}


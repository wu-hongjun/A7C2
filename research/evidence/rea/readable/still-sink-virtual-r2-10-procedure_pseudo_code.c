// Decompiled pseudocode, not original source.
// REA Evidence: ev_b2458ea857f333a209f7720b586a0b7423ff752f6f7e5aef454e914811e59597
// Source record: research/evidence/rea/still-sink-virtual-r2/10-procedure_pseudo_code.json

undefined8 _ZN7libjpeg3getEPN5still8get_if_tEN3mpr10StillImageE(long param_1,long param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  long lVar4;
  long *plVar5;
  undefined4 local_18;
  undefined1 local_14;
  undefined1 local_13;
  undefined1 local_12;
  undefined1 local_11;
  undefined4 local_10;
  uint uStack_c;
  undefined1 local_8;
  
  if (*(long **)(param_1 + 0x1488) != (long *)0x0) {
    (**(code **)(**(long **)(param_1 + 0x1488) + 0x60))();
  }
  if ((*(long *)(param_1 + 0x1490) != 0) &&
     (iVar2 = FUN_00379cf4(*(long *)(param_1 + 0x1490),&local_18), iVar2 == 0)) {
    lVar4 = *(long *)(param_2 + 8);
    if (lVar4 != 0) {
      *(undefined1 *)(lVar4 + 0x1d) = (undefined1)local_18;
      *(undefined1 *)(lVar4 + 0x1e) = local_18._1_1_;
      *(undefined1 *)(lVar4 + 0x1c) = local_8;
    }
    *(undefined1 *)(param_2 + 0x3fa) = 1;
  }
  plVar5 = *(long **)(param_1 + 0x14a0);
  if (plVar5 != (long *)0x0) {
    (**(code **)(*plVar5 + 0x30))(plVar5,param_2);
  }
  if (*(long *)(param_1 + 0x14a8) != 0) {
    uStack_c = *(uint *)(*(long *)(param_1 + 0x14a8) + 0x40);
    local_13 = 0;
    local_12 = 0;
    local_11 = 0;
    *(ulong *)(param_2 + 0x338) = (ulong)uStack_c;
    local_14 = 2;
    local_18 = 0x83002240;
    local_10 = *(undefined4 *)(param_1 + 0x1550);
    FUN_0036cf68(&local_18,0x10);
    *(ulong *)(param_2 + 0x340) = (ulong)*(uint *)(param_1 + 0x1550);
    FUN_003528f0(*(undefined8 *)(param_1 + 0x14a8),*(undefined8 *)(param_2 + 8));
  }
  lVar4 = *(long *)(param_2 + 8);
  if (lVar4 != 0) {
    *(undefined1 *)(lVar4 + 0x1f) = 1;
    *(undefined4 *)(lVar4 + 0x20) = 1;
  }
  lVar4 = *(long *)(*(long *)(param_1 + 0x1488) + 0x38);
  local_18 = 0;
  if (lVar4 != 0) {
    cVar1 = FUN_001c77a4(lVar4,0x213,0,&local_18);
    if (cVar1 == '\0') {
      uVar3 = 2;
    }
    else {
      uVar3 = _ZN8libstill18to_mpr_ChromaPhaseEj(param_1,local_18);
    }
    *(undefined4 *)(param_2 + 0x3c0) = uVar3;
  }
  lVar4 = *(long *)(*(long *)(param_1 + 0x1488) + 0x38);
  if (lVar4 != 0) {
    cVar1 = FUN_001c77a4(lVar4,0x213,0,&local_18);
    if (cVar1 == '\0') {
      uVar3 = 2;
    }
    else {
      uVar3 = _ZN8libstill18to_mpr_ChromaPhaseEj(param_1,local_18);
    }
    *(undefined4 *)(param_2 + 0x3d0) = uVar3;
  }
  lVar4 = *(long *)(*(long *)(param_1 + 0x1488) + 0x40);
  if (lVar4 != 0) {
    cVar1 = FUN_001c77a4(lVar4,0x213,0,&local_18);
    if (cVar1 == '\0') {
      uVar3 = 2;
    }
    else {
      uVar3 = _ZN8libstill18to_mpr_ChromaPhaseEj(param_1,local_18);
    }
    *(undefined4 *)(param_2 + 0x3e0) = uVar3;
  }
  *(undefined4 *)(param_2 + 0x3f0) = 2;
  return 0;
}


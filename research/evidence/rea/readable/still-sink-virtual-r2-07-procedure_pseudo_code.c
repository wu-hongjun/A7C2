// Decompiled pseudocode, not original source.
// REA Evidence: ev_7cdf39dbfed3a206642fded178cb5a045d0bd855b2a5a38cc875ae623a9223a8
// Source record: research/evidence/rea/still-sink-virtual-r2/07-procedure_pseudo_code.json

void _ZN3mpr11stillrecord12_make_headerEPv(long *param_1)

{
  uint uVar1;
  undefined2 uVar2;
  int iVar3;
  ulong uVar4;
  long *plVar5;
  int iVar6;
  long *plVar7;
  long lVar8;
  long *plVar9;
  int iVar10;
  ulong uVar11;
  long local_b8;
  long local_b0;
  long local_a8;
  undefined4 local_94;
  undefined1 local_90;
  undefined1 local_8f;
  undefined1 local_8d;
  undefined1 local_8b;
  int local_88;
  uint uStack_84;
  undefined1 local_80;
  undefined4 local_68;
  code *local_60;
  undefined8 local_58;
  int local_50;
  int iStack_4c;
  long local_48;
  undefined4 local_40;
  undefined4 local_3c;
  undefined1 local_38;
  undefined1 local_36;
  undefined1 local_34;
  undefined1 local_32;
  int local_30;
  uint uStack_2c;
  undefined1 local_28;
  undefined4 local_10;
  code *local_8;
  
  plVar7 = param_1 + 1;
  uVar4 = 0;
  lVar8 = 0x676;
  iVar6 = 0;
  plVar9 = plVar7;
  do {
    iVar3 = (int)uVar4;
    if (*(int *)(*param_1 + uVar4 * 0x588 + 0x20c) != -1) {
      if (*plVar9 != 0) {
        iVar6 = iVar6 + 1;
        local_58 = 0x31b002120;
        local_50 = iVar3;
        FUN_001df8d4(&local_58,0xc);
      }
      if (plVar9[2] != 0) {
        iVar6 = iVar6 + 1;
        local_58 = 0x31b012120;
        local_50 = iVar3;
        FUN_001df8d4(&local_58,0xc);
      }
      local_b8 = lVar8 + -0x43a;
      uVar11 = 0;
      do {
        iVar10 = (int)uVar11;
        if (*(char *)(*param_1 + uVar4 * 0x588 + uVar11 * 0x106 + 0x23c) != '\0') {
          local_58 = 0x21b022120;
          local_50 = iVar3;
          iStack_4c = iVar10;
          FUN_001df8d4(&local_58,0x10);
          if (*param_1 + local_b8 != 0) {
            FUN_00195048(0x2120,0x1b03,*param_1 + local_b8);
          }
          if (*(char *)(*param_1 + uVar4 * 0x588 + uVar11 + 0x238) == '\0') {
            uVar2 = 0x1b05;
          }
          else {
            uVar2 = 0x1b04;
          }
          local_58._0_4_ = CONCAT22(uVar2,0x2120);
          local_58 = (ulong)CONCAT24(2,(undefined4)local_58);
          local_50 = iVar3;
          iStack_4c = iVar10;
          FUN_001df8d4(&local_58,0x10);
        }
        uVar11 = (ulong)(iVar10 + 1U);
        local_b8 = local_b8 + 0x106;
      } while (iVar10 + 1U != 4);
      uVar2 = 0x1b06;
      if (*(char *)(*param_1 + uVar4 * 0x588 + 0x674) == '\0') {
        uVar2 = 0x1b07;
      }
      local_58._0_4_ = CONCAT22(uVar2,0x2120);
      local_58 = (ulong)CONCAT24(3,(undefined4)local_58);
      local_50 = iVar3;
      FUN_001df8d4(&local_58,0xc);
      uVar2 = 0x1b08;
      if (*(char *)(*param_1 + uVar4 * 0x588 + 0x675) == '\0') {
        uVar2 = 0x1b09;
      }
      local_58._0_4_ = CONCAT22(uVar2,0x2120);
      local_58 = (ulong)CONCAT24(3,(undefined4)local_58);
      local_50 = iVar3;
      FUN_001df8d4(&local_58,0xc);
      if (*(char *)(*param_1 + uVar4 * 0x588 + 0x676) != '\0') {
        local_58 = 0x31b0a2120;
        local_50 = iVar3;
        FUN_001df8d4(&local_58,0xc);
        if (*param_1 + lVar8 != 0) {
          FUN_00195048(0x2120,0x1b0b,*param_1 + lVar8);
        }
      }
    }
    uVar4 = (ulong)(iVar3 + 1U);
    lVar8 = lVar8 + 0x588;
    plVar9 = plVar9 + 1;
  } while (iVar3 + 1U != 2);
  local_58 = 0x31c032120;
  local_50 = iVar6;
  FUN_001df8d4(&local_58,0xc);
  plVar5 = param_1 + 5;
  _ZN3mpr11stillrecord21RegistProcessingOrderEPNS_18recordStillParam_tEj(*param_1,iVar6);
  plVar9 = param_1;
  do {
    uVar1 = (int)plVar9 - (int)param_1;
    if (*(int *)(*param_1 + (ulong)uVar1 * 0x588 + 0x20c) != -1) {
      _ZN3mpr15broadcast_queue6exec_tC2Ev(&local_b0);
      local_80 = 0;
      local_b0 = *param_1;
      local_a8 = *plVar7;
      local_68 = 0;
      local_94 = (undefined4)param_1[6];
      local_60 = _ZN3mpr11stillrecord19FinalizeStillHeaderEPv;
      local_90 = *(undefined1 *)((long)param_1 + 0x34);
      local_8f = *(undefined1 *)((long)plVar9 + 0x35);
      local_8d = *(undefined1 *)((long)plVar9 + 0x39);
      local_8b = *(undefined1 *)((long)plVar9 + 0x3d);
      local_88 = iVar6;
      uStack_84 = uVar1;
      _ZN3mpr15broadcast_queue4execERNS0_6exec_tE(&local_b0);
      if (plVar7[2] != 0) {
        _ZN3mpr15broadcast_queue6exec_tC2Ev(&local_58);
        local_58 = *param_1;
        local_10 = 0;
        local_48 = plVar7[2];
        local_8 = _ZN3mpr11stillrecord19FinalizeStillHeaderEPv;
        local_40 = (undefined4)*plVar5;
        local_3c = (undefined4)param_1[6];
        local_38 = *(undefined1 *)((long)param_1 + 0x34);
        local_36 = *(undefined1 *)((long)plVar9 + 0x37);
        local_34 = *(undefined1 *)((long)plVar9 + 0x3b);
        local_32 = *(undefined1 *)((long)plVar9 + 0x3f);
        local_28 = 1;
        local_30 = iVar6;
        uStack_2c = uVar1;
        _ZN3mpr15broadcast_queue4execERNS0_6exec_tE(&local_58);
      }
    }
    plVar9 = (long *)((long)plVar9 + 1);
    plVar7 = plVar7 + 1;
    plVar5 = (long *)((long)plVar5 + 4);
  } while (plVar9 != (long *)((long)param_1 + 2));
  _ZN3mpr8capidtbl11AddCapIdTblEPN9DefStruct9LocalTimeE(0);
  return;
}


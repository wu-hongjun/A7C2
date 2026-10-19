// Decompiled pseudocode, not original source.
// REA Evidence: ev_08dd9c726aa3de702cfca02e5e3f85ff0398ea7178694253448a574cf1d5f1df
// Source record: research/evidence/rea/sony-mpr-completion/01-procedure_pseudo_code.json

void _ZN3mpr11stillrecord15_write_finishedEPv(long *param_1)

{
  long lVar1;
  undefined2 local_68;
  undefined2 local_66;
  undefined1 local_64;
  undefined1 local_63;
  undefined1 local_62;
  undefined1 local_61;
  uint local_60;
  undefined4 local_5c;
  undefined2 local_58;
  undefined2 local_56;
  undefined4 local_54;
  uint local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined8 local_44;
  undefined8 uStack_3c;
  undefined8 local_34;
  undefined8 uStack_2c;
  undefined8 local_24;
  undefined8 uStack_1c;
  undefined8 local_14;
  undefined8 uStack_c;
  undefined4 local_4;

  if (*param_1 != -0x15f0) {
    FUN_00195048(0x2120,0x5900);
  }
  local_56 = 64000;
  local_50 = (uint)((ulong)param_1 >> 0x20);
  local_4c = SUB84(param_1,0);
  local_54 = 2;
  local_58 = 0x2120;
  FUN_001df8d4(&local_58,0x10);
  FUN_002356c0(*param_1 + 0x15f0,*(undefined4 *)(*param_1 + 0x15e0));
  local_54 = 3;
  local_58 = 0x2120;
  local_56 = 0xfa01;
  local_50 = *(undefined4 *)(*param_1 + 0x15e0);
  FUN_001df8d4(&local_58,0xc);
  lVar1 = *param_1;
  if ((int)param_1[1] != 0) {
    *(int *)(&DAT_00481a10 + (ulong)*(uint *)(lVar1 + 0x15e0) * 4) = (int)param_1[1];
  }
  local_54 = 3;
  local_58 = 0x2120;
  local_56 = 0xfa02;
  local_50 = (uint)*(byte *)(lVar1 + 0x15e4);
  FUN_001df8d4(&local_58,0xc);
  lVar1 = *param_1;
  if (*(char *)(lVar1 + 0x15e4) != '\0') {
    if (*(int *)(&DAT_00481a10 + (ulong)*(uint *)(lVar1 + 0x15e0) * 4) != 0) {
      *(int *)(lVar1 + 0x14) = *(int *)(&DAT_00481a10 + (ulong)*(uint *)(lVar1 + 0x15e0) * 4);
    }
    *(undefined4 *)(lVar1 + 0x1c) = *(undefined4 *)(lVar1 + 0x15e8);
    _ZN3mpr27evtStillWriteCountdownParamC2Ev(&local_58);
    local_60 = 0xffffffff;
    if (*(uint *)(*param_1 + 0x15e0) < 4) {
      local_60 = (&DAT_00474d2c)[(ulong)*(uint *)(*param_1 + 0x15e0) * 0x20];
    }
    local_66 = 0xfa03;
    local_64 = 3;
    local_63 = 0;
    local_62 = 0;
    local_61 = 0;
    local_68 = 0x2120;
    local_50 = local_60;
    FUN_001df8d4(&local_68,0xc);
    lVar1 = *param_1;
    local_4c = *(undefined4 *)(lVar1 + 0x10);
    local_48 = *(undefined4 *)(lVar1 + 0x20);
    local_44 = *(undefined8 *)(lVar1 + 0x24);
    uStack_3c = *(undefined8 *)(lVar1 + 0x2c);
    local_34 = *(undefined8 *)(lVar1 + 0x34);
    uStack_2c = *(undefined8 *)(lVar1 + 0x3c);
    local_24 = *(undefined8 *)(lVar1 + 0x44);
    uStack_1c = *(undefined8 *)(lVar1 + 0x4c);
    local_14 = *(undefined8 *)(lVar1 + 0x54);
    uStack_c = *(undefined8 *)(lVar1 + 0x5c);
    local_4 = *(undefined4 *)(lVar1 + 0x14);
    local_54 = *(undefined4 *)(lVar1 + 0x15d0);
    local_64 = 3;
    local_63 = 0;
    local_62 = 0;
    local_61 = 0;
    local_68 = 0x2120;
    local_60 = *(undefined4 *)(lVar1 + 0x15e0);
    local_66 = 0xfa04;
    FUN_001df8d4(&local_68,0xc);
    _ZN3mpr11stillrecord30SetEvtStillWriteCountdownParamEPNS_18recordStillParam_tEjNS_5EVTIDERNS_27evtStillWriteCountdownParamEm
              (*(undefined8 *)(*param_1 + 0x1670),*(undefined4 *)(*param_1 + 0x15e0),0x100d,
               &local_58,0x58);
    local_63 = 0;
    local_62 = 0;
    local_64 = 2;
    local_61 = 0;
    local_66 = 0xfa05;
    local_60 = *(uint *)(*param_1 + 0x15e0);
    local_68 = 0x2120;
    local_5c = *(undefined4 *)(*(long *)(*param_1 + 0x1670) + (ulong)local_60 * 4 + 0x130);
    FUN_001df8d4(&local_68,0x10);
    local_64 = 3;
    local_63 = 0;
    local_62 = 0;
    local_66 = 0xfa06;
    local_61 = 0;
    local_68 = 0x2120;
    local_60 = *(undefined4 *)(*param_1 + 0x10);
    FUN_001df8d4(&local_68,0xc);
    _ZN3mpr11stillrecord15SetWriteDbParamEPNS_18recordStillParam_tEjRN9filequeue6exec_tE
              (*(undefined8 *)(*param_1 + 0x1670),*(undefined4 *)(*param_1 + 0x15e0));
    *(undefined4 *)(&DAT_00481a10 + (ulong)*(uint *)(*param_1 + 0x15e0) * 4) = 0;
  }
  _ZN3mpr11stillrecord20SetFsysWriteFinishedEPNS_18recordStillParam_tEj
            (*(undefined8 *)(*param_1 + 0x1670),*(undefined4 *)(*param_1 + 0x1678));
  if (*(int *)(*param_1 + 0x14) == 0) {
    local_54 = 3;
    local_58 = 0x2120;
    local_56 = 0x5901;
    local_50 = 0;
    FUN_001df8d4(&local_58,0xc);
    FUN_0022a7a0(*(undefined4 *)(*param_1 + 0x15e0),-(ulong)*(uint *)(*param_1 + 0x15ec),param_1[2])
    ;
  }
  free((void *)*param_1);
  local_54 = 4;
  local_58 = 0x2120;
  local_56 = 0x590f;
  FUN_001df8d4(&local_58,8);
  return;
}

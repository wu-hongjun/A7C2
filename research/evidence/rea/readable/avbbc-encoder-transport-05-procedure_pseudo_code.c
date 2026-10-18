// Decompiled pseudocode, not original source.
// REA Evidence: ev_e40860c846a793643c8dc99c10135bd4f4872a7a1ce4dc4247a6d30092cb233d
// Source record: research/evidence/rea/avbbc-encoder-transport/05-procedure_pseudo_code.json

void _ZN36InfraStillEncoderSeqResizeEncForHeif12SetParameterEPKh(long param_1,long param_2)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  undefined8 uVar9;
  undefined8 uVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  undefined8 uVar14;
  undefined8 uVar15;
  undefined8 uVar16;
  undefined8 uVar17;
  undefined8 uVar18;
  undefined8 uVar19;
  undefined8 uVar20;
  undefined8 uVar21;
  undefined8 uVar22;
  undefined8 uVar23;
  undefined8 uVar24;
  undefined8 uVar25;
  undefined8 uVar26;
  undefined8 uVar27;
  undefined8 uVar28;
  undefined8 uVar29;
  undefined8 uVar30;
  undefined8 uVar31;
  undefined8 uVar32;
  ulong uVar33;
  undefined4 uVar34;
  long lVar35;
  long local_178;
  undefined4 local_170;
  undefined4 local_16c;
  undefined2 local_168;
  undefined4 local_164;
  undefined8 local_160;
  undefined4 local_158;
  undefined4 local_150;
  undefined4 local_14c;
  undefined2 local_148;
  undefined4 local_144;
  undefined8 local_140;
  undefined4 local_138;
  undefined4 local_130;
  undefined4 local_12c;
  undefined2 local_128;
  undefined4 local_124;
  undefined8 local_120;
  undefined4 local_118;
  undefined4 local_110;
  undefined4 local_10c;
  undefined2 local_108;
  undefined4 local_104;
  undefined8 local_100;
  undefined4 local_f8;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined2 local_e8;
  undefined4 local_e4;
  undefined8 local_e0;
  undefined4 local_d8;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined2 local_c8;
  undefined4 local_c4;
  undefined8 local_c0;
  undefined4 local_b8;
  undefined1 auStack_b0 [2];
  undefined2 local_ae;
  undefined4 local_9c;
  undefined4 local_8c;
  undefined4 local_80 [32];
  
  lVar35 = *(long *)(param_2 + 0x30);
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))(*(long **)(param_1 + 0x1c0),0x104a,lVar35,0x80)
  ;
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt
            (0x11,0x201005a,*(undefined1 *)(lVar35 + 0x60),*(undefined1 *)(lVar35 + 0x61),
             *(undefined1 *)(lVar35 + 99),*(undefined1 *)(lVar35 + 100),
             *(undefined1 *)(lVar35 + 0x70));
  _ZN13InfraAvbbcLog7SetLog1Ejjjttt
            (0x11,0x201005b,*(undefined4 *)(lVar35 + 0x78),*(undefined1 *)(lVar35 + 0x71),
             *(undefined1 *)(lVar35 + 0x72),*(undefined1 *)(lVar35 + 0x74));
  uVar5 = *(uint *)(lVar35 + 0x34);
  local_168 = 0;
  local_148 = 0;
  local_128 = 0;
  local_108 = 0;
  local_e8 = 0;
  local_c8 = 0;
  local_16c = 0;
  local_164 = 0;
  local_160 = 0;
  local_158 = 0;
  local_14c = 0;
  local_144 = 0;
  local_140 = 0;
  local_138 = 0;
  local_12c = 0;
  local_124 = 0;
  local_120 = 0;
  local_118 = 0;
  local_10c = 0;
  local_104 = 0;
  local_100 = 0;
  local_f8 = 0;
  local_ec = 0;
  local_e4 = 0;
  local_e0 = 0;
  local_d8 = 0;
  local_cc = 0;
  local_c4 = 0;
  local_c0 = 0;
  local_b8 = 0;
  local_170 = 0xe0b00;
  local_130 = 0xe0b00;
  local_f0 = 0xe0b00;
  local_150 = 0x61f;
  local_d0 = 0x61c;
  local_110 = 0x61b;
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))
            (*(long **)(param_1 + 0x1c0),0x1057,&local_170,0x20);
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))
            (*(long **)(param_1 + 0x1c0),0x1058,&local_150,0x20);
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))
            (*(long **)(param_1 + 0x1c0),0x1059,&local_130,0x20);
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))
            (*(long **)(param_1 + 0x1c0),0x105a,&local_110,0x20);
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))
            (*(long **)(param_1 + 0x1c0),0x105b,&local_f0,0x20);
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0x60))
            (*(long **)(param_1 + 0x1c0),0x105c,&local_d0,0x20);
  uVar6 = _Znwm(0x28);
  _ZN28ExecCmd_MC_StillEncOpenCloseC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjtj
            (uVar6,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x532,*(undefined4 *)(lVar35 + 0x78));
  uVar7 = _Znwm(0x30);
  _ZN24ExecCmd_DMM_OpenUnitMainC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjh
            (uVar7,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x2c),3);
  uVar8 = _Znwm(0x30);
  _ZN28ExecCmd_DMM_Alloc_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj9cpu_types
            (uVar8,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1057,0x1043,0);
  uVar9 = _Znwm(0x30);
  _ZN28ExecCmd_DMM_Alloc_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj9cpu_types
            (uVar9,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1058,0x1043,0);
  uVar10 = _Znwm(0x30);
  _ZN24ExecCmd_SDT_ResizeRotateC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjjjh
            (uVar10,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1043,0x1057,0x1058,3);
  uVar11 = _Znwm(0x30);
  _ZN21ExecCmd_MC_EncodeHeifC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjhjb
            (uVar11,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1058,3,0x1043,uVar5 >> 0xf & 1);
  uVar12 = _Znwm(0x30);
  _ZN25ExecCmd_DMM_CloseUnitMainC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjh
            (uVar12,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x2c),3);
  uVar13 = _Znwm(0x28);
  _ZN27ExecCmd_DMM_Free_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj
            (uVar13,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1057,0x1043);
  uVar14 = _Znwm(0x28);
  _ZN27ExecCmd_DMM_Free_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj
            (uVar14,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1058,0x1043);
  uVar15 = _Znwm(0x30);
  _ZN24ExecCmd_DMM_OpenUnitMainC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjh
            (uVar15,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x2c),4);
  uVar16 = _Znwm(0x30);
  _ZN28ExecCmd_DMM_Alloc_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj9cpu_types
            (uVar16,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1059,0x1043,0);
  uVar17 = _Znwm(0x30);
  _ZN28ExecCmd_DMM_Alloc_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj9cpu_types
            (uVar17,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105a,0x1043,0);
  uVar18 = _Znwm(0x30);
  _ZN24ExecCmd_SDT_ResizeRotateC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjjjh
            (uVar18,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1043,0x1059,0x105a,4);
  uVar19 = _Znwm(0x30);
  _ZN21ExecCmd_MC_EncodeHeifC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjhjb
            (uVar19,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105a,4,0x1043,1);
  uVar20 = _Znwm(0x30);
  _ZN25ExecCmd_DMM_CloseUnitMainC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjh
            (uVar20,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x2c),4);
  uVar21 = _Znwm(0x28);
  _ZN27ExecCmd_DMM_Free_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj
            (uVar21,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1059,0x1043);
  uVar22 = _Znwm(0x28);
  _ZN27ExecCmd_DMM_Free_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj
            (uVar22,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105a,0x1043);
  uVar23 = _Znwm(0x30);
  _ZN24ExecCmd_DMM_OpenUnitMainC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjh
            (uVar23,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x2c),5);
  uVar24 = _Znwm(0x30);
  _ZN28ExecCmd_DMM_Alloc_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj9cpu_types
            (uVar24,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105b,0x1043,0);
  uVar25 = _Znwm(0x30);
  _ZN28ExecCmd_DMM_Alloc_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj9cpu_types
            (uVar25,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105c,0x1043,0);
  uVar26 = _Znwm(0x30);
  _ZN24ExecCmd_SDT_ResizeRotateC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjjjh
            (uVar26,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x1043,0x105b,0x105c,5);
  uVar27 = _Znwm(0x30);
  _ZN21ExecCmd_MC_EncodeHeifC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjhjb
            (uVar27,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105c,5,0x1043,1);
  uVar28 = _Znwm(0x30);
  _ZN25ExecCmd_DMM_CloseUnitMainC2EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjh
            (uVar28,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x2c),5);
  uVar29 = _Znwm(0x28);
  _ZN27ExecCmd_DMM_Free_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj
            (uVar29,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105b,0x1043);
  uVar30 = _Znwm(0x28);
  _ZN27ExecCmd_DMM_Free_usePrmCtrlC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjjj
            (uVar30,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x105c,0x1043);
  uVar31 = _Znwm(0x28);
  _ZN28ExecCmd_MC_StillEncOpenCloseC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEjtj
            (uVar31,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8),0x533,0);
  uVar32 = _Znwm(0x28);
  _ZN21ExecCmd_ResizeEncCompC1EP20InfraAvbbcMsgManagerPN14AvbbcParamCtrl16ParameterControlEj
            (uVar32,*(undefined8 *)(param_1 + 8),*(undefined8 *)(param_1 + 0x1c0),
             *(undefined4 *)(param_1 + 0x1b8));
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,1,uVar6);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,5,uVar7);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,2,uVar8);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,3,uVar9);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,4,uVar10);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,6,uVar11);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,7,uVar12);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,8,uVar13);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,9,uVar14);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0xd,uVar15);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,10,uVar16);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0xb,uVar17);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0xc,uVar18);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0xe,uVar19);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0xf,uVar20);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x10,uVar21);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x11,uVar22);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x15,uVar23);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x12,uVar24);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x13,uVar25);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x14,uVar26);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x16,uVar27);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x17,uVar28);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x18,uVar29);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x19,uVar30);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x1a,uVar31);
  _ZN17InfraAvbbcSeqBase10SetExecCmdEjP10IExecCmdIf(param_1,0x1b,uVar32);
  _ZN20InfraAvbbcMsgManager11RES_PAYLOADC2Ev(auStack_b0);
  local_9c = 0;
  local_ae = 0x1002;
  local_8c = 0x4e0;
  _ZN13InfraAvbbcLog7SetLog1Ejjjttt(0x11,0x2010014,0x4e0,0x1002,0,0);
  _ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj
            (*(undefined8 *)(param_1 + 8),*(undefined4 *)(param_1 + 0x1b8),auStack_b0,0x2c);
  local_178 = 0;
  (**(code **)(**(long **)(param_1 + 0x1c0) + 0xb0))(*(long **)(param_1 + 0x1c0),0x105e,&local_178);
  uVar33 = 1;
  local_80[0] = 1;
  if (*(int *)(local_178 + 0x54) != 0) {
    if ((uVar5 & 0x8000) == 0) {
      local_80[1] = 5;
      local_80[2] = 6;
      local_80[3] = 7;
      uVar33 = 4;
    }
    else {
      local_80[1] = 2;
      local_80[2] = 3;
      local_80[3] = 4;
      local_80[4] = 5;
      local_80[5] = 6;
      local_80[6] = 7;
      local_80[7] = 8;
      uVar33 = 9;
      local_80[8] = 9;
    }
  }
  if (*(int *)(local_178 + 0x60) != 0) {
    local_80[uVar33] = 10;
    iVar4 = (int)uVar33;
    local_80[iVar4 + 1] = 0xb;
    local_80[iVar4 + 2] = 0xc;
    local_80[iVar4 + 3] = 0xd;
    local_80[iVar4 + 4] = 0xe;
    local_80[iVar4 + 5] = 0xf;
    uVar33 = (ulong)(iVar4 + 8);
    local_80[iVar4 + 6] = 0x10;
    local_80[iVar4 + 7] = 0x11;
  }
  uVar5 = (uint)uVar33;
  if (*(int *)(local_178 + 0x6c) != 0) {
    local_80[uVar33] = 0x12;
    local_80[uVar5 + 1] = 0x13;
    local_80[uVar5 + 2] = 0x14;
    local_80[uVar5 + 3] = 0x15;
    local_80[uVar5 + 4] = 0x16;
    local_80[uVar5 + 5] = 0x17;
    uVar1 = uVar5 + 6;
    uVar2 = uVar5 + 7;
    uVar5 = uVar5 + 8;
    local_80[uVar1] = 0x18;
    local_80[uVar2] = 0x19;
  }
  lVar35 = 0;
  local_80[uVar5] = 0x1a;
  local_80[uVar5 + 1] = 0x1b;
  uVar34 = 0;
  do {
    uVar3 = local_80[lVar35];
    lVar35 = lVar35 + 1;
    _ZN17InfraAvbbcSeqBase13SetNextStatusEjj(param_1,uVar3,uVar34);
    uVar34 = uVar3;
  } while ((uint)lVar35 < uVar5 + 2);
  _ZN17InfraAvbbcSeqBase13SetNextStatusEjj(param_1,0xffff,uVar3);
  return;
}


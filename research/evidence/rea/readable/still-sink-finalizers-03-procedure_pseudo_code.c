// Decompiled pseudocode, not original source.
// REA Evidence: ev_3515dac0e2a7c2e44f6afac617b3c43f5975ddf91f83e58dd264f07b0bee9870
// Source record: research/evidence/rea/still-sink-finalizers/03-procedure_pseudo_code.json

undefined4 FUN_00370f9c(long *param_1,long *param_2)

{
  long lVar1;
  undefined4 uVar2;
  
  FUN_00370aec();
  if (*(int *)((long)param_1 + 0x13e4) == 1) {
    lVar1 = param_2[1];
    param_1[0x1e] = *param_2;
    param_1[0x1f] = lVar1;
    lVar1 = param_2[3];
    param_1[0x20] = param_2[2];
    param_1[0x21] = lVar1;
    lVar1 = param_2[5];
    param_1[0x22] = param_2[4];
    param_1[0x23] = lVar1;
    lVar1 = param_2[7];
    param_1[0x24] = param_2[6];
    param_1[0x25] = lVar1;
    lVar1 = param_2[9];
    param_1[0x26] = param_2[8];
    param_1[0x27] = lVar1;
    lVar1 = param_2[0xb];
    param_1[0x28] = param_2[10];
    param_1[0x29] = lVar1;
    lVar1 = param_2[0xd];
    param_1[0x48] = param_2[0xc];
    param_1[0x49] = lVar1;
    lVar1 = param_2[0xf];
    param_1[0x4a] = param_2[0xe];
    param_1[0x4b] = lVar1;
    lVar1 = param_2[0x11];
    param_1[0x4c] = param_2[0x10];
    param_1[0x4d] = lVar1;
    lVar1 = param_2[0x13];
    param_1[0x4e] = param_2[0x12];
    param_1[0x4f] = lVar1;
    lVar1 = param_2[0x15];
    param_1[0x50] = param_2[0x14];
    param_1[0x51] = lVar1;
    lVar1 = param_2[0x17];
    param_1[0x52] = param_2[0x16];
    param_1[0x53] = lVar1;
    lVar1 = param_2[0x19];
    param_1[0x54] = param_2[0x18];
    param_1[0x55] = lVar1;
    lVar1 = param_2[0x1b];
    param_1[0x56] = param_2[0x1a];
    param_1[0x57] = lVar1;
    lVar1 = param_2[0x1d];
    param_1[0x58] = param_2[0x1c];
    param_1[0x59] = lVar1;
    lVar1 = param_2[0x1f];
    param_1[0x5a] = param_2[0x1e];
    param_1[0x5b] = lVar1;
    lVar1 = param_2[0x21];
    param_1[0x5c] = param_2[0x20];
    param_1[0x5d] = lVar1;
    lVar1 = param_2[0x23];
    param_1[0x5e] = param_2[0x22];
    param_1[0x5f] = lVar1;
    lVar1 = param_2[0x25];
    param_1[0x60] = param_2[0x24];
    param_1[0x61] = lVar1;
    lVar1 = param_2[0x27];
    param_1[0x62] = param_2[0x26];
    param_1[99] = lVar1;
    lVar1 = param_2[0x29];
    param_1[100] = param_2[0x28];
    param_1[0x65] = lVar1;
    lVar1 = param_2[0x2b];
    param_1[0x72] = param_2[0x2a];
    param_1[0x73] = lVar1;
    lVar1 = param_2[0x2d];
    param_1[0x74] = param_2[0x2c];
    param_1[0x75] = lVar1;
    lVar1 = param_2[0x2f];
    param_1[0x76] = param_2[0x2e];
    param_1[0x77] = lVar1;
    uVar2 = (**(code **)(*param_1 + 0x18))(param_1,param_1 + 1);
    *(undefined4 *)((long)param_1 + 0x13e4) = 3;
  }
  else {
    uVar2 = 0xffffffff;
  }
  FUN_00370b0c(param_1);
  return uVar2;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_030a83676ce24e35355e5ad920717c04190a7aaf0f55cdb743197c394acdf452
// Source record: research/evidence/rea/libmpr-extra/01-procedure_pseudo_code.json

/* WARNING: Type propagation algorithm not settling */

void FUN_0021df70(long param_1)

{
  undefined1 *puVar1;
  int iVar2;
  byte bVar3;
  byte bVar4;
  undefined1 uVar5;
  undefined1 uVar6;
  undefined1 uVar7;
  undefined1 uVar8;
  undefined2 uVar9;
  ulong uVar10;
  bool bVar11;
  undefined2 uVar12;
  undefined2 uVar13;
  undefined1 uVar14;
  undefined1 uVar15;
  undefined1 uVar16;
  undefined1 uVar17;
  undefined1 uVar18;
  char cVar19;
  undefined2 uVar20;
  ushort uVar21;
  uint uVar22;
  int iVar23;
  undefined4 uVar24;
  int iVar25;
  uint uVar26;
  undefined4 uVar27;
  uint uVar28;
  uint uVar29;
  undefined8 uVar30;
  long *plVar31;
  ulong uVar32;
  ulong uVar33;
  ulong uVar34;
  void *__ptr;
  void *__s;
  ulong uVar35;
  void *pvVar36;
  long lVar37;
  ulong uVar38;
  undefined8 uVar39;
  void *pvVar40;
  void *pvVar41;
  undefined8 *puVar42;
  uint *puVar43;
  long lVar44;
  ulong uVar45;
  long lVar46;
  ulong uVar47;
  undefined1 uVar48;
  uint uVar49;
  ulong uVar50;
  uint uVar51;
  uint uVar52;
  uint uVar53;
  uint uVar54;
  uint uVar55;
  uint uVar56;
  uint uVar57;
  uint uVar58;
  uint uVar59;
  uint uVar60;
  uint uVar61;
  uint uVar62;
  uint uVar63;
  uint uVar64;
  uint uVar65;
  undefined8 uVar66;
  undefined8 uVar67;
  uint uVar68;
  long lVar69;
  long lVar70;
  ulong uVar71;
  ulong uVar72;
  ulong uVar73;
  uint uVar74;
  uint uVar75;
  uint *in_stack_ffffffffffffe620;
  undefined4 uVar76;
  uint *in_stack_ffffffffffffe628;
  ulong *in_stack_ffffffffffffe630;
  undefined4 uVar77;
  int *in_stack_ffffffffffffe638;
  undefined4 uVar78;
  undefined8 *in_stack_ffffffffffffe640;
  undefined4 uVar79;
  undefined4 local_1900;
  ulong local_18c8;
  int local_18c0;
  uint local_18bc;
  undefined4 local_18b0;
  uint local_18a8;
  int local_18a4;
  undefined4 local_18a0;
  long local_1890;
  uint local_1884;
  long local_1870;
  uint local_1868;
  int local_1864;
  int local_1860;
  uint local_185c;
  ulong local_1850;
  uint local_1848;
  uint local_1844;
  ulong local_1840;
  int local_181c;
  undefined1 auStack_1810 [80];
  undefined1 auStack_17c0 [1408];
  undefined1 auStack_1240 [512];
  byte local_1040;
  undefined1 local_103f [3];
  int local_103c;
  undefined4 local_1038;
  undefined4 local_1034;
  uint local_1030;
  undefined4 local_102c;
  uint local_1028;
  int local_1024;
  uint local_1020;
  uint local_101c;
  undefined4 local_1018;
  undefined4 local_1014;
  undefined4 local_1010;
  undefined4 local_100c;
  undefined4 local_1008;
  uint local_1004;
  undefined4 local_1000;
  undefined4 local_ffc;
  undefined8 *local_ff8;
  undefined8 local_ff0;
  undefined8 local_fe8;
  long local_fe0;
  undefined8 local_fd8;
  undefined8 local_fd0;
  undefined2 local_fc8;
  undefined2 uStack_fc6;
  undefined1 uStack_fc4;
  undefined1 uStack_fc3;
  undefined1 uStack_fc2;
  undefined1 uStack_fc1;
  undefined2 local_fc0;
  undefined2 local_fb8;
  undefined2 uStack_fb6;
  undefined1 uStack_fb4;
  undefined1 uStack_fb3;
  undefined1 uStack_fb2;
  undefined1 uStack_fb1;
  undefined2 local_fb0;
  undefined2 local_fa8;
  undefined2 uStack_fa6;
  undefined1 uStack_fa4;
  undefined1 uStack_fa3;
  undefined1 uStack_fa2;
  undefined1 uStack_fa1;
  undefined4 local_fa0;
  undefined4 local_f98;
  undefined1 local_f94;
  undefined1 local_f93;
  undefined1 local_f92;
  undefined1 local_f91;
  undefined4 local_f90;
  undefined1 auStack_f88 [16];
  undefined1 auStack_f78 [16];
  undefined1 auStack_f68 [16];
  uint local_f58;
  undefined4 uStack_f54;
  undefined8 uStack_f50;
  undefined8 local_f48;
  undefined8 uStack_f40;
  undefined8 local_f38;
  undefined1 auStack_f30 [8];
  long local_f28;
  uint local_f10 [2];
  undefined1 *local_f08;
  undefined8 local_f00;
  undefined1 local_ef8 [16];
  undefined8 local_ee8;
  undefined4 uStack_ee0;
  undefined4 local_edc;
  undefined4 uStack_ed8;
  uint local_ed4;
  undefined4 uStack_ed0;
  undefined4 local_ecc;
  uint local_ec8;
  undefined4 local_ec4;
  uint local_ec0;
  undefined4 uStack_ebc;
  undefined8 local_eb8;
  undefined4 local_eb0;
  undefined1 local_ea8;
  undefined7 uStack_ea7;
  undefined1 uStack_ea0;
  undefined8 uStack_e9f;
  undefined8 local_e97;
  undefined8 uStack_e8f;
  undefined1 *local_e80;
  undefined8 local_e78;
  undefined1 local_e70 [16];
  undefined8 local_e60;
  undefined1 auStack_e58 [8];
  undefined8 local_e50;
  undefined8 uStack_e48;
  undefined8 local_e40;
  undefined8 uStack_e38;
  undefined8 local_e30;
  undefined8 uStack_e28;
  undefined8 local_e20;
  undefined8 local_e18;
  undefined4 local_e10;
  int local_e0c;
  undefined4 local_e08;
  undefined4 local_e04;
  uint local_e00;
  uint local_dfc;
  undefined4 local_df8;
  undefined4 local_df4;
  uint local_df0;
  undefined4 local_dec;
  undefined4 local_de8;
  undefined8 local_de0;
  undefined1 local_dd8;
  ulong *local_dd0;
  undefined1 auStack_dc8 [8];
  undefined4 local_dc0;
  undefined4 local_db4;
  ulong local_d78 [10];
  undefined1 auStack_d28 [32];
  undefined1 auStack_d08 [32];
  undefined1 auStack_ce8 [32];
  undefined1 auStack_cc8 [104];
  long local_c60 [13];
  int local_bf8;
  uint local_bf4;
  ulong local_bf0;
  undefined1 auStack_be8 [24];
  int local_bd0;
  long local_bc8;
  int local_bc0;
  undefined1 auStack_bb8 [24];
  int local_ba0;
  long local_b98;
  int local_b90;
  undefined4 local_b8c;
  undefined4 local_b88;
  undefined8 local_b80;
  void *local_b78;
  long local_b70;
  int local_b60;
  uint local_b5c;
  ulong local_b58;
  long local_b50;
  long local_b48;
  int local_b38 [2];
  long local_b30;
  int local_b28;
  long local_b20 [3];
  int local_b08 [2];
  long local_b00;
  int local_af8;
  undefined4 local_af4;
  undefined4 local_af0;
  ulong local_ae8;
  undefined4 local_ad8;
  undefined1 local_ad4;
  undefined1 local_ad3;
  undefined1 local_ad2;
  undefined1 local_ad1;
  undefined4 local_ad0;
  undefined4 local_acc;
  undefined4 local_ac8;
  uint local_ac4;
  undefined1 auStack_ac0 [80];
  undefined1 local_a70;
  undefined8 local_a6f;
  undefined8 uStack_a67;
  undefined8 local_a5f;
  undefined8 uStack_a57;
  undefined1 *local_a48;
  undefined8 local_a40;
  undefined1 local_a38 [16];
  undefined4 local_a28;
  undefined4 local_a24;
  undefined1 local_a20;
  undefined1 *local_a18;
  undefined8 local_a10;
  undefined1 local_a08 [16];
  undefined4 local_9f8;
  uint local_9f4;
  undefined8 local_9f0;
  undefined8 local_9e8;
  undefined8 local_9e0;
  uint local_9d8;
  undefined4 local_9d4;
  uint local_9d0;
  undefined8 local_9cc;
  undefined8 uStack_9c4;
  undefined8 local_9bc;
  int local_9b4;
  undefined4 local_9b0;
  undefined8 local_9ac;
  undefined4 local_9a4;
  undefined8 local_9a0;
  undefined4 local_998;
  undefined1 auStack_994 [128];
  undefined8 local_914;
  undefined2 local_90c;
  undefined8 local_908;
  undefined8 local_900;
  undefined8 uStack_8f8;
  undefined8 local_8f0;
  undefined8 uStack_8e8;
  undefined1 auStack_8e0 [80];
  undefined1 auStack_890 [104];
  undefined1 auStack_828 [32];
  undefined1 auStack_808 [32];
  undefined1 auStack_7e8 [32];
  undefined4 local_7c8 [2];
  ulong local_7c0;
  undefined4 local_790;
  undefined1 auStack_78c [268];
  ulong local_680;
  ulong local_678 [65];
  undefined8 local_470;
  undefined8 local_468;
  undefined8 local_460;
  undefined8 local_458;
  undefined8 local_450;
  undefined8 uStack_448;
  uint local_440;
  undefined4 uStack_43c;
  ulong local_438;
  undefined8 local_430;
  ulong local_428;
  ulong *local_420;
  ulong local_418;
  ulong local_410;
  undefined8 local_408;
  ulong local_400;
  ulong local_3f8;
  undefined1 auStack_32a [262];
  undefined1 auStack_224 [268];
  void *local_118;
  undefined8 local_110;
  undefined8 uStack_108;
  undefined4 local_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined8 local_e8;
  ulong local_e0;
  char local_d8;
  void *local_d0;
  uint local_c8;
  undefined1 local_c4;
  void *local_c0;
  uint local_b8;
  undefined1 local_b4;
  undefined1 local_b3;
  undefined1 local_b2;
  undefined8 local_b0;
  undefined8 uStack_a8;
  undefined8 local_a0;
  undefined8 uStack_98;
  undefined8 local_90;
  undefined8 uStack_88;
  undefined8 local_80;
  undefined8 uStack_78;
  long local_70;
  ulong local_68;
  undefined8 local_60;
  undefined4 local_58;
  undefined8 local_50;
  undefined8 uStack_48;
  undefined8 local_40;
  undefined8 uStack_38;
  undefined8 local_30;
  undefined4 local_28;
  undefined8 local_20;
  undefined8 uStack_18;
  undefined4 local_10;
  long local_8;
  
  iVar23 = 0;
  local_470 = (char *)0x401702310;
  FUN_00216878(&local_470,8);
  do {
    while( true ) {
      do {
        uVar22 = FUN_00218b84(param_1,&local_103c,&local_ff8);
        puVar42 = local_ff8;
        uVar27 = (undefined4)((ulong)in_stack_ffffffffffffe628 >> 0x20);
        uVar77 = (undefined4)((ulong)in_stack_ffffffffffffe630 >> 0x20);
        uVar78 = (undefined4)((ulong)in_stack_ffffffffffffe638 >> 0x20);
        uVar79 = (undefined4)((ulong)in_stack_ffffffffffffe640 >> 0x20);
        local_18bc = uVar22 & 0xff;
      } while (local_18bc == 0);
      if (local_103c == 0x10000000) break;
      free(local_ff8);
    }
    pvVar41 = (void *)(param_1 + 0x430);
    uVar54 = 0;
    memcpy(pvVar41,local_ff8,0x550);
    free(puVar42);
    uVar76 = (undefined4)((ulong)in_stack_ffffffffffffe620 >> 0x20);
    uVar73 = *(ulong *)(param_1 + 0x958);
    *(undefined4 *)(param_1 + 0x20) = *(undefined4 *)(param_1 + 0x434);
    if (*(int *)(param_1 + 0x56c) == 2) {
      if (*(long **)(param_1 + 0x998) != (long *)0x0) {
        (**(code **)(**(long **)(param_1 + 0x998) + 8))();
      }
      *(undefined8 *)(param_1 + 0x998) = 0;
      uVar30 = _Znwm(0x20);
      FUN_00205f7c();
      *(undefined8 *)(param_1 + 0x998) = uVar30;
      uVar54 = *(uint *)(param_1 + 0x928) >> 1;
      FUN_00205f94(uVar30,*(undefined8 *)(param_1 + 0x920),uVar54);
      uVar30 = CONCAT71((int7)((ulong)in_stack_ffffffffffffe620 >> 8),1);
      iVar23 = FUN_00206038(*(undefined8 *)(param_1 + 0x998),1,*(undefined8 *)(param_1 + 0x8a8),
                            *(undefined4 *)(param_1 + 0x8b0),1,1,*(undefined8 *)(param_1 + 0x848),
                            *(undefined4 *)(param_1 + 0x850),uVar30);
      uVar76 = (undefined4)((ulong)uVar30 >> 0x20);
      if (iVar23 != 0) {
        local_470 = (char *)0x201782310;
        local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x430));
        FUN_00216878(&local_470,0x10);
        FUN_001fdd5c(uVar73,iVar23);
      }
    }
    lVar44 = *(long *)(param_1 + 0x920) + (ulong)uVar54;
    *(long *)(param_1 + 0x18) = *(long *)(param_1 + 0x930);
    iVar25 = *(int *)(param_1 + 0x928) - uVar54;
    *(int *)(param_1 + 0x10) = iVar25;
    *(long *)(param_1 + 8) = lVar44;
    plVar31 = (long *)_ZN3mpr7DmacLib11MakeDmacLibEPvjS1_NS0_22TransferMaxSizePerItemENS0_8DmacModeE
                                (lVar44,iVar25,lVar44 - *(long *)(param_1 + 0x930),0xffffffff,0);
    lVar44 = *plVar31;
    *(long **)(param_1 + 0x128) = plVar31;
    cVar19 = (**(code **)(lVar44 + 0x30))();
    if (cVar19 == '\0') {
      *(undefined8 *)(param_1 + 8) = 0;
      *(undefined4 *)(param_1 + 0x10) = 0;
      *(undefined8 *)(param_1 + 0x18) = 0;
    }
    uVar32 = _Znwm(0x68);
    FUN_00326da0();
    local_ff0 = 0;
    if (*(int *)(param_1 + 0x574) == 1) {
      uVar54 = (uint)*(ushort *)(param_1 + 0x578);
      uVar24 = 1;
LAB_0021ed64:
      local_ff0 = CONCAT44(uVar24,uVar54);
    }
    else if (*(int *)(param_1 + 0x574) == 2) {
      uVar54 = *(uint *)(param_1 + 0x580);
      uVar24 = *(undefined4 *)(param_1 + 0x57c);
      goto LAB_0021ed64;
    }
    bVar3 = *(byte *)(param_1 + 0x5b0);
    plVar31 = *(long **)(param_1 + 0x5f0);
    if (bVar3 == 0) {
      *(undefined4 *)((long)plVar31 + 0x14) = 0;
      *(undefined4 *)(plVar31 + 3) = 0;
    }
    else {
      *(undefined4 *)(plVar31 + 3) = *(undefined4 *)((long)plVar31 + 0x14);
    }
    *(undefined4 *)((long)plVar31 + 0x1c) = 0;
    local_470 = (char *)0x101812310;
    local_468 = (long *)(ulong)CONCAT14(bVar3,*(undefined4 *)(param_1 + 0x430));
    local_460 = (ulong)*(uint *)((long)plVar31 + 0x14);
    FUN_00216878(&local_470,0x18);
    local_181c = 0;
    local_1038 = 0;
    local_1034 = 0;
    local_fe8 = 0;
    uVar45 = *(ulong *)(*(long *)(param_1 + 0x5f0) + 0x28);
    uVar33 = *(ulong *)(*(long *)(param_1 + 0x5f0) + 0x38);
    if (uVar45 < uVar33) {
      uVar33 = uVar45;
    }
    if (bVar3 != 0) {
      local_181c = _ZN15libMp4Signature18ReMmapStreamBufferERKN3mpr5Codec14BufferAreaInfoERiS5_Rl
                             (param_1 + 0x848,&local_1038,&local_1034,&local_fe8);
    }
    FUN_0021a594(param_1);
    local_468 = (long *)0x0;
    local_470 = (char *)&local_460;
    local_460 = local_460 & 0xffffffffffffff00;
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEPKc(&local_470,param_1 + 0x448);
    uVar24 = _ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4findEPKcm
                       (&local_470,&DAT_003d2871,0);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7replaceEmmPKc
              (&local_470,uVar24,4,&DAT_003d0e9c);
    strncpy((char *)(param_1 + 0x9d0),local_470,0x106);
    pvVar40 = (void *)(param_1 + 0x440);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7replaceEmmPKc(&local_470,uVar24,4,".BIN");
    strncpy((char *)(param_1 + 0xad6),local_470,0x106);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_470);
    local_dd8 = 0;
    local_dfc = *(uint *)(param_1 + 0x568);
    local_e08 = 0;
    local_df8 = *(undefined4 *)(param_1 + 0x56c);
    local_e18 = *(undefined8 *)(param_1 + 0x54e);
    local_e10 = *(undefined4 *)(param_1 + 0x556);
    local_df4 = *(undefined4 *)(param_1 + 0x570);
    local_e0c = *(int *)(param_1 + 0x560);
    local_df0 = (uint)*(ushort *)(param_1 + 0x578);
    local_e04 = *(undefined4 *)(param_1 + 0x564);
    local_e00 = *(uint *)(param_1 + 0x55c);
    local_dec = 0;
    local_de8 = 0;
    local_de0 = 0;
    local_dd0 = (ulong *)0x0;
    if (*(int *)(_ZN3mpr5pconv7m_vpcStE + (long)local_e0c * 0x1c + 0x18) == 0) {
      if (*(int *)(param_1 + 0x444) == 0) {
        if (*(int *)(_ZN3mpr5pconv7m_vpcStE + (long)local_e0c * 0x1c) == 1) {
          local_e08 = 2;
          if (7 < local_e00 - 4) {
            local_e08 = 1;
          }
        }
        else {
LAB_0021ed8c:
          local_e08 = 1;
        }
      }
      else if (*(int *)(_ZN3mpr5pconv7m_vpcStE + (long)local_e0c * 0x1c) == 0) goto LAB_0021ed8c;
    }
    if (*(char *)(param_1 + 0x5b0) != '\0') {
      local_dd8 = 1;
      local_dd0 = (ulong *)FUN_002a8024(*(undefined4 *)(param_1 + 0x5ec));
    }
    local_fe0 = 0;
    local_678[0] = CONCAT44(local_678[0]._4_4_,0x58415643);
    local_678[1] = 0xa00000;
    uVar30 = _Znwm(800);
    FUN_0031f088(uVar30,local_678);
    iVar25 = FUN_0031f20c(uVar30,&local_e18);
    if (iVar25 == 0) {
      iVar25 = FUN_003269f0(uVar30,0,0,0,&local_fe0);
      if (iVar25 != 0) {
        uVar20 = 0x188;
        goto LAB_0021e4c8;
      }
    }
    else {
      uVar20 = 0x180;
LAB_0021e4c8:
      local_470._0_4_ = CONCAT22(uVar20,0x2310);
      local_470 = (char *)(ulong)CONCAT14(2,(undefined4)local_470);
      local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x430));
      FUN_00216878(&local_470,0x10);
      FUN_001fdd5c(uVar73,iVar23);
    }
    FUN_00325ae4(uVar30);
    thunk_FUN_00325ae4(uVar30);
    _ZdlPvm(uVar30,800);
    if (bVar3 != 0) {
      local_fe0 = local_fe0 + 0x18 + (uVar33 - 0x18);
    }
    uVar30 = *(undefined8 *)(param_1 + 0x880);
    uVar71 = *(ulong *)(param_1 + 0x878);
    uVar34 = (ulong)*(uint *)(param_1 + 0x910) * (ulong)*(uint *)(param_1 + 0x914);
    uVar45 = 0;
    if (uVar34 != 0) {
      uVar45 = (local_fe0 + -1 + uVar34) / uVar34;
    }
    local_fe0 = uVar45 * uVar34;
    memset(&local_470,0,0x88);
    local_470 = *(char **)(param_1 + 0x108);
    local_468 = (long *)local_e18;
    local_460 = CONCAT44(local_e0c,local_e10);
    local_458 = CONCAT44(local_e04,local_e08);
    local_450._0_4_ = local_e00;
    local_450._4_4_ = local_dfc;
    uStack_448._0_4_ = local_df8;
    uStack_448._4_4_ = local_df4;
    local_440 = local_df0;
    uStack_43c = local_dec;
    local_438 = CONCAT44(local_438._4_4_,local_de8);
    local_430 = local_de0;
    local_428 = CONCAT71(local_428._1_7_,local_dd8);
    local_420 = local_dd0;
    local_418 = uVar32;
    local_410 = uVar71;
    local_408 = uVar30;
    local_400 = uVar34;
    local_3f8 = uVar73;
    FUN_0021776c(param_1,*(undefined4 *)(param_1 + 0x430),&local_470);
    memset(local_7c8,0,0x150);
    local_7c8[0] = *(undefined4 *)(param_1 + 0x434);
    local_790 = *(undefined4 *)(param_1 + 0x444);
    local_7c0 = uVar32;
    local_680 = uVar73;
    memcpy(auStack_78c,(void *)(param_1 + 0x9d0),0x106);
    FUN_002177dc(param_1,*(undefined4 *)(param_1 + 0x430),local_7c8);
    uVar30 = _Znwm(0x38);
    FUN_001fb384();
    *(undefined8 *)(param_1 + 0x9a8) = uVar30;
    uVar30 = _Znwm(0x30);
    thunk_FUN_001fb498(uVar30,*(undefined8 *)(param_1 + 0x9a8));
    *(undefined8 *)(param_1 + 0x9a0) = uVar30;
    lVar44 = param_1 + 0xcb8;
    FUN_001fb77c(uVar30,local_fe0);
    uVar45 = *(ulong *)(param_1 + 0x840);
    puVar1 = &DAT_003d25ef;
    if (*(undefined1 **)(param_1 + 0x5a8) != (undefined1 *)0x0) {
      puVar1 = *(undefined1 **)(param_1 + 0x5a8);
    }
    FUN_00216738(auStack_f30,puVar1);
    if (local_f28 == 0) {
      _ZNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEEC1ESt13_Ios_Openmode
                (&local_470,0x18);
      _ZNSolsEj(&local_460,0xffffffff);
      _ZNKSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEE3strEv(local_678,&local_458);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEOS4_(lVar44,local_678);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(local_678);
      _ZNSt7__cxx1118basic_stringstreamIcSt11char_traitsIcESaIcEED1Ev(&local_470);
    }
    else {
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6assignEPKc
                (lVar44,*(undefined8 *)(param_1 + 0x5a8));
    }
    FUN_00216cc0(auStack_dc8);
    FUN_00216cfc(auStack_cc8);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_d28,param_1 + 0xc78);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_d08,param_1 + 0xc98);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_ce8,lVar44);
    FUN_00216ea4(param_1,pvVar40,auStack_dc8,auStack_cc8);
    memset(&local_900,0,0x138);
    memcpy(auStack_8e0,auStack_dc8,0x50);
    memcpy(auStack_890,auStack_cc8,100);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_828,auStack_d28);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_808,auStack_d08);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(auStack_7e8,auStack_ce8);
    local_900 = *(undefined8 *)(param_1 + 0x584);
    uStack_8f8 = *(undefined8 *)(param_1 + 0x58c);
    local_8f0 = *(undefined8 *)(param_1 + 0x594);
    uStack_8e8 = *(undefined8 *)(param_1 + 0x59c);
    uVar30 = _Znwm(0x30);
    FUN_0034f450();
    *(undefined8 *)(param_1 + 0x980) = uVar30;
    iVar23 = FUN_0034f4a0(uVar30,&local_900);
    if (iVar23 != 0) {
      local_470 = (char *)0x201902310;
      local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x430));
      FUN_00216878(&local_470,0x10);
    }
    uVar30 = _Znwm(0x5cf0);
    FUN_00207a94();
    in_stack_ffffffffffffe640 = (undefined8 *)CONCAT44(uVar79,*(undefined4 *)(param_1 + 0x974));
    in_stack_ffffffffffffe638 = (int *)CONCAT44(uVar78,*(undefined4 *)(param_1 + 0x970));
    in_stack_ffffffffffffe630 = (ulong *)CONCAT44(uVar77,*(undefined4 *)(param_1 + 0x96c));
    in_stack_ffffffffffffe628 = (uint *)CONCAT44(uVar27,*(undefined4 *)(param_1 + 0x968));
    in_stack_ffffffffffffe620 = (uint *)CONCAT44(uVar76,*(undefined4 *)(param_1 + 0x964));
    iVar23 = _ZN14xavcsMovieMeta4InitEP14NrtMetaWrapperjbN3mpr10StreamTypeEN9DefStruct9FrameRateEjNS2_16MovieCaptureTypeENS2_16LmbMlogRecStatusENS2_19LmbGeneralRecStatusENS4_10DeviceTypeENS4_14FileSystemTypeEjRj
                       (uVar30,*(undefined8 *)(param_1 + 0x980),*(undefined4 *)(param_1 + 0x434),
                        *(undefined1 *)(param_1 + 0x438),*(undefined4 *)(param_1 + 0x444),
                        *(undefined4 *)(param_1 + 0x55c),*(undefined2 *)(param_1 + 0x578),
                        *(undefined4 *)(param_1 + 0x574),in_stack_ffffffffffffe620,
                        in_stack_ffffffffffffe628,in_stack_ffffffffffffe630,
                        in_stack_ffffffffffffe638,in_stack_ffffffffffffe640,&local_1030);
    if (iVar23 != 0) {
      local_470 = (char *)0x201982310;
      local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x430));
      FUN_00216878(&local_470,0x10);
      FUN_001fdd5c(uVar73,iVar23);
    }
    __ptr = malloc((ulong)local_1030);
    __s = (void *)_Znam(0x1000);
    memset(__s,0,0x1000);
    FUN_001ca7d4(local_678 + 1);
    local_678[0] = uVar34;
    memcpy(local_678 + 1,pvVar40,0x1fe);
    FUN_00217d4c(param_1,local_678);
    _ZN3mpr6thread10normalLock4lockEv();
    if (*(char *)(param_1 + 0x2c20) == '\0') {
      *(undefined1 *)(param_1 + 0x2c20) = *(undefined1 *)(param_1 + 0x978);
      *(undefined4 *)(param_1 + 0x2c24) = *(undefined4 *)(param_1 + 0x97c);
    }
    _ZN3mpr6thread10normalLock6unlockEv(param_1 + 0x2c28);
    local_1040 = 0;
    local_f48 = 0;
    uStack_f40 = 0;
    local_103f[0] = 0xff;
    local_102c = 0;
    local_1028 = 0;
    local_1024 = 0;
    local_1020 = 0;
    local_101c = 0;
    local_fd8 = 0;
    local_f38 = 0;
    _ZN3mpr5pconv31ConvertFrameRateToNumerAndDenomEN9DefStruct9FrameRateEPjS3_
              (*(undefined4 *)(param_1 + 0x55c),&local_1020,&local_101c);
    uVar71 = (ulong)local_1020;
    uVar10 = (ulong)local_101c;
    local_fd8 = CONCAT44(local_101c,local_1020);
    memset(&local_b80,0,0xa8);
    memset(&local_bf8,0,0x78);
    local_fd0 = 0;
    local_af4 = 4;
    if (*(int *)(param_1 + 0x56c) != 2) {
      local_af4 = 1;
    }
    local_470 = (char *)0x1f4000003e8;
    FUN_00219b8c(pvVar40,&local_470,&local_ff0,&local_fd0);
    uStack_ed0 = *(undefined4 *)(param_1 + 0x444);
    local_ed4 = *(uint *)(param_1 + 0x430);
    uStack_ee0 = 0;
    local_ee8 = (undefined8 *)CONCAT44(uStack_ed0,local_ed4);
    local_ec8 = (uint)((ulong)(*(long *)(param_1 + 0x850) * 0x32) / 100);
    local_ecc = 1;
    local_eb8 = 2;
    local_ec4 = (undefined4)((ulong)(*(long *)(param_1 + 0x850) * 0x19) / 100);
    local_edc = (undefined4)local_fd0;
    uStack_ed8 = (undefined4)((ulong)local_fd0 >> 0x20);
    local_468 = (long *)CONCAT44(uStack_ed0,local_ed4);
    local_460 = local_fd0;
    local_eb0 = 0;
    local_470 = (char *)0x105482310;
    local_ec0 = local_ed4;
    uStack_ebc = uStack_ed0;
    FUN_00216878(&local_470,0x18);
    local_470 = (char *)0x105502310;
    local_468 = local_ee8;
    local_460 = CONCAT44(local_ec4,local_ec8);
    FUN_00216878(&local_470,0x18);
    lVar44 = (ulong)*(uint *)(param_1 + 0x430) * 0x18 + (long)*(int *)(param_1 + 0x444) * 0xc;
    *(undefined4 *)(&DAT_00492780 + lVar44) = 0;
    iVar23 = *(int *)(param_1 + 0x574);
    *(undefined4 *)(&DAT_00492784 + lVar44) = 0;
    *(undefined4 *)(&DAT_00492788 + lVar44) = 0;
    if (iVar23 == 0) {
LAB_0021ebf0:
      uVar35 = 10000;
    }
    else {
      uVar21 = _ZN3mpr5pconv23ConvertFrameRateToNumerEN9DefStruct9FrameRateE
                         (*(undefined4 *)(param_1 + 0x55c));
      uVar72 = (ulong)uVar21 * (local_ff0 >> 0x20);
      uVar38 = local_ff0 & 0xffffffff;
      if (uVar72 < uVar38 || uVar72 - uVar38 == 0) goto LAB_0021ebf0;
      uVar35 = 0;
      if (uVar38 != 0) {
        uVar35 = uVar72 / uVar38;
      }
      if (uVar72 - uVar35 * uVar38 != 0) {
        uVar35 = uVar35 + 1;
      }
      uVar35 = uVar35 * 5000;
    }
    iVar23 = 0;
    pvVar36 = memcpy(auStack_1240,pvVar40,0x200);
    uVar26 = FUN_00219ea8(param_1,pvVar36,*(undefined4 *)(param_1 + 0x90c),&local_ff0);
    local_103f[1] = '\0';
    local_103f[2] = '\0';
    local_1018 = 0;
    local_1014 = 0;
    local_1010 = 0;
    local_100c = 0;
    local_1008 = 0;
    FUN_0021a08c(param_1,pvVar40,&local_ff0,&local_1018,&local_1014,&local_1010,&local_100c,
                 &local_1008);
    local_18c8 = 0;
    local_18a8 = 0;
    local_1890 = 0;
    local_1884 = 0;
    uVar74 = uVar22 & 0xff;
    uVar54 = uVar22 & 0xff;
    local_1868 = 0;
    local_1864 = 0;
    local_1860 = 0;
    local_185c = 0;
    local_1850 = 0;
    local_1840 = 0;
    uVar68 = 0;
    while (uVar29 = uVar22 & 0xff, local_1040 == 0) {
      uVar72 = 0;
      local_18c0 = 0;
      bVar11 = false;
      local_1844 = 0;
      local_f98 = 0;
      local_f58 = 0;
      local_ea8 = 0;
      uStack_ea7 = 0;
      local_e60 = 0;
      local_d78[0] = 0;
      local_c60[0] = 0;
      while ((CONCAT71(uStack_ea7,local_ea8) < uVar35 && (local_1040 == 0))) {
        lVar37 = *(long *)(param_1 + 0x990);
        lVar44 = *(long *)(param_1 + 0x988);
        cVar19 = FUN_0021af14(param_1,pvVar41,local_1018,local_1010,local_100c,local_1008,uVar73,
                              local_103f + 2);
        local_1848 = 0;
        if (cVar19 == '\0') {
          iVar25 = _ZN3mpr5Codec20GetVideoInfoReadListEjjPSt6vectorINS_16VideoEncodeTable9VideoInfoESaIS3_EE
                             (*(undefined4 *)(param_1 + 0x444),*(undefined4 *)(param_1 + 0x434),
                              &local_b50);
          if (iVar25 == 0) {
            local_f10[0] = local_f10[0] & 0xffffff00;
            local_ad8 = 0;
            local_9f8 = 0;
            _ZN3mpr5pconv31ConvertFrameRateToNumerAndDenomEN9DefStruct9FrameRateEPjS3_
                      (*(undefined4 *)(param_1 + 0x55c),&local_ad8,&local_9f8);
            uVar48 = 1;
            if ((*(uint *)(param_1 + 0x560) < 0xd) &&
               ((1L << ((ulong)*(uint *)(param_1 + 0x560) & 0x3f) & 0x1600U) != 0)) {
              uVar48 = 0x1e;
              if (*(uint *)(param_1 + 0x55c) < 0xc) {
                uVar48 = (&DAT_003d0bf8)[*(uint *)(param_1 + 0x55c)];
              }
            }
            in_stack_ffffffffffffe630 = (ulong *)&local_f58;
            in_stack_ffffffffffffe628 = (uint *)local_103f;
            in_stack_ffffffffffffe620 = local_f10;
            iVar25 = _ZN16XavcsMovieWriter28WaitForVideoInfoAccumulationERKSt6vectorIN3mpr16VideoEncodeTable9VideoInfoESaIS3_EEjjjPmPjPbSA_PhS9_
                               (param_1,&local_b50,uVar48,local_ad8,local_9f8,&local_e60,local_b38,
                                &local_1040,in_stack_ffffffffffffe620,in_stack_ffffffffffffe628,
                                in_stack_ffffffffffffe630);
            if (iVar25 == 0) {
              if (local_b38[0] != 0) {
                local_1844 = uVar74;
              }
              local_18c0 = (int)(local_b48 - local_b50 >> 5) * -0x55555555;
              FUN_00218c84(&local_ee8);
              if ((char)local_f10[0] == '\0') goto LAB_0021ed44;
              goto LAB_0021f304;
            }
            local_470 = (char *)0x201a02310;
            local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
            FUN_00216878(&local_470,0x10);
LAB_0021ef88:
            FUN_001fdd5c(uVar73,iVar25);
            goto LAB_0021ef90;
          }
LAB_0021f304:
          if (local_1040 != 0) {
            FUN_00218c84(&local_ec0,1);
          }
          lVar44 = lVar44 + local_e60;
          uVar72 = *(ulong *)(local_b50 + 0x18);
          uVar38 = (local_b48 - local_b50 >> 5) * -0x5555555555555555;
          for (uVar50 = 0; uVar50 <= uVar38 && uVar38 - uVar50 != 0;
              uVar50 = (ulong)((int)uVar50 + 1)) {
            bVar4 = *(byte *)(local_b50 + uVar50 * 0x60);
            if (bVar4 == 3 || bVar4 < 2) {
              uVar47 = *(ulong *)(local_b50 + uVar50 * 0x60 + 0x18);
              if (uVar47 < uVar72) {
                uVar72 = uVar47;
              }
            }
            else if (bVar4 == 9 || bVar4 == 2) break;
          }
          if (local_1890 == 0) {
            local_460 = uVar72 & 0xffffffff;
            local_470 = (char *)0x101a42310;
            local_468 = (long *)CONCAT44((int)(uVar72 >> 0x20),*(undefined4 *)(param_1 + 0x444));
            FUN_00216878(&local_470,0x18);
            local_18c8 = uVar72;
          }
          iVar25 = _ZN3mpr5Codec20GetAudioInfoReadListEjjPSt6vectorINS_16AudioEncodeTable9AudioInfoESaIS3_EEb
                             (*(undefined4 *)(param_1 + 0x444),*(undefined4 *)(param_1 + 0x434),
                              local_b20,*(int *)(param_1 + 0x568) - 3U < 2);
          if (iVar25 == 0) {
            iVar2 = *(int *)(param_1 + 0x568);
            local_9f8 = local_9f8 & 0xffffff00;
            iVar25 = 0x5a0;
            if (iVar2 - 3U < 2) {
              iVar25 = 0x400;
            }
            if ((iVar2 == 2) || (uVar65 = 48000, iVar2 == 4)) {
              uVar65 = 96000;
            }
            in_stack_ffffffffffffe638 = local_b08;
            in_stack_ffffffffffffe630 = local_d78;
            in_stack_ffffffffffffe628 = (uint *)(param_1 + 0x8a8);
            uVar28 = 0;
            if (uVar65 != 0) {
              uVar28 = (uint)(iVar25 * 90000) / uVar65;
            }
            in_stack_ffffffffffffe620 =
                 (uint *)CONCAT44((int)((ulong)in_stack_ffffffffffffe620 >> 0x20),uVar28);
            in_stack_ffffffffffffe640 = (undefined8 *)&local_9f8;
            iVar25 = _ZN16XavcsMovieWriter28WaitForAudioInfoAccumulationEN3mpr14AudioParamCombEjRSt6vectorINS0_16AudioEncodeTable9AudioInfoESaIS4_EEmmmbjRKNS0_5Codec14BufferAreaInfoEPmPjPbRm
                               (param_1,iVar2,local_af4,local_b20,uVar72,lVar44,lVar37,local_1040,
                                in_stack_ffffffffffffe620,in_stack_ffffffffffffe628,
                                in_stack_ffffffffffffe630,in_stack_ffffffffffffe638,&local_9f8,
                                local_c60);
            if (iVar25 == 0) {
              if ((char)local_9f8 != '\0') goto LAB_0021f52c;
              goto LAB_0021f524;
            }
            uVar20 = 0x1a8;
LAB_0021f4f4:
            local_470._0_4_ = CONCAT22(uVar20,0x2310);
            local_470 = (char *)(ulong)CONCAT14(2,(undefined4)local_470);
            local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
            puVar42 = &local_470;
LAB_0021f510:
            FUN_00216878(puVar42,0x10);
            goto LAB_0021ef88;
          }
LAB_0021f52c:
          uVar38 = local_d78[0];
          iVar25 = _ZN3mpr5Codec19GetMetaInfoReadListEjjPSt6vectorIN4McIf8MetaInfoESaIS3_EE
                             (*(undefined4 *)(param_1 + 0x444),*(undefined4 *)(param_1 + 0x434),
                              &local_b78);
          if (iVar25 == 0) {
            local_9f8 = local_9f8 & 0xffffff00;
            iVar25 = _ZN16XavcsMovieWriter27WaitForMetaInfoAccumulationERKSt6vectorIN4McIf8MetaInfoESaIS2_EERKS0_IN3mpr16VideoEncodeTable9VideoInfoESaIS9_EEjbPjSE_Pb
                               (param_1,&local_b78,&local_b50,local_b38[0],local_1040,&local_b60,
                                &local_f98,&local_9f8);
            if (iVar25 != 0) {
              uVar20 = 0x1b0;
              goto LAB_0021f4f4;
            }
            if ((char)local_9f8 == '\0') goto LAB_0021f524;
          }
          if (local_b60 == 0) {
LAB_0021f644:
            pvVar40 = memcpy(&local_470,&DAT_003d0b30,0xc0);
            uVar50 = 0;
            do {
              if (*(int *)(uVar50 * 0x10 + (long)pvVar40) == *(int *)(param_1 + 0x55c)) {
                uVar50 = (ulong)(double)(&local_468)[(uVar50 & 0xffffffff) * 2];
                goto LAB_0021f694;
              }
              uVar50 = uVar50 + 1;
            } while (uVar50 != 0xc);
            uVar50 = 0x16e93c;
LAB_0021f694:
            local_18a4 = 0;
            for (local_1870 = 0; (int)local_1870 < (int)(local_b48 - local_b50 >> 5) * -0x55555555;
                local_1870 = local_1870 + 1) {
              lVar70 = local_1870 * 0x60;
              lVar69 = local_b50 + lVar70;
              cVar19 = FUN_0021666c(*(undefined1 *)(local_b50 + lVar70));
              if (cVar19 != '\0') {
                if (*(char *)(lVar69 + 0x59) != '\0') {
                  uVar65 = _ZN16XavcsMovieWriter30ConvertMprMarkType2NrtMarkTypeEN3mpr8MarkTypeE
                                     (param_1);
                  lVar69 = local_b50 + lVar70;
                  if (uVar65 != 0xe) {
                    lVar46 = *(long *)(lVar69 + 0x18);
                    uVar28 = 0;
                    if (uVar50 != 0) {
                      uVar28 = (uint)((((1 - local_18c8) + lVar46) * 1000) / uVar50);
                    }
                    if (*(uint *)(param_1 + 0xc04) < uVar28) {
                      uVar39 = *(undefined8 *)(lVar69 + 0x20);
                      uVar27 = _ZN16XavcsMovieWriter28ConvertMprMarkType2MprEmTypeEN3mpr8MarkTypeE
                                         (param_1,*(undefined1 *)(lVar69 + 0x59));
                      cVar19 = _ZN16XavcsMovieWriter15IsEditAvailableENS_8EditTypeEN3mpr15EssenceMarkTypeE
                                         (param_1,0,uVar27);
                      if (cVar19 != '\0') {
                        local_9d8 = local_9d8 & 0xffffff00;
                        local_9f0 = local_9f0 & 0xffffffff00000000;
                        local_9e0 = 0;
                        local_9f8 = uVar65;
                        local_9f4 = uVar28;
                        local_9e8 = &local_9d8;
                        FUN_003522fc(*(undefined8 *)(param_1 + 0x980),&local_9f8);
                        puVar43 = *(uint **)(param_1 + 0xc10);
                        if (puVar43 == *(uint **)(param_1 + 0xc18)) {
                          FUN_0021b818(param_1 + 0xc08,&local_9f8);
                        }
                        else {
                          if (puVar43 != (uint *)0x0) {
                            *puVar43 = local_9f8;
                            puVar43[1] = local_9f4;
                            puVar43[2] = (uint)local_9f0;
                            _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_
                                      (puVar43 + 4,&local_9e8);
                          }
                          *(long *)(param_1 + 0xc10) = *(long *)(param_1 + 0xc10) + 0x30;
                        }
                        lVar69 = param_1 + (long)(int)uVar65 * 4;
                        local_ad3 = 0;
                        local_ad2 = 0;
                        local_ad4 = 1;
                        local_ad1 = 0;
                        *(int *)(lVar69 + 0xbdc) = *(int *)(lVar69 + 0xbdc) + 1;
                        *(uint *)(param_1 + 0xc04) = uVar28;
                        local_ad0 = *(undefined4 *)(param_1 + 0x444);
                        local_ad8 = 0x1a12310;
                        local_acc = (int)local_1870;
                        local_ac8 = local_18a4 + (int)local_1890;
                        local_ac4 = (uint)*(byte *)(local_b50 + lVar70);
                        FUN_00216878(&local_ad8,0x18);
                        local_ad3 = 0;
                        local_ad4 = 1;
                        local_ad0 = (uint)((ulong)lVar46 >> 0x20);
                        local_18b0 = (undefined4)lVar46;
                        local_ad8 = 0x1a22310;
                        local_acc = local_18b0;
                        local_ac8 = (uint)((ulong)uVar39 >> 0x20);
                        local_1900 = (undefined4)uVar39;
                        local_ac4 = local_1900;
                        local_ad2 = 0;
                        local_ad1 = 0;
                        FUN_00216878(&local_ad8,0x18);
                        local_ad3 = 0;
                        local_ad2 = 0;
                        local_18a0 = (undefined4)uVar50;
                        local_ad1 = 0;
                        local_acc = local_18a0;
                        local_ad4 = 1;
                        local_ad8 = 0x1a32310;
                        local_ac4 = 0;
                        local_ac8 = (uint)*(byte *)(local_b50 + lVar70 + 0x59);
                        local_ad0 = uVar28;
                        FUN_00216878(&local_ad8,0x18);
                        _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv
                                  (&local_9e8);
                      }
                    }
                  }
                }
                local_18a4 = local_18a4 + 1;
              }
            }
            lVar69 = 0;
            while (((int)lVar69 < (int)(local_b70 - (long)local_b78 >> 7) * -0x45d1745d &&
                   (local_1864 != 2))) {
              if ((*(char *)((long)local_b78 + lVar69 * 0x580 + 0x13) != '\0') &&
                 (*(char *)((long)local_b78 + lVar69 * 0x580 + 0x486) != '\0')) {
                cVar19 = *(char *)((long)local_b78 + lVar69 * 0x580 + 0x492);
                if (cVar19 == 'A') {
                  FUN_00208c74(uVar30,(long)local_b78 + lVar69 * 0x580 + 0x480);
                  local_1864 = 2;
                }
                else if (cVar19 == 'V') {
                  if (local_1864 == 0) {
                    FUN_00208c74(uVar30,(long)local_b78 + lVar69 * 0x580 + 0x480);
                  }
                  local_1864 = 1;
                }
              }
              lVar69 = lVar69 + 1;
            }
            _ZN3mpr8BaseList5ClearEv(*(undefined8 *)(param_1 + 0x110));
            iVar25 = _ZN16XavcsMovieWriter15MakeVideoEsListEN3mpr10StreamTypeERKSt6vectorINS0_16VideoEncodeTable9VideoInfoESaIS4_EEjPNS0_8BaseListERjl
                               (param_1,*(undefined4 *)(param_1 + 0x444),&local_b50,local_b38[0],
                                *(undefined8 *)(param_1 + 0x110),&local_1024,
                                *(undefined8 *)(param_1 + 0x8a0));
            if (iVar25 == 0) {
              _ZN3mpr8BaseList5ClearEv(*(undefined8 *)(param_1 + 0x118));
              iVar25 = _ZN16XavcsMovieWriter15MakeAudioEsListEN3mpr10StreamTypeERKSt6vectorINS0_16AudioEncodeTable9AudioInfoESaIS4_EEjPNS0_8BaseListEl
                                 (param_1,*(undefined4 *)(param_1 + 0x444),local_b20,local_b08[0],
                                  *(undefined8 *)(param_1 + 0x118),*(undefined8 *)(param_1 + 0x8b8))
              ;
              if (iVar25 == 0) {
                uVar27 = _ZN3mpr10RingBuffer11GetUsedSizeEv(uVar45);
                FUN_00218c84(&local_ed4,uVar27);
                uVar68 = _ZNK3mpr8BaseList7GetSizeEv(*(undefined8 *)(param_1 + 0x110));
                iVar23 = _ZNK3mpr8BaseList7GetSizeEv(*(undefined8 *)(param_1 + 0x118));
                local_1860 = local_b60 * local_1030;
                uVar28 = _ZN3mpr10RingBuffer13GetRemainSizeEv(uVar45);
                uVar65 = uVar68 + iVar23 + local_1860;
                if (uVar28 < uVar65) {
LAB_0021f524:
                  local_1040 = 0;
                  goto LAB_0021ed44;
                }
                if (local_1040 != 0) {
                  uVar47 = (ulong)*(uint *)(param_1 + 0x90c);
                  uVar50 = 0;
                  if (uVar47 != 0) {
                    uVar50 = (local_1840 + uVar65) / uVar47;
                  }
                  lVar69 = (local_1840 + uVar65) - uVar50 * uVar47;
                  if (lVar69 != 0) {
                    iVar25 = *(uint *)(param_1 + 0x90c) - (int)lVar69;
                    uVar28 = _ZN3mpr10RingBuffer13GetRemainSizeEv(uVar45);
                    if (uVar28 < uVar65 + iVar25) {
                      local_9f4 = 1;
                      local_9f8 = 0x1e12310;
                      local_9f0 = CONCAT44(local_9f0._4_4_,*(undefined4 *)(param_1 + 0x430));
                      uVar27 = _ZN3mpr10RingBuffer13GetRemainSizeEv(uVar45);
                      local_9f0 = CONCAT44(uVar27,(uint)local_9f0);
                      local_9e8 = (uint *)CONCAT44(iVar25,uVar65);
                      FUN_00216878(&local_9f8,0x18);
                      goto LAB_0021f524;
                    }
                  }
                }
                *(long *)(param_1 + 0x988) = lVar44;
                *(ulong *)(param_1 + 0x990) = lVar37 + uVar38;
                local_1848 = uVar29;
                goto LAB_0021ef90;
              }
              uVar20 = 0x1c0;
            }
            else {
              uVar20 = 0x1b8;
            }
            local_9f4 = 2;
            local_9f8 = CONCAT22(uVar20,0x2310);
            local_9f0 = CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
            puVar42 = (undefined8 *)&local_9f8;
            goto LAB_0021f510;
          }
          local_470 = (char *)((ulong)local_470 & 0xffffffff00000000);
          FUN_00209dc4(uVar30,local_b78,(void *)((long)local_b78 + (ulong)(local_b60 - 1) * 0x580),
                       local_b60,&local_b78,&local_470);
          if ((1 < *(int *)(param_1 + 0x968) - 1U) ||
             (((ulong)local_470 & 0x10000000) != 0 || bVar11)) {
            bVar11 = false;
            goto LAB_0021f644;
          }
          local_1040 = 0;
          FUN_00217730(param_1,(undefined1)local_1844,&local_ea8,local_b60);
          bVar11 = true;
        }
        else {
          cVar19 = FUN_0021b2d0(param_1,pvVar41,local_103f);
          if (cVar19 != '\0') goto LAB_0021ef90;
          FUN_0021b54c(param_1,pvVar41,1);
LAB_0021ed44:
          FUN_00217730(param_1,(undefined1)local_1844,&local_ea8,100);
        }
      }
      local_1848 = 0;
LAB_0021ef90:
      local_f10[0] = 0;
      if (local_1848 == 0) {
        local_470 = (char *)0x101c82310;
        local_468 = (long *)CONCAT44(local_468._4_4_,*(undefined4 *)(param_1 + 0x430));
        uVar27 = _ZN3mpr10RingBuffer13GetRemainSizeEv(uVar45);
        local_468 = (long *)CONCAT44(uVar27,(undefined4)local_468);
        local_460 = (ulong)(uVar68 + local_1860 + iVar23);
        FUN_00216878(&local_470,0x18);
        local_470 = (char *)0x101cb2310;
        local_460 = CONCAT71(uStack_ea7,local_ea8) & 0xffffffff;
        local_468 = (long *)CONCAT44((int)((uint7)uStack_ea7 >> 0x18),
                                     *(undefined4 *)(param_1 + 0x430));
        FUN_00216878(&local_470,0x18);
        local_470 = (char *)0x101c92310;
        local_468 = (long *)CONCAT44(local_b38[0],*(undefined4 *)(param_1 + 0x430));
        FUN_00216878(&local_470,0x18);
        local_470 = (char *)0x201ca2310;
        local_468 = (long *)(ulong)CONCAT14(local_103f[2],*(undefined4 *)(param_1 + 0x430));
        FUN_00216878(&local_470,0x10);
        FUN_0021b54c(param_1,pvVar41,0);
        FUN_00218c84(&local_ee8,0);
        if (local_103f[2] == '\0') {
          FUN_001fdd5c(uVar73,2);
        }
        local_1040 = 1;
        iVar23 = 0;
        FUN_00218c84(&local_ec0,1);
        local_1860 = 0;
        uVar68 = 0;
      }
      else {
        _ZN3mpr10RingBuffer11GetFreeAreaEPNS_8BaseListE(uVar45,*(undefined8 *)(param_1 + 0x120));
        local_1028 = 0;
        if (uVar54 == 0) {
          local_b28 = 0;
          local_af8 = 0;
        }
        else {
          if ((local_b70 - (long)local_b78 >> 7) * 0x2e8ba2e8ba2e8ba3 == 0) {
            local_470 = (char *)0x301d02310;
            local_468 = (long *)CONCAT44(local_468._4_4_,*(undefined4 *)(param_1 + 0x430));
            FUN_00216878(&local_470,0xc);
          }
          else {
            local_460 = 0;
            local_470 = (char *)0x0;
            local_468 = (undefined8 *)0x0;
            local_458 = local_458 & 0xffffffff00000000;
            pvVar40 = memcpy(auStack_17c0,local_b78,0x580);
            _ZN14xavcsMovieMeta18SetFirstMetaForNRTEN4McIf8MetaInfoERjRN3mpr13RDD18MetaInfoEPN6NRTDEF21CameraUnitMetadataSetE
                      (uVar30,pvVar40,&local_102c,&local_f48,(long)&local_470 + 4);
            local_470 = (char *)CONCAT44(local_470._4_4_,local_102c);
            FUN_00217c60(param_1,&local_470);
          }
          local_9d8 = local_9d8 & 0xffffff00;
          local_9f4 = 0;
          local_9f8 = 10;
          local_9f0 = local_9f0 & 0xffffffff00000000;
          local_9e0 = 0;
          local_9e8 = &local_9d8;
          if (*(int *)(param_1 + 0x960) != 0) {
            _ZNSt7__cxx1119basic_ostringstreamIcSt11char_traitsIcESaIcEEC1ESt13_Ios_Openmode
                      (&local_470,0x10);
            *(undefined8 *)((long)&local_460 + *(long *)((long)local_470 - 0x18)) = 2;
            uVar39 = _ZStlsIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_St8_SetfillIS3_E
                               (&local_470,0x30);
            _ZNSolsEj(uVar39,*(int *)(param_1 + 0x960) + 1);
            local_9f4 = 0;
            local_9f0 = local_9f0 & 0xffffffff00000000;
            local_9f8 = 0xb;
            _ZNKSt7__cxx1115basic_stringbufIcSt11char_traitsIcESaIcEE3strEv(&local_ad8,&local_468);
            _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEOS4_(&local_9e8,&local_ad8);
            _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_ad8);
            _ZNSt7__cxx1119basic_ostringstreamIcSt11char_traitsIcESaIcEED1Ev(&local_470);
          }
          if (local_9e8 != (uint *)0x0) {
            local_470 = (char *)0x501d12310;
            strncpy((char *)&local_468,(char *)local_9e8,0x20);
            local_450._4_4_ = local_450._4_4_ & 0xffffff;
            FUN_00216878(&local_470,0x28);
          }
          FUN_003522fc(*(undefined8 *)(param_1 + 0x980),&local_9f8);
          local_af8 = (int)*(ulong *)(local_b20[0] + 8);
          if (*(ulong *)(local_b20[0] + 8) < uVar72) {
            local_b28 = (int)uVar72 - local_af8;
            local_af8 = 0;
          }
          else {
            local_af8 = local_af8 - (int)uVar72;
            local_b28 = 0;
          }
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_9e8);
        }
        if ((bVar3 & local_181c == 0) != 0) {
          local_181c = _ZN15libMp4Signature24GetChunkHashesTargetDataEPKN3mpr8BaseListEmRNS_12BufferArea_tE
                                 (*(undefined8 *)(param_1 + 0x120),local_1860,auStack_f68);
        }
        local_470 = (char *)0x0;
        local_468 = (long *)0x0;
        local_460 = 0;
        _ZN16XavcsMovieWriter21MakeAudioMetaInfoListERKSt6vectorIN3mpr16AudioEncodeTable9AudioInfoESaIS3_EEjRKS0_INS1_16VideoEncodeTable9VideoInfoESaIS9_EEjRS0_IPKN22DataflowInfra_StdAudio19PIN_AUDIO_META_INFOESaISH_EE
                  (param_1,local_b20,local_b08[0],&local_b50,local_b38[0],&local_470);
        in_stack_ffffffffffffe640 = (undefined8 *)(param_1 + 0x63a);
        in_stack_ffffffffffffe638 = (int *)(param_1 + 0x9b8);
        in_stack_ffffffffffffe630 = (ulong *)(param_1 + 0x9b0);
        in_stack_ffffffffffffe628 = local_f10;
        in_stack_ffffffffffffe620 = &local_1028;
        iVar25 = _ZN14xavcsMovieMeta13WriteMetaInfoERKSt6vectorIN4McIf8MetaInfoESaIS2_EEjPhjRKS0_IPKN22DataflowInfra_StdAudio19PIN_AUDIO_META_INFOESaISB_EEbPN3mpr8BaseListERjSJ_PS7_SJ_RKNSG_24movieManuallyEnteredMetaE
                           (uVar30,&local_b78,local_b60,__ptr,local_1030,&local_470,uVar54,
                            *(undefined8 *)(param_1 + 0x120),in_stack_ffffffffffffe620,
                            in_stack_ffffffffffffe628,in_stack_ffffffffffffe630,
                            in_stack_ffffffffffffe638,in_stack_ffffffffffffe640);
        if (iVar25 != 0) {
          local_9f4 = 2;
          local_9f8 = 0x1d82310;
          local_9f0 = CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
          FUN_00216878(&local_9f8,0x10);
          FUN_001fdd5c(uVar73,iVar25);
        }
        if (local_470 != (char *)0x0) {
          _ZdlPv();
        }
        if ((bVar3 & local_181c == 0) != 0) {
          local_181c = _ZN15libMp4Signature24GetChunkHashesTargetDataEPKN3mpr8BaseListEmRNS_12BufferArea_tE
                                 (*(undefined8 *)(param_1 + 0x120),uVar68,auStack_f88);
        }
        if (iVar25 == 0) {
          uVar66 = *(undefined8 *)(param_1 + 0x110);
          uVar67 = *(undefined8 *)(param_1 + 0x120);
          uVar39 = _ZN3mpr5Codec15GetEsAddrOffsetENS0_10BufferTypeENS_10StreamTypeE
                             (2,*(undefined4 *)(param_1 + 0x444));
          iVar25 = _ZN16XavcsMovieWriter20CreateXferLinkedListEPN3mpr8BaseListES2_llPNS0_7DmacLibE
                             (param_1,uVar66,uVar67,uVar39,*(undefined8 *)(param_1 + 0x858),
                              *(undefined8 *)(param_1 + 0x128));
          if (iVar25 != 0) {
            local_470 = (char *)0x201e02310;
            local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
            FUN_00216878(&local_470,0x10);
            FUN_001fdd5c(uVar73,iVar25);
          }
        }
        if ((bVar3 & local_181c == 0) != 0) {
          local_181c = _ZN15libMp4Signature24GetChunkHashesTargetDataEPKN3mpr8BaseListEmRNS_12BufferArea_tE
                                 (*(undefined8 *)(param_1 + 0x120),iVar23,auStack_f78);
        }
        if (iVar25 == 0) {
          if (*(int *)(param_1 + 0x56c) == 2) {
            iVar25 = FUN_002060cc(*(undefined8 *)(param_1 + 0x998));
            if (iVar25 == 0) {
              uVar54 = 2;
              if (*(uint *)(param_1 + 0x56c) < 5) {
                uVar54 = (uint)(byte)(&DAT_003d0bf0)[*(uint *)(param_1 + 0x56c)];
              }
              if ((*(int *)(param_1 + 0x568) == 0) || (uVar39 = 2, *(int *)(param_1 + 0x568) == 3))
              {
                uVar39 = 0;
              }
              in_stack_ffffffffffffe620 =
                   (uint *)CONCAT44((int)((ulong)in_stack_ffffffffffffe620 >> 0x20),uVar54);
              iVar25 = _ZN11SacAccessor16AddMultiList1toNEPN3mpr8BaseListES2_lljNS_14AudioBitLengthEjj
                                 (*(undefined8 *)(param_1 + 0x998),*(undefined8 *)(param_1 + 0x120),
                                  *(undefined8 *)(param_1 + 0x118),*(undefined8 *)(param_1 + 0x858),
                                  *(undefined8 *)(param_1 + 0x8b8),iVar23,uVar39,uVar54,
                                  in_stack_ffffffffffffe620);
              if (iVar25 != 0) {
                uVar20 = 0x1f0;
                goto LAB_002206e4;
              }
            }
            else {
              local_470 = (char *)0x201e82310;
              local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
              FUN_00216878(&local_470,0x10);
LAB_00220040:
              FUN_001fdd5c(uVar73,iVar25);
            }
          }
          else {
            uVar39 = *(undefined8 *)(param_1 + 0x118);
            uVar66 = *(undefined8 *)(param_1 + 0x120);
            uVar67 = _ZN3mpr5Codec15GetEsAddrOffsetENS0_10BufferTypeENS_10StreamTypeE
                               (3,*(undefined4 *)(param_1 + 0x444));
            iVar25 = _ZN16XavcsMovieWriter20CreateXferLinkedListEPN3mpr8BaseListES2_llPNS0_7DmacLibE
                               (param_1,uVar39,uVar66,uVar67,*(undefined8 *)(param_1 + 0x858),
                                *(undefined8 *)(param_1 + 0x128));
            if (iVar25 != 0) {
              uVar20 = 0x1f8;
LAB_002206e4:
              local_470._0_4_ = CONCAT22(uVar20,0x2310);
              local_470 = (char *)(ulong)CONCAT14(2,(undefined4)local_470);
              local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
              FUN_00216878(&local_470,0x10);
              goto LAB_00220040;
            }
          }
        }
        local_470 = *(char **)(param_1 + 0x108);
        local_468 = &local_b50;
        local_460 = (ulong)*(uint *)(param_1 + 0x444);
        local_458 = param_1 + 0x130;
        local_450 = param_1 + 0x230;
        uStack_448 = (ulong *)(param_1 + 0x330);
        local_440 = (uint)__s;
        uStack_43c = (undefined4)((ulong)__s >> 0x20);
        local_430 = *(undefined8 *)(param_1 + 0x8a0);
        local_428 = *(ulong *)(param_1 + 0x128);
        local_420 = *(ulong **)(param_1 + 0x938);
        local_418 = *(ulong *)(param_1 + 0x940);
        local_410 = *(ulong *)(param_1 + 0x948);
        local_438 = uVar73;
        iVar25 = FUN_0021bed0(param_1,*(undefined4 *)(param_1 + 0x430),&local_470);
        if (iVar25 != 0) {
          local_9f4 = 2;
          local_9f8 = 0x2002310;
          local_9f0 = CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
          FUN_00216878(&local_9f8,0x10);
          FUN_001fdd5c(uVar73,iVar25);
        }
        if (*(int *)(param_1 + 0x56c) == 2) {
          iVar25 = FUN_002060e8(*(undefined8 *)(param_1 + 0x998));
          if (iVar25 != 0) {
            local_470 = (char *)0x202082310;
            local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
            FUN_00216878(&local_470,0x10);
            FUN_001fdd5c(uVar73,iVar25);
          }
          if (uVar68 != 0) {
            iVar25 = (**(code **)(**(long **)(param_1 + 0x128) + 0x20))();
            if (iVar25 != 0) {
              uVar20 = 0x210;
LAB_00220204:
              local_470._0_4_ = CONCAT22(uVar20,0x2310);
              local_470 = (char *)(ulong)CONCAT14(2,(undefined4)local_470);
              local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
              FUN_00216878(&local_470,0x10);
              FUN_001fdd5c(uVar73,iVar25);
            }
          }
        }
        else if (uVar68 != 0 || iVar23 != 0) {
          iVar25 = (**(code **)(**(long **)(param_1 + 0x128) + 0x20))();
          if (iVar25 != 0) {
            uVar20 = 0x218;
            goto LAB_00220204;
          }
        }
        iVar25 = _ZN3mpr5Codec26SetVideoInfoReadIndexCountEjjj
                           (*(undefined4 *)(param_1 + 0x444),*(undefined4 *)(param_1 + 0x434),
                            local_b38[0]);
        if (iVar25 != 0) {
          local_470 = (char *)0x202202310;
          local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
          FUN_00216878(&local_470,0x10);
          FUN_001fdd5c(uVar73,iVar25);
        }
        FUN_00218c84(&local_ee8,local_18c0 - local_b38[0]);
        iVar25 = _ZN3mpr5Codec26SetAudioInfoReadIndexCountEjjjb
                           (*(undefined4 *)(param_1 + 0x444),*(undefined4 *)(param_1 + 0x434),
                            local_b08[0] - (uint)(local_c60[0] != 0),
                            1 < *(int *)(param_1 + 0x568) - 3U & local_1040);
        if (iVar25 != 0) {
          local_470 = (char *)0x202282310;
          local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
          FUN_00216878(&local_470,0x10);
          FUN_001fdd5c(uVar73,iVar25);
        }
        iVar25 = _ZN3mpr5Codec25SetMetaInfoReadIndexCountEjjj
                           (*(undefined4 *)(param_1 + 0x444),*(undefined4 *)(param_1 + 0x434),
                            local_f98);
        if (iVar25 != 0) {
          local_470 = (char *)0x202302310;
          local_468 = (long *)CONCAT44(iVar25,*(undefined4 *)(param_1 + 0x430));
          FUN_00216878(&local_470,0x10);
          FUN_001fdd5c(uVar73,iVar25);
        }
        _ZN3mpr10RingBuffer9UseMemoryEj(uVar45,uVar68 + iVar23 + local_f10[0]);
        uVar27 = _ZN3mpr10RingBuffer11GetUsedSizeEv(uVar45);
        FUN_00218c84(&local_ed4,uVar27);
        local_b80 = *(undefined8 *)(param_1 + 0x108);
        local_b5c = local_1030;
        local_b58 = local_1850 + local_fe0;
        local_b30 = local_f10[0] + local_b58;
        local_b00 = local_b30 + (ulong)uVar68;
        local_af0 = *(undefined4 *)(param_1 + 0x568);
        local_ae8 = uVar73;
        FUN_0021d770(param_1,*(undefined4 *)(param_1 + 0x430),&local_b80);
        local_bf8 = local_b60;
        local_bf4 = local_b5c;
        local_bf0 = local_b58;
        _ZNSt6vectorIN3mpr16VideoEncodeTable9VideoInfoESaIS2_EEaSERKS4_(auStack_be8,&local_b50);
        local_bd0 = local_b38[0];
        local_bc8 = local_b30;
        local_bc0 = local_b28;
        _ZNSt6vectorIN3mpr16AudioEncodeTable9AudioInfoESaIS2_EEaSERKS4_(auStack_bb8,local_b20);
        local_ba0 = local_b08[0];
        local_b98 = local_b00;
        local_b90 = local_af8;
        local_b8c = local_af4;
        local_b88 = local_af0;
        FUN_0021d404(param_1,&local_bf8);
        local_460 = local_460 & 0xffffffffffffff00;
        local_470 = (char *)CONCAT44(local_470._4_4_,local_bf4);
        local_468 = (long *)local_bf0;
        _ZN16XavcsMovieWriter12WriteFixInfoEN3mpr10StreamTypeEjjPN10libXavcMp429RecRealTimeMetaDataSampleInfoE
                  (param_1,*(undefined4 *)(param_1 + 0x444),local_bf8,local_bf4,&local_470);
        uVar54 = (uint)(bVar3 & local_181c == 0);
        if (uVar54 != 0) {
          if (*(int *)(param_1 + 0x444) == 0) {
            local_181c = _ZN15libMp4Signature15CalcChunkHashesERKNS_12BufferArea_tES2_S2_RKN3mpr5Codec14BufferAreaInfoEilbRNS_13ChunkHashes_tE
                                   (auStack_f68,auStack_f88,auStack_f78,param_1 + 0x848,local_1038,
                                    local_fe8,*(int *)(param_1 + 0x56c) == 2,
                                    *plVar31 + (ulong)*(uint *)(plVar31 + 3) * 0xc0);
          }
          else {
            local_181c = _ZN15libMp4Signature20CalcChunkHashesProxyERKNS_12BufferArea_tES2_S2_RKN3mpr5Codec14BufferAreaInfoEilRNS_18ChunkHashesProxy_tE
                                   (auStack_f68,auStack_f88,auStack_f78,param_1 + 0x848,local_1038,
                                    local_fe8,*plVar31 + (ulong)*(uint *)(plVar31 + 3) * 0x60);
          }
          uVar54 = 0;
          *(int *)((long)plVar31 + 0x1c) = *(int *)((long)plVar31 + 0x1c) + 1;
          uVar65 = (int)plVar31[3] + 1;
          uVar28 = uVar65 - *(uint *)(plVar31 + 2);
          if (uVar65 < *(uint *)(plVar31 + 2)) {
            uVar28 = uVar65;
          }
          *(uint *)(plVar31 + 3) = uVar28;
        }
      }
      local_1840 = local_1840 + (uVar68 + iVar23 + local_f10[0]);
      local_468 = (long *)CONCAT44(local_468._4_4_,*(undefined4 *)(param_1 + 0x90c));
      local_458 = CONCAT44(*(undefined4 *)(param_1 + 0x434),(undefined4)local_458);
      local_450._4_4_ = local_ed4;
      uStack_448._0_4_ = uStack_ed0;
      uStack_448._4_4_ = local_ecc;
      local_440 = local_ec8;
      uStack_43c = local_ec4;
      local_458 = local_458 & 0xffffffffffffff00;
      local_450._0_4_ = (uint)local_450 & 0xffffff00;
      local_470 = (char *)uVar32;
      local_460 = uVar45;
      while( true ) {
        uVar65 = *(uint *)(param_1 + 0x90c);
        if (local_1840 < uVar65) break;
        if ((local_1840 == uVar65 & local_1040) != 0) {
          local_458 = CONCAT71(local_458._1_7_,1);
        }
        if (local_1884 == 0) {
          _ZN3mpr10VpgManager10StartWriteEjNS_10StreamTypeEPb
                    (*(undefined4 *)(param_1 + 0x430),*(undefined4 *)(param_1 + 0x444),
                     local_103f + 1);
        }
        if ((local_103f[1] == '\0') && (local_1868 = local_1868 + 1, uVar26 <= local_1868)) {
          local_1868 = 0;
          local_450._0_4_ = CONCAT31(local_450._1_3_,1);
        }
        local_438 = uVar73;
        FUN_0021784c(param_1,*(undefined4 *)(param_1 + 0x430),&local_470);
        local_1840 = local_1840 - *(uint *)(param_1 + 0x90c);
        local_1884 = uVar29;
      }
      uVar28 = (uint)local_1040;
      if ((uVar28 != 0) && (local_1840 != 0)) {
        _ZN16XavcsMovieWriter16FillStreamBufferEPN3mpr10RingBufferEj
                  (param_1,uVar45,uVar65 - (int)local_1840);
        uVar27 = _ZN3mpr10RingBuffer11GetUsedSizeEv(uVar45);
        FUN_00218c84(&local_ed4,uVar27);
        local_458 = CONCAT71(local_458._1_7_,1);
        if (local_1884 == 0) {
          _ZN3mpr10VpgManager10StartWriteEjNS_10StreamTypeEPb
                    (*(undefined4 *)(param_1 + 0x430),*(undefined4 *)(param_1 + 0x444),
                     local_103f + 1);
        }
        if (local_103f[1] == '\0') {
          local_1868 = 0;
          local_450._0_4_ = CONCAT31(local_450._1_3_,1);
        }
        local_438 = uVar73;
        FUN_0021784c(param_1,*(undefined4 *)(param_1 + 0x430),&local_470);
        local_1840 = 0;
        local_1884 = uVar28;
      }
      if (local_1848 == 0) break;
      local_1850 = local_1850 + (uVar68 + iVar23 + local_f10[0]);
      local_1890 = local_1890 + (ulong)local_f58;
      uVar29 = _ZN3mpr5Codec29GetEncStatusVideoFrameCounterEv();
      if ((((local_18a8 == 0) && (*(int *)(param_1 + 0x444) == 0)) &&
          (0.0 < (float)uVar71 / (float)uVar10)) &&
         ((float)((ulong)*(ushort *)(param_1 + 0x24) * 0x3c - 0x3c) <=
          (float)uVar29 / ((float)uVar71 / (float)uVar10))) {
        local_9f4 = 3;
        local_9f8 = 0x2382310;
        local_9f0 = CONCAT44(local_9f0._4_4_,*(undefined4 *)(param_1 + 0x430));
        FUN_00216878(&local_9f8,0xc);
        (**(code **)(param_1 + 0x38))
                  (*(undefined8 *)(param_1 + 0x40),0x40000005,0,0,0,*(undefined8 *)(param_1 + 0x48))
        ;
        local_18a8 = local_1848;
      }
      if (local_1040 == 0) {
        if (local_185c == 0) {
          uVar29 = *(uint *)(param_1 + 0x908);
          if ((uVar29 != 0xffffffff) && (*(int *)(param_1 + 0x444) == 0)) {
            if (((ulong)uVar29 <= local_1850 / 1000000) ||
               ((ulong)uVar29 - local_1850 / 1000000 < 0x14)) {
              local_185c = FUN_0021aa38(param_1,pvVar41,local_1014,uVar73);
              local_185c = local_185c & 0xff;
              if (local_185c == 0) {
                if (*(long *)(param_1 + 0x38) != 0) {
                  local_ad8 = *(undefined4 *)(param_1 + 0x430);
                  local_9f4 = 3;
                  local_9f0 = CONCAT44(local_9f0._4_4_,local_ad8);
                  local_9f8 = 0x2402310;
                  FUN_00216878(&local_9f8,0xc);
                  (**(code **)(param_1 + 0x38))
                            (*(undefined8 *)(param_1 + 0x40),0x40000003,0,&local_ad8,0,
                             *(undefined8 *)(param_1 + 0x48));
                }
                goto LAB_0022098c;
              }
              local_103f[2] = '\x01';
            }
          }
        }
        else {
LAB_0022098c:
          local_185c = local_1848;
        }
      }
      if ((*(int *)(param_1 + 0x444) == 0) && (local_18bc != 0)) {
        local_18bc = FUN_0022b578(*(undefined4 *)(param_1 + 0x430));
        local_18bc = local_18bc & 0xff;
      }
    }
    if ((*(char *)(param_1 + 0xcd8) == '\0') &&
       (cVar19 = FUN_00218c04(param_1,0x10000007), cVar19 != '\0')) {
      FUN_00218b84(param_1,&local_103c,&local_ff8);
      iVar23 = FUN_00207cfc(uVar30,*local_ff8,*(undefined4 *)(local_ff8 + 1),param_1 + 0xcd9,8000,
                            param_1 + 0x2c1c);
      if (iVar23 == 0) {
        if (*(int *)(param_1 + 0x2c1c) != 0) {
          *(undefined1 *)(param_1 + 0xcd8) = 1;
        }
      }
      else {
        local_470 = (char *)0x212502310;
        local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x434));
        FUN_00216878(&local_470,0x10);
      }
      free(local_ff8);
    }
    if (bVar3 != 0) {
      _ZN15libMp4Signature20ReMunmapStreamBufferERKN3mpr5Codec14BufferAreaInfoEiil
                (param_1 + 0x848,local_1038,local_1034,local_fe8);
    }
    uVar48 = *(undefined1 *)(param_1 + 0x553);
    puVar42 = (undefined8 *)(param_1 + 0x54e);
    uVar20 = *(undefined2 *)puVar42;
    uVar5 = *(undefined1 *)(param_1 + 0x554);
    uVar9 = *(undefined2 *)(param_1 + 0x550);
    uVar27 = *(undefined4 *)puVar42;
    uVar6 = *(undefined1 *)(param_1 + 0x555);
    uVar7 = *(undefined1 *)(param_1 + 0x552);
    uVar39 = *puVar42;
    uVar8 = *(undefined1 *)(param_1 + 0x556);
    _ZN3mpr4time6GetLTMEPN9DefStruct9LocalTimeE(&local_fa8);
    uVar17 = uStack_fa1;
    uVar16 = uStack_fa2;
    uVar15 = uStack_fa3;
    uVar14 = uStack_fa4;
    uVar13 = uStack_fa6;
    uVar12 = local_fa8;
    local_e70[0] = 0;
    local_ea8 = 0;
    local_e78 = 0;
    uVar18 = (undefined1)local_fa0;
    local_e80 = local_e70;
    uStack_ea7 = 0;
    uStack_ea0 = 0;
    uStack_e9f = 0;
    local_e97 = 0;
    uStack_e8f = 0;
    FUN_00216cc0(local_d78);
    FUN_00216cfc(local_c60);
    _ZN3mpr6thread10normalLockC2ENS1_9MutexTypeE(auStack_e58,0);
    local_e60 = (local_e60 >> 8 & 0xffffff) << 8;
    _ZN3mpr6thread10normalLock4lockEv(param_1 + 0x2c28);
    local_e50 = *(undefined8 *)(param_1 + 0x2c30);
    uStack_e48 = *(undefined8 *)(param_1 + 0x2c38);
    local_e60 = CONCAT71(local_e60._1_7_,*(undefined1 *)(param_1 + 0x2c20));
    local_e60 = CONCAT44(*(undefined4 *)(param_1 + 0x2c24),(undefined4)local_e60);
    local_e40 = *(undefined8 *)(param_1 + 0x2c40);
    uStack_e38 = *(undefined8 *)(param_1 + 0x2c48);
    local_e30 = *(undefined8 *)(param_1 + 0x2c50);
    uStack_e28 = *(undefined8 *)(param_1 + 0x2c58);
    local_e20 = *(undefined8 *)(param_1 + 0x2c60);
    *(undefined1 *)(param_1 + 0x2c20) = 0;
    *(undefined4 *)(param_1 + 0x2c24) = 0;
    _ZN3mpr6thread10normalLock6unlockEv(param_1 + 0x2c28);
    if (*(char *)(param_1 + 0x438) == '\0') {
      local_d78[0] = local_d78[0] & 0xffffffffffffff00;
    }
    else if (*(int *)(param_1 + 0x444) == 0) {
      FUN_00216ea4(param_1,param_1 + 0x640,local_d78,local_c60);
    }
    else {
      local_ea8 = 1;
      FUN_00219e34(param_1,param_1 + 0x648,&local_e80);
      uStack_e9f = *(undefined8 *)(param_1 + 0x78c);
      local_e97 = *(undefined8 *)(param_1 + 0x794);
      uStack_e8f = *(undefined8 *)(param_1 + 0x79c);
      uStack_ea7 = (undefined7)*(undefined8 *)(param_1 + 0x784);
      uStack_ea0 = (undefined1)((ulong)*(undefined8 *)(param_1 + 0x784) >> 0x38);
    }
    local_f08 = local_ef8;
    local_ef8[0] = 0;
    uVar54 = local_f10[0] >> 8;
    local_f10[0] = local_f10[0] & 0xffffff00;
    local_f00 = 0;
    if ((*(char *)(param_1 + 0x5a5) != '\0') && (*(char *)(param_1 + 0xc30) != '\0')) {
      local_f10[0] = CONCAT31((int3)uVar54,1);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEPKc(&local_f08,param_1 + 0xc30);
    }
    local_ad8 = 0;
    local_ad4 = 0;
    local_ad3 = 0;
    local_ad2 = 0;
    local_ad1 = 0;
    local_ad0 = (local_ad0 >> 8 & 0xff) << 8;
    local_acc = 0;
    local_ac8._3_1_ = (undefined1)(local_ac8 >> 0x18);
    local_ac8._0_3_ = 0;
    FUN_00216cc0(auStack_ac0);
    local_a48 = local_a38;
    local_a38[0] = 0;
    local_a40 = 0;
    local_a6f = 0;
    uStack_a67 = 0;
    local_a5f = 0;
    uStack_a57 = 0;
    local_a18 = local_a08;
    local_a08[0] = 0;
    local_a28 = 3;
    local_a20 = 0;
    local_ad0 = CONCAT31(local_ad0._1_3_,uVar8);
    local_a24 = 0;
    local_ad0 = CONCAT22(uVar12,(undefined2)local_ad0);
    local_a10 = 0;
    local_acc = CONCAT13(uVar15,CONCAT12(uVar14,uVar13));
    local_ac8._0_3_ = CONCAT12(uVar18,CONCAT11(uVar17,uVar16));
    local_ac4 = local_1024;
    local_ad8 = uVar27;
    local_ad4 = uVar7;
    local_ad3 = uVar48;
    local_ad2 = uVar5;
    local_ad1 = uVar6;
    memcpy(auStack_ac0,local_d78,0x4d);
    local_a70 = local_ea8;
    lVar44 = 0;
    do {
      lVar37 = lVar44 + 1;
      *(undefined1 *)((long)&local_a6f + lVar44) = *(undefined1 *)((long)&uStack_ea7 + lVar44);
      lVar44 = lVar37;
    } while (lVar37 != 0x20);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_assignERKS4_(&local_a48,&local_e80);
    local_a28 = 3;
    if ((char)local_e60 != '\0') {
      FUN_0021a2f0(local_e60._4_4_,&local_470);
      local_a28 = (undefined4)local_470;
    }
    local_a24 = 0;
    local_a20 = (char)local_f10[0];
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_assignERKS4_(&local_a18,&local_f08);
    iVar23 = FUN_0034f8e4(*(undefined8 *)(param_1 + 0x980),&local_ad8);
    if (iVar23 != 0) {
      local_470 = (char *)0x202482310;
      local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x434));
      FUN_00216878(&local_470,0x10);
    }
    local_1004 = 0;
    _ZN3mpr5pconv14SetUInt32be2leERjj(&local_1004,local_1028);
    iVar23 = FUN_0034fbc4(*(undefined8 *)(param_1 + 0x980),local_1024 + -1,local_1004,2);
    if (iVar23 != 0) {
      local_470 = (char *)0x202502310;
      local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x434));
      FUN_00216878(&local_470,0x10);
    }
    if (*(char *)(param_1 + 0x5f8) != '\0') {
      local_460 = 0;
      local_468 = &local_458;
      *(undefined1 *)(param_1 + 0x617) = 0;
      local_458 = local_458 & 0xffffffffffffff00;
      uStack_448 = &local_438;
      local_440 = 0;
      uStack_43c = 0;
      local_438 = local_438 & 0xffffffffffffff00;
      local_420 = &local_410;
      local_418 = 0;
      local_410 = local_410 & 0xffffffffffffff00;
      local_470 = (char *)((ulong)local_470 & 0xffffffffffffff00);
      local_428 = local_428 & 0xffffffffffffff00;
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEPKc(&uStack_448,param_1 + 0x5f8);
      iVar23 = FUN_00350290(*(undefined8 *)(param_1 + 0x980),&local_470);
      if (iVar23 != 0) {
        local_9f4 = 2;
        local_9f8 = 0x2492310;
        local_9f0 = CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x434));
        FUN_00216878(&local_9f8,0x10);
      }
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_420);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&uStack_448);
      _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_468);
    }
    if ((bVar3 == 0) || (*(int *)(param_1 + 0x5ec) != 0)) {
LAB_00221714:
      if ((bVar3 & local_181c == 0) != 0) {
        local_9f8 = 1;
        if (*(int *)(param_1 + 0x5ec) == 0) {
          local_9f8 = 0x101;
        }
        local_9f4 = CONCAT31(local_9f4._1_3_,*(undefined1 *)(param_1 + 0x5b8));
        local_9f0 = *(ulong *)(param_1 + 0x5c0);
        iVar23 = FUN_003500e8(*(undefined8 *)(param_1 + 0x980),&local_9f8);
        if (iVar23 != 0) {
          local_470 = (char *)0x202512310;
          local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x434));
          FUN_00216878(&local_470,0x10);
        }
        local_181c = 0;
      }
    }
    else {
      iVar23 = 0x22;
      do {
        FUN_00216738(&local_470,param_1 + 0x448);
        iVar25 = FUN_002acfc0(&local_470,&local_f58);
        _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_470);
        if (iVar25 == 0) goto LAB_00221714;
        usleep(30000);
        iVar23 = iVar23 + -1;
      } while (iVar23 != 0);
      local_470 = (char *)0x402522310;
      FUN_00216878(&local_470,8);
      local_181c = 2;
    }
    local_1000 = 0;
    iVar23 = FUN_0034fbac(*(undefined8 *)(param_1 + 0x980),*(undefined8 *)(param_1 + 0x918),0x200000
                          ,&local_1000);
    if (iVar23 != 0) {
      local_470 = (char *)0x202582310;
      local_468 = (long *)CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x434));
      FUN_00216878(&local_470,0x10);
    }
    _ZN3mpr10VpgManager9StopWriteEjNS_10StreamTypeE
              (*(undefined4 *)(param_1 + 0x430),*(undefined4 *)(param_1 + 0x444));
    local_468 = *(long **)(param_1 + 0x108);
    local_d0 = (void *)0x0;
    local_c8 = 0;
    local_460 = local_1850;
    local_450._0_4_ = local_fa0;
    uStack_448._0_4_ = (undefined4)*(undefined8 *)(param_1 + 0x918);
    uStack_448._4_4_ = (undefined4)((ulong)*(undefined8 *)(param_1 + 0x918) >> 0x20);
    local_440 = local_1000;
    local_110 = CONCAT44(uStack_ebc,local_ec0);
    uStack_108 = local_eb8;
    local_100 = local_eb0;
    local_f8 = *(undefined8 *)(param_1 + 0x860);
    uStack_f0 = *(undefined8 *)(param_1 + 0x868);
    local_e8 = *(undefined8 *)(param_1 + 0x870);
    local_d8 = *(char *)(param_1 + 0xcd8);
    local_470 = (char *)uVar32;
    local_438 = uVar34;
    local_118 = __s;
    local_e0 = uVar73;
    if (local_d8 != '\0') {
      pvVar41 = (void *)_Znam(*(undefined4 *)(param_1 + 0x2c1c));
      uVar54 = *(uint *)(param_1 + 0x2c1c);
      local_d0 = memcpy(pvVar41,(void *)(param_1 + 0xcd9),(ulong)uVar54);
      local_c8 = uVar54;
    }
    local_c0 = (void *)0x0;
    local_b8 = 0;
    local_c4 = 0;
    if ((*(char *)(param_1 + 0x5a5) != '\0') && (*(char *)(param_1 + 0xc30) != '\0')) {
      pvVar41 = (void *)_Znam(*(undefined4 *)(param_1 + 0xc70));
      uVar54 = *(uint *)(param_1 + 0xc70);
      local_c0 = memcpy(pvVar41,*(void **)(param_1 + 0xc28),(ulong)uVar54);
      local_c4 = 1;
      local_b8 = uVar54;
    }
    local_b4 = *(int *)(param_1 + 0x56c) == 2;
    local_b3 = *(int *)(param_1 + 0x444) != 0;
    local_8 = local_fe0;
    if (bVar3 == 0) {
      local_b2 = 0;
      local_a0 = 0;
      uStack_98 = 0;
      local_b0 = 0;
      uStack_a8 = 0;
      local_90 = 0;
      uStack_88 = 0;
      local_80 = 0;
      uStack_78 = 0;
      local_70 = 0;
      local_68 = 0;
      local_60 = 0;
      local_58 = 0;
      local_40 = 0;
      uStack_38 = 0;
      local_50 = 0;
      uStack_48 = 0;
      local_30 = 0;
      local_28 = 0;
      *(undefined4 *)((long)plVar31 + 0x14) = 0;
      *(undefined4 *)(plVar31 + 3) = 0;
    }
    else {
      if (local_181c != 0) {
        *(undefined4 *)((long)plVar31 + 0x1c) = 0;
      }
      local_b2 = 1;
      puVar42 = *(undefined8 **)(param_1 + 0x5f0);
      local_b0 = *puVar42;
      uStack_a8 = puVar42[1];
      local_a0 = puVar42[2];
      uStack_98 = puVar42[3];
      local_90 = puVar42[4];
      uStack_88 = puVar42[5];
      local_80 = puVar42[6];
      uStack_78 = puVar42[7];
      local_68 = (ulong)*(uint *)(param_1 + 0x5d8);
      local_60 = *(undefined8 *)(param_1 + 0x5e0);
      local_58 = *(undefined4 *)(param_1 + 0x5b4);
      local_50 = *(undefined8 *)(param_1 + 0x5b8);
      uStack_48 = *(undefined8 *)(param_1 + 0x5c0);
      local_40 = *(undefined8 *)(param_1 + 0x5c8);
      uStack_38 = *(undefined8 *)(param_1 + 0x5d0);
      local_30 = *(undefined8 *)(param_1 + 0x54e);
      local_28 = *(undefined4 *)(param_1 + 0x556);
      local_20 = CONCAT44(uStack_f54,local_f58);
      uStack_18 = uStack_f50;
      local_10 = *(undefined4 *)(param_1 + 0x5ec);
      *(int *)((long)plVar31 + 0x14) = (int)plVar31[3];
      local_70 = uVar33 - 0x18;
    }
    *(undefined4 *)((long)plVar31 + 0x1c) = 0;
    memcpy(&local_430,(void *)(param_1 + 0x448),0x106);
    memcpy(auStack_32a,(void *)(param_1 + 0x9d0),0x106);
    memcpy(auStack_224,(void *)(param_1 + 0xad6),0x106);
    FUN_002178cc(param_1,*(undefined4 *)(param_1 + 0x430),&local_470);
    free(__ptr);
    if (*(int *)(param_1 + 0x56c) == 2) {
      iVar23 = FUN_002060a8(*(undefined8 *)(param_1 + 0x998),*(undefined8 *)(param_1 + 0x8a8),1,
                            *(undefined8 *)(param_1 + 0x848));
      if (iVar23 != 0) {
        local_9f8 = 0x2602310;
        local_9f0 = CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x430));
        local_9f4 = 2;
        FUN_00216878(&local_9f8,0x10);
      }
      FUN_00205ff4(*(undefined8 *)(param_1 + 0x998));
      if (*(long **)(param_1 + 0x998) != (long *)0x0) {
        (**(code **)(**(long **)(param_1 + 0x998) + 8))();
      }
      *(undefined8 *)(param_1 + 0x998) = 0;
    }
    lVar44 = *(long *)(param_1 + 0x9a0);
    if (lVar44 != 0) {
      FUN_001fb514(lVar44);
      _ZdlPvm(lVar44,0x30);
    }
    lVar44 = *(long *)(param_1 + 0x9a8);
    *(undefined8 *)(param_1 + 0x9a0) = 0;
    if (lVar44 != 0) {
      FUN_001fb38c(lVar44);
      _ZdlPvm(lVar44,0x38);
    }
    *(undefined8 *)(param_1 + 0x9a8) = 0;
    if (*(long **)(param_1 + 0x128) != (long *)0x0) {
      (**(code **)(**(long **)(param_1 + 0x128) + 8))();
    }
    *(undefined8 *)(param_1 + 0x128) = 0;
    if (*(long *)(param_1 + 0x38) != 0) {
      local_ffc = 0;
      _ZN3mpr5pconv14SetUInt32be2leERjj(&local_ffc,local_102c);
      local_9d8 = 0x3d;
      local_9ac = 0;
      local_9a4 = 0;
      local_9a0 = 0;
      local_998 = 0;
      memset(auStack_994,0,0x80);
      local_90c = 0;
      local_914 = 0;
      local_9e8 = (uint *)(local_fe0 + local_1850);
      local_908 = 0;
      local_9f0 = *(ulong *)(param_1 + 0x988);
      local_9f8 = local_1004;
      local_9d0 = local_1004;
      local_9d4 = local_ffc;
      uVar54 = 0;
      local_9cc = local_f48;
      uStack_9c4 = uStack_f40;
      local_9bc = local_f38;
      local_9e0._0_5_ = CONCAT14(local_103f[0],*(undefined4 *)(param_1 + 0x430));
      local_9b0 = local_1000;
      local_9b4 = local_1024;
      local_fc0 = CONCAT11(local_fc0._1_1_,uVar8);
      local_fc8 = uVar20;
      uStack_fc6 = uVar9;
      uStack_fc4 = uVar7;
      uStack_fc3 = uVar48;
      uStack_fc2 = uVar5;
      uStack_fc1 = uVar6;
      FUN_00218aa0(param_1,uVar39,local_fc0,&local_9ac);
      uVar26 = 0;
      local_fb8 = uVar12;
      uStack_fb6 = uVar13;
      uStack_fb4 = uVar14;
      uStack_fb3 = uVar15;
      uStack_fb2 = uVar16;
      uStack_fb1 = uVar17;
      local_fb0 = CONCAT11(local_fb0._1_1_,uVar18);
      FUN_00218aa0(param_1,CONCAT17(uVar17,CONCAT16(uVar16,CONCAT15(uVar15,CONCAT14(uVar14,CONCAT22(
                                                  uVar13,uVar12))))),local_fb0,&local_9a0);
      _ZN16XavcsMovieWriter29ConvertMPFPictureQualityLevelEN3mpr14VideoParamCombEN6NRTDEF5LevelENS2_12VideoProfileEPN17LibMediaProfileIf28T_XAVCS_STREAM_PROFILE_LEVELE
                (param_1,*(undefined4 *)(param_1 + 0x560),local_db4,local_dc0,&local_9d8);
      pvVar41 = memcpy(auStack_1810,auStack_dc8,0x50);
      _ZN16XavcsMovieWriter25ConvertVideoCodecTypeNameEN6NRTDEF14VideoRecFormatEN3mpr10StreamTypeEPc
                (param_1,pvVar41,*(undefined4 *)(param_1 + 0x444),auStack_994);
      uVar73 = local_914 & 0xff;
      uVar57 = 0;
      uVar59 = 0;
      uVar51 = (uint)local_90c._1_1_;
      uVar61 = 0;
      uVar63 = 0;
      uVar28 = 0;
      uVar53 = (uint)(byte)local_90c;
      uVar56 = (uint)local_914._7_1_;
      uVar58 = (uint)local_914._6_1_;
      uVar60 = (uint)local_914._5_1_;
      uVar49 = 0;
      uVar62 = (uint)local_914._4_1_;
      uVar52 = 0;
      uVar64 = (uint)local_914._3_1_;
      uVar55 = 0;
      uVar65 = (uint)local_914._2_1_;
      uVar68 = (uint)local_914._1_1_;
      for (puVar43 = *(uint **)(param_1 + 0xc08); puVar43 != *(uint **)(param_1 + 0xc10);
          puVar43 = puVar43 + 0xc) {
        if (*puVar43 < 10) {
          uVar75 = uVar22 & 0xff;
          switch(*puVar43) {
          case 0:
            uVar54 = uVar29;
            uVar68 = uVar29;
            break;
          case 1:
            uVar26 = uVar75;
            uVar65 = uVar75;
            break;
          case 2:
            uVar28 = uVar75;
            uVar64 = uVar75;
            break;
          case 3:
            uVar63 = uVar75;
            uVar62 = uVar75;
            break;
          case 4:
            uVar61 = uVar75;
            uVar60 = uVar75;
            break;
          case 5:
            uVar59 = uVar75;
            uVar58 = uVar75;
            break;
          case 6:
            uVar57 = uVar75;
            uVar56 = uVar75;
            break;
          case 7:
            uVar55 = uVar75;
            uVar53 = uVar75;
            break;
          case 8:
            uVar52 = uVar75;
            uVar51 = uVar75;
            break;
          case 9:
            uVar73 = (ulong)uVar74;
            uVar49 = uVar74;
          }
        }
      }
      if (uVar54 != 0) {
        local_914._0_2_ = CONCAT11((char)uVar68,(undefined1)local_914);
      }
      if (uVar26 != 0) {
        local_914._0_3_ = CONCAT12((char)uVar65,(undefined2)local_914);
      }
      if (uVar28 != 0) {
        local_914._0_4_ = CONCAT13((char)uVar64,(undefined3)local_914);
      }
      if (uVar63 != 0) {
        local_914._0_5_ = CONCAT14((char)uVar62,(undefined4)local_914);
      }
      if (uVar61 != 0) {
        local_914._0_6_ = CONCAT15((char)uVar60,(undefined5)local_914);
      }
      if (uVar59 != 0) {
        local_914._0_7_ = CONCAT16((char)uVar58,(undefined6)local_914);
      }
      if (uVar57 != 0) {
        local_914 = CONCAT17((char)uVar56,(undefined7)local_914);
      }
      if (uVar55 != 0) {
        local_90c = CONCAT11(local_90c._1_1_,(char)uVar53);
      }
      if (uVar52 != 0) {
        local_90c = CONCAT11((char)uVar51,(byte)local_90c);
      }
      if (uVar49 != 0) {
        local_914 = CONCAT71(local_914._1_7_,(char)uVar73);
      }
      local_f93 = 0;
      local_f92 = 0;
      local_f91 = 0;
      local_908 = CONCAT71(local_908._1_7_,(char)local_e60);
      local_908 = CONCAT44(local_e60._4_4_,(undefined4)local_908);
      local_f94 = 3;
      local_f98 = 0x2682310;
      local_f90 = *(undefined4 *)(param_1 + 0x430);
      FUN_00216878(&local_f98,0xc);
      (**(code **)(param_1 + 0x38))
                (*(undefined8 *)(param_1 + 0x40),0x40000001,0,&local_9f8,0,
                 *(undefined8 *)(param_1 + 0x48));
    }
    *(undefined8 *)(param_1 + 0x108) = 0;
    iVar23 = FUN_0020996c(uVar30);
    if (iVar23 != 0) {
      local_9f4 = 2;
      local_9f8 = 0x2702310;
      local_9f0 = CONCAT44(iVar23,*(undefined4 *)(param_1 + 0x430));
      FUN_00216878(&local_9f8,0x10);
    }
    FUN_00207af0(uVar30);
    _ZdlPvm(uVar30,0x5cf0);
    *(undefined8 *)(param_1 + 0x980) = 0;
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_a18);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_a48);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_f08);
    _ZN3mpr6thread10normalLockD2Ev(auStack_e58);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_e80);
    _ZNSt12_Vector_baseIN3mpr16AudioEncodeTable9AudioInfoESaIS2_EED1Ev(auStack_bb8);
    _ZNSt12_Vector_baseIN3mpr16VideoEncodeTable9VideoInfoESaIS2_EED1Ev(auStack_be8);
    _ZNSt12_Vector_baseIN3mpr16AudioEncodeTable9AudioInfoESaIS2_EED1Ev(local_b20);
    _ZNSt12_Vector_baseIN3mpr16VideoEncodeTable9VideoInfoESaIS2_EED1Ev(&local_b50);
    FUN_0021aa18(&local_b78);
    FUN_00217700(auStack_828);
    FUN_00217700(auStack_d28);
    _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(auStack_f30);
  } while( true );
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_21113704a8d86633eeae5e53bb1e0c80e459b2f15fadc5afd10f418c7aa861b3
// Source record: research/evidence/rea/still-sink-finalizers/05-procedure_pseudo_code.json

undefined8
_ZN10HEIFCoreIf3FinEPN10HEIFCommon17AREA_MULTI_LIST_TEbPN3mpr8libcrypt10assetParamE
          (long param_1,uint *param_2,char param_3,undefined8 param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  uint uVar6;
  char *__assertion;
  long *plVar7;
  ulong uVar8;
  uint uVar9;
  uint uVar10;
  long lVar11;
  int iVar12;
  long lVar13;
  ulong uVar14;
  long local_2c0;
  int local_2b4;
  uint local_2a8;
  int local_2a4;
  ulong local_2a0;
  undefined8 local_298;
  undefined2 local_290;
  undefined2 local_28e;
  undefined1 local_28c;
  undefined1 local_28b;
  undefined1 local_28a;
  undefined1 local_289;
  uint local_288;
  undefined4 uStack_284;
  long local_280;
  undefined4 local_278;
  undefined4 uStack_274;
  uint local_270 [4];
  long local_260;
  uint local_250;
  int iStack_24c;
  undefined4 local_248;
  long local_240;
  int local_230;
  int local_22c;
  undefined4 local_228;
  long local_220;
  int local_210;
  uint local_20c;
  undefined4 local_208;
  long local_200;
  int local_1f0;
  uint local_1ec;
  undefined4 local_1e8;
  
  local_280 = 0x220012262;
  local_278 = 0;
  uStack_274 = 0;
  FUN_0036522c(&local_280,0x10);
  FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0xfd,&DAT_003dd440);
  FUN_0035e7a8(1,"start ---------------------------------\n");
  local_2a4 = 0;
  lVar13 = *(long *)(*(long *)(param_1 + 0x30) + 0x1d0);
  memset(&local_280,0,0x280);
  FUN_0035e9d4(*(undefined8 *)(param_1 + 0x10),lVar13,&local_2a8);
  uVar4 = *(undefined4 *)(*(long *)(param_1 + 0x30) + 0x1dc);
  lVar11 = lVar13 + (ulong)local_2a8;
  local_270[0] = 0;
  local_270[1] = local_2a8;
  local_280 = lVar13;
  local_270[2] = uVar4;
  if (param_3 == '\0') {
    local_2c0 = 0;
    local_2b4 = 0;
    lVar13 = lVar11;
    uVar9 = local_2a8;
  }
  else {
    local_2a0 = local_2a0 & 0xffffffff00000000;
    _ZN10HEIFCoreIC15createUUIDJumbfEPhPjPN3mpr8libcrypt10assetParamElPNS3_9cryptAreaEjb
              (*(undefined8 *)(param_1 + 0x10),0,&local_2a0,0,0,0,0,1);
    local_28b = 0;
    local_28a = 0;
    local_28c = 2;
    local_289 = 0;
    local_28e = 0x2094;
    local_290 = 0x2262;
    local_288 = (int)local_2a0;
    uStack_284 = 0;
    FUN_0036522c(&local_290,0x10);
    local_2b4 = local_2a8 + (int)local_2a0;
    uVar10 = (int)local_2a0 + 0x3000;
    lVar13 = lVar11 + (ulong)uVar10;
    uVar9 = local_2a8 + uVar10;
    local_2c0 = lVar11;
    local_2a8 = uVar10;
  }
  local_28b = 0;
  local_28c = 2;
  local_28a = 0;
  local_28e = 0x2004;
  local_289 = 0;
  local_290 = 0x2262;
  uStack_284 = 0;
  local_288 = uVar9;
  FUN_0036522c(&local_290,0x10);
  local_2a0 = 0;
  local_298 = 0;
  FUN_00361f0c(*(undefined8 *)(param_1 + 0x10),lVar13,&local_2a8,uVar9,&local_2a0,
               *(undefined4 *)(param_1 + 0x48));
  if (local_298._4_4_ != 0) {
    iStack_24c = (int)local_2a0 - (int)lVar13;
    local_22c = (local_2a8 - iStack_24c) - local_298._4_4_;
    local_230 = local_298._4_4_ + (int)local_298;
    local_240 = local_2a0 + local_298._4_4_;
    lVar11 = lVar13 + (ulong)local_2a8;
    iVar12 = uVar9 + local_2a8;
    local_260 = lVar13;
    local_250 = uVar9;
    local_248 = uVar4;
    local_228 = uVar4;
    FUN_00362220(*(undefined8 *)(param_1 + 0x10),lVar11,&local_2a8,iVar12);
    local_20c = local_2a8;
    uVar8 = (ulong)local_2a8;
    iVar1 = iVar12 + local_2a8;
    local_220 = lVar11;
    local_210 = iVar12;
    local_208 = uVar4;
    FUN_00362458(*(undefined8 *)(param_1 + 0x10),param_2,&local_2a4);
    FUN_00362334(*(undefined8 *)(param_1 + 0x10),lVar11 + uVar8,&local_2a8,
                 local_2a4 - *(int *)(*(long *)(param_1 + 0x30) + 0x1d8));
    uVar9 = 0;
    uVar10 = iVar1 + local_2a8;
    local_1ec = local_2a8;
    uVar14 = 5;
    local_200 = lVar11 + uVar8;
    local_1f0 = iVar1;
    local_1e8 = uVar4;
    do {
      iVar12 = (int)uVar14;
      if (*param_2 <= uVar9) {
        if (param_3 != '\0') {
          local_28b = 0;
          local_28c = 2;
          local_28a = 0;
          local_290 = 0x2262;
          local_289 = 0;
          local_28e = 0x2050;
          local_288 = 0;
          uStack_284 = 0;
          FUN_0036522c(&local_290,0x10);
          _ZN10HEIFCoreIC15createUUIDJumbfEPhPjPN3mpr8libcrypt10assetParamElPNS3_9cryptAreaEjb
                    (*(undefined8 *)(param_1 + 0x10),local_2c0,&local_2a8,param_4,local_2b4,
                     &local_280,uVar14,0);
        }
        local_28b = 0;
        local_28c = 2;
        local_28a = 0;
        local_290 = 0x2262;
        local_289 = 0;
        local_28e = 0x20ff;
        local_288 = 0;
        uStack_284 = 0;
        FUN_0036522c(&local_290,0x10);
        return 0;
      }
      uVar5 = param_2[(ulong)uVar9 + 4];
      if (uVar5 == 5) {
        plVar7 = (long *)_Znwm(0x1d060);
        FUN_003798a0();
        lVar11 = (ulong)uVar9 * 0x18;
        local_288 = FUN_0037a56c(plVar7,*(undefined8 *)(*(long *)(param_2 + 2) + lVar11 + 8),
                                 *(undefined4 *)(*(long *)(param_2 + 2) + lVar11 + 0x10),0,0);
        uVar5 = iVar12 + 1;
        if (local_288 == 0) {
          local_28b = 0;
          uVar2 = *(uint *)((long)plVar7 + 0x1cff4);
          local_28a = 0;
          local_28c = 3;
          local_290 = 0x2262;
          local_28e = 0x2062;
          local_289 = 0;
          local_288 = uVar2;
          FUN_0036522c(&local_290,0xc);
          uVar8 = (ulong)uVar5;
          lVar11 = *(long *)(param_2 + 2) + lVar11;
          lVar13 = *(long *)(lVar11 + 8);
          uVar3 = *(uint *)(lVar11 + 0x14);
          (&local_280)[uVar14 * 4] = lVar13;
          local_270[uVar14 * 8] = uVar10;
          local_270[uVar14 * 8 + 1] = uVar2;
          local_270[uVar14 * 8 + 2] = uVar3;
          if (uVar5 < 0x14) {
            uVar10 = uVar2 + uVar10 + 1;
            uVar6 = (*(int *)(lVar11 + 0x10) + -1) - uVar2;
            uVar5 = iVar12 + 2;
            (&local_280)[uVar8 * 4] = lVar13 + 1 + (ulong)uVar2;
            local_270[uVar8 * 8] = uVar10;
            local_270[uVar8 * 8 + 1] = uVar6;
            uVar10 = uVar10 + uVar6;
            local_270[uVar8 * 8 + 2] = uVar3;
            if (uVar5 != 0x14) goto LAB_00365e34;
            uVar9 = 0x183;
          }
          else {
            uVar9 = 0x17b;
          }
        }
        else {
          local_28c = 3;
          local_28b = 0;
          local_290 = 0x2262;
          local_28a = 0;
          local_28e = 0x2063;
          local_289 = 0;
          FUN_0036522c(&local_290,0xc);
          lVar11 = *(long *)(param_2 + 2) + lVar11;
          uVar2 = *(uint *)(lVar11 + 0x10);
          uVar3 = *(uint *)(lVar11 + 0x14);
          (&local_280)[uVar14 * 4] = *(long *)(lVar11 + 8);
          local_270[uVar14 * 8] = uVar10;
          local_270[uVar14 * 8 + 1] = uVar2;
          local_270[uVar14 * 8 + 2] = uVar3;
          uVar10 = uVar10 + uVar2;
          if (uVar5 < 0x14) {
LAB_00365e34:
            (**(code **)(*plVar7 + 8))(plVar7);
            uVar8 = (ulong)uVar5;
            goto LAB_00365e44;
          }
          uVar9 = 0x175;
        }
        goto LAB_00365d48;
      }
      uVar8 = uVar14;
      if (uVar5 != 6) {
        uVar2 = iVar12 + 1;
        uVar8 = (ulong)uVar2;
        lVar11 = *(long *)(param_2 + 2) + (ulong)uVar9 * 0x18;
        uVar3 = *(uint *)(lVar11 + 0x10);
        if (uVar5 == 4) {
          uVar5 = *(uint *)(lVar11 + 0x14);
          (&local_280)[uVar14 * 4] = *(long *)(lVar11 + 8);
          local_270[uVar14 * 8] = uVar10;
          local_270[uVar14 * 8 + 1] = uVar3;
          uVar10 = uVar10 + uVar3;
          local_270[uVar14 * 8 + 2] = uVar5;
          if (0x13 < uVar2) {
            uVar9 = 0x18e;
LAB_00365d48:
            __assertion = "hashListNum < MAX_ARRAY_NUM";
            goto LAB_00365b94;
          }
          uVar5 = *(uint *)(param_1 + 0x48);
          if (uVar5 != 0) {
            uVar2 = *(uint *)(param_1 + 0x4c);
            (&local_280)[uVar8 * 4] = *(long *)(param_1 + 0x40);
            local_270[uVar8 * 8] = uVar10;
            local_270[uVar8 * 8 + 1] = uVar5;
            uVar10 = uVar10 + uVar5;
            local_270[uVar8 * 8 + 2] = uVar2;
            uVar8 = (ulong)(iVar12 + 2U);
            if (iVar12 + 2U == 0x14) {
              uVar9 = 0x195;
              goto LAB_00365d48;
            }
          }
        }
        else {
          uVar5 = *(uint *)(lVar11 + 0x14);
          (&local_280)[uVar14 * 4] = *(long *)(lVar11 + 8);
          local_270[uVar14 * 8] = uVar10;
          local_270[uVar14 * 8 + 1] = uVar3;
          uVar10 = uVar10 + uVar3;
          local_270[uVar14 * 8 + 2] = uVar5;
          if (0x13 < uVar2) {
            uVar9 = 0x19a;
            goto LAB_00365d48;
          }
        }
      }
LAB_00365e44:
      uVar9 = uVar9 + 1;
      uVar14 = uVar8;
    } while( true );
  }
  __assertion = "irotValueArea.size != 0";
  uVar9 = 0x126;
LAB_00365b94:
                    /* WARNING: Subroutine does not return */
  __assert_fail(__assertion,"core/HEIFCoreIf.cpp",uVar9,
                "HEIFCommon::HEIF_RET_T HEIFCoreIf::Fin(HEIFCommon::AREA_MULTI_LIST_T*, bool, mpr::libcrypt::assetParam_t*)"
               );
}


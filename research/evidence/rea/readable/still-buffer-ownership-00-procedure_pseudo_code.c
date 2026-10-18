// Decompiled pseudocode, not original source.
// REA Evidence: ev_a83c86cd9296c80489313877ad3191d28148c6aa279911763bc2625c4f46a9eb
// Source record: research/evidence/rea/still-buffer-ownership/00-procedure_pseudo_code.json

undefined8
_ZN3mpr11stillrecord7_shrareERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tEbPP10dmm_munmap
          (undefined8 param_1,long param_2,long param_3,long param_4,long param_5,long param_6,
          byte param_7,undefined8 *param_8)

{
  int iVar1;
  long lVar2;
  int *piVar3;
  long lVar4;
  char *pcVar5;
  uint uVar6;
  uint uVar7;
  char *pcVar8;
  long local_e8 [8];
  undefined1 auStack_a8 [168];
  
  pcVar5 = (char *)(param_2 + 0x23c);
  do {
    uVar6 = *(uint *)(pcVar5 + -0x30);
    if (uVar6 != 0xffffffff) {
      pcVar8 = pcVar5;
      do {
        if (*pcVar8 != '\0') {
          iVar1 = *(int *)(pcVar5 + -0x48);
          local_e8[0] = 0;
          local_e8[1] = 0;
          local_e8[2] = 0;
          local_e8[3] = 0;
          local_e8[4] = 0;
          local_e8[5] = 0;
          local_e8[6] = 0;
          local_e8[7] = 0;
          if ((param_7 & iVar1 == 1) == 0) {
            if ((param_7 & iVar1 == 4) == 0) {
              if (*(int *)(param_2 + 0xc) == 2) {
                if (iVar1 != 1) {
                  if (iVar1 == 4) {
                    local_e8[0] = *(long *)(param_5 + (ulong)uVar6 * 8);
                    local_e8[1] = local_e8[0] + 0x90;
                    local_e8[2] = local_e8[0] + 0xa0;
                    local_e8[3] = local_e8[0] + 0xb0;
                    local_e8[5] = local_e8[0] + 0x80;
                    local_e8[4] = local_e8[0] + 0xc0;
                  }
                  else {
                    if (iVar1 != 2) {
LAB_001e2a10:
                      pcVar5 = "0";
                      uVar6 = 0x1286;
LAB_001e2a2c:
                    /* WARNING: Subroutine does not return */
                      __assert_fail(pcVar5,"integRecordStill.cpp",uVar6,
                                    "int mpr::stillrecord::_shrare(const mpr::recordStillInParam&, mpr::recordStillParam_t*, DefStruct::JpgInfo* (&)[2], DefStruct::RawInfo* (&)[2], DefStruct::HifInfo* (&)[2], still::common_t (&)[2], bool, dmm_munmap**)"
                                   );
                    }
LAB_001e2994:
                    local_e8[0] = *(long *)(param_4 + (ulong)uVar6 * 8);
                    local_e8[1] = local_e8[0] + 0x90;
                    local_e8[2] = local_e8[0] + 0xa0;
                    local_e8[3] = local_e8[0] + 0x110;
                    local_e8[4] = local_e8[0] + 0x120;
                    local_e8[5] = local_e8[0] + 0x130;
                  }
                  uVar7 = 6;
                  goto LAB_001e2880;
                }
                local_e8[0] = *(long *)(param_3 + (ulong)uVar6 * 8);
                local_e8[1] = local_e8[0] + 0x90;
                local_e8[2] = local_e8[0] + 0xa0;
                local_e8[4] = local_e8[0] + 0x80;
                local_e8[3] = local_e8[0] + 0xb0;
              }
              else {
                if (iVar1 == 1) {
                  local_e8[0] = *(long *)(param_3 + (ulong)uVar6 * 8);
                  local_e8[1] = local_e8[0] + 0x90;
                  local_e8[2] = local_e8[0] + 0xa0;
                  local_e8[3] = local_e8[0] + 0xb0;
                  goto LAB_001e283c;
                }
                if (iVar1 == 2) goto LAB_001e2994;
                if (iVar1 != 4) goto LAB_001e2a10;
                local_e8[0] = *(long *)(param_5 + (ulong)uVar6 * 8);
                local_e8[1] = local_e8[0] + 0x90;
                local_e8[2] = local_e8[0] + 0xa0;
                local_e8[3] = local_e8[0] + 0xb0;
                local_e8[4] = local_e8[0] + 0xc0;
              }
              uVar7 = 5;
            }
            else {
              lVar2 = *(long *)(param_5 + (ulong)uVar6 * 8);
              uVar7 = 8;
              local_e8[5] = param_6;
              local_e8[6] = param_6 + 0x150;
              local_e8[7] = param_6 + 0x180;
              local_e8[0] = lVar2 + 0x90;
              local_e8[1] = lVar2 + 0xa0;
              local_e8[2] = lVar2 + 0xb0;
              local_e8[3] = lVar2 + 0xc0;
            }
          }
          else {
            lVar2 = *(long *)(param_3 + (ulong)uVar6 * 8);
            local_e8[3] = param_6;
            local_e8[0] = lVar2 + 0x90;
            local_e8[1] = lVar2 + 0xa0;
            local_e8[2] = lVar2 + 0xb0;
LAB_001e283c:
            uVar7 = 4;
          }
LAB_001e2880:
          lVar2 = 0;
          do {
            piVar3 = (int *)local_e8[lVar2];
            if (((piVar3 != (int *)0x0) && (*piVar3 != 0)) &&
               (lVar4 = FUN_003b2258(piVar3[3],*piVar3,auStack_a8,1,*param_8), lVar4 == 0)) {
              uVar6 = 0x1292;
              pcVar5 = 
              "app_dmm::share_and_open( area[j]->unitType, area[j]->memHandle, &tmp_unit, true, pp_dmm_munmap[i] )"
              ;
              goto LAB_001e2a2c;
            }
            lVar2 = lVar2 + 1;
          } while ((uint)lVar2 < uVar7);
        }
        pcVar8 = pcVar8 + 0x106;
      } while (pcVar8 != pcVar5 + 0x418);
    }
    pcVar5 = pcVar5 + 0x588;
    param_8 = param_8 + 1;
    param_6 = param_6 + 0x2a0;
    if (pcVar5 == (char *)(param_2 + 0xd4c)) {
      return 0;
    }
  } while( true );
}


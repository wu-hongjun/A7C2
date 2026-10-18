// Decompiled pseudocode, not original source.
// REA Evidence: ev_4eec2aae2d60cce820fa70c12832ee2e3b3fcfe225126d601bf853352cfa2fc0
// Source record: research/evidence/rea/still-buffer-ownership/03-procedure_pseudo_code.json

void _ZN3mpr11stillrecord20ShareStillInfoMemoryEibPNS_18recordStillParam_tEP10dmm_munmapPN5still8fin_if_tEbb
               (int param_1,char param_2,long param_3,undefined8 param_4,long param_5,char param_6,
               char param_7)

{
  uint uVar1;
  int *piVar2;
  long lVar3;
  char *pcVar4;
  long lVar5;
  ulong uVar6;
  long local_e8 [8];
  undefined2 local_a8;
  undefined2 local_a6;
  undefined1 local_a4;
  undefined1 local_a3;
  undefined1 local_a2;
  undefined1 local_a1;
  undefined4 local_a0;
  
  lVar5 = param_3 + (long)param_1 * 0x588;
  if (*(int *)(lVar5 + 0x20c) == -1) {
    local_a3 = 0;
    local_a2 = 0;
    local_a4 = 3;
    local_a1 = 0;
    local_a8 = 0x2120;
    local_a6 = 0x8202;
    local_a0 = osal_thread_id();
    FUN_001df8d4(&local_a8,0xc);
  }
  else {
    pcVar4 = (char *)(param_3 + (long)param_1 * 0x588 + 0x23c);
    do {
      if (*pcVar4 != '\0') {
        uVar1 = *(uint *)(lVar5 + 500);
        local_e8[0] = 0;
        local_e8[1] = 0;
        local_e8[2] = 0;
        local_e8[3] = 0;
        local_e8[4] = 0;
        local_e8[5] = 0;
        local_e8[6] = 0;
        local_e8[7] = 0;
        if (param_2 == '\0') {
          lVar3 = param_5 + 0xc0;
          if (uVar1 == 1) {
            local_e8[2] = lVar3;
            local_e8[0] = param_5;
            local_e8[1] = param_5 + 0x30;
            uVar1 = 3;
            if (param_6 != '\0') {
              local_e8[3] = param_5 + 0x120;
              uVar1 = 4;
            }
          }
          else if (uVar1 == 2) {
            local_e8[0] = param_5;
            if (param_7 == '\0') {
              local_e8[1] = lVar3;
            }
            else {
              local_e8[1] = param_5 + 0xf0;
              if (param_6 != '\0') {
                local_e8[1] = param_5 + 0x120;
              }
              local_e8[2] = lVar3;
              uVar1 = 3;
            }
          }
          else {
            if (uVar1 != 4) {
              pcVar4 = "0";
              uVar1 = 0xd28;
LAB_001e0e28:
                    /* WARNING: Subroutine does not return */
              __assert_fail(pcVar4,"integRecordStill.cpp",uVar1,
                            "void mpr::stillrecord::ShareStillInfoMemory(int, bool, mpr::recordStillParam_t*, dmm_munmap*, still::fin_if_t*, bool, bool)"
                           );
            }
            local_e8[3] = lVar3;
            local_e8[0] = param_5;
            local_e8[1] = param_5 + 0x60;
            local_e8[2] = param_5 + 0x90;
          }
          uVar6 = 0;
          local_e8[0] = param_5;
          do {
            piVar2 = (int *)local_e8[uVar6];
            if (((piVar2 != (int *)0x0) && (*piVar2 != 0)) &&
               (lVar3 = FUN_003b2258(piVar2[3],*piVar2,&local_a8,1,param_4), lVar3 == 0)) {
              uVar1 = 0xd30;
              pcVar4 = 
              "app_dmm::share_and_open( area[i]->unitType, area[i]->memHandle, &tmp_unit, true, p_dmm_munmap )"
              ;
              goto LAB_001e0e28;
            }
            uVar6 = uVar6 + 1;
          } while (uVar6 != uVar1);
        }
      }
      pcVar4 = pcVar4 + 0x106;
    } while ((char *)(param_3 + (long)param_1 * 0x588 + 0x654) != pcVar4);
  }
  return;
}


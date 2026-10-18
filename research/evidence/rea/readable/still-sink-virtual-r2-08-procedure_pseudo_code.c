// Decompiled pseudocode, not original source.
// REA Evidence: ev_6694f94d96df226bfc4fd0f977cd040dbfcf8d291f37c236f9e54f489d250c3a
// Source record: research/evidence/rea/still-sink-virtual-r2/08-procedure_pseudo_code.json

undefined8
_ZN3mpr11stillrecord28_write_async_and_sidb_updateEPNS_18recordStillParam_tEjP8libstillj
          (long param_1,int param_2,long param_3,int param_4)

{
  undefined8 *puVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  char cVar6;
  bool bVar7;
  undefined4 uVar8;
  int iVar9;
  void *__ptr;
  size_t sVar10;
  int *piVar11;
  void *__dest;
  ulong uVar12;
  undefined8 uVar13;
  uint uVar14;
  long lVar15;
  undefined1 uVar16;
  undefined1 *puVar17;
  int iVar18;
  ulong uVar19;
  char *__src;
  long lVar20;
  undefined1 *puVar21;
  int iVar22;
  ulong uVar23;
  int *piVar24;
  void *pvVar25;
  char *local_5d8;
  int local_5cc;
  long local_5c8;
  int *local_5b0;
  long local_598;
  int local_590 [4];
  void *local_580;
  long local_578;
  long local_570;
  long local_568;
  undefined2 local_560;
  undefined2 local_55e;
  undefined4 local_55c;
  int local_558;
  int local_554;
  int local_550;
  ulong local_54c;
  undefined8 uStack_544;
  undefined8 local_53c;
  undefined8 uStack_534;
  undefined8 local_52c;
  undefined8 uStack_524;
  undefined8 local_51c;
  undefined8 uStack_514;
  undefined4 local_50c;
  ulong *local_508;
  undefined8 local_500;
  ulong local_4f8;
  undefined8 uStack_4f0;
  undefined1 auStack_4ec [4];
  undefined8 local_4e8;
  undefined8 uStack_4e0;
  undefined8 local_4d8;
  undefined8 uStack_4d0;
  undefined4 local_f8;
  ushort local_f4;
  ushort local_f2;
  ushort local_f0;
  ushort local_ee;
  undefined2 local_ec;
  undefined2 local_ea;
  short local_e8;
  undefined2 local_e6;
  short local_e4;
  undefined2 local_e2;
  undefined2 local_e0;
  int local_dc;
  ulong auStack_d8 [24];
  uint local_18;
  code *local_10;
  void *local_8;
  
  local_508 = (ulong *)0x150002120;
  local_500._0_4_ = osal_thread_id();
  local_4f8 = CONCAT44((int)param_3,param_2);
  local_500._4_4_ = (int)param_1;
  FUN_001df8d4(&local_508,0x18);
  local_590[0] = 0;
  local_590[1] = 0;
  local_590[2] = 0;
  local_590[3] = 0;
  __ptr = calloc(4,0x15f0);
  if (__ptr == (void *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_tmpBuffer!=NULL","integRecordStill.cpp",0xbf5,
                  "int mpr::stillrecord::_write_async_and_sidb_update(mpr::recordStillParam_t*, uint32_t, libstill*, uint32_t)"
                 );
  }
  local_578 = (long)__ptr + 0x15f0;
  puVar21 = (undefined1 *)(param_1 + 0x214);
  local_570 = (long)__ptr + 0x2be0;
  piVar11 = (int *)(param_1 + 0x130);
  lVar20 = 0;
  local_568 = (long)__ptr + 0x41d0;
  local_508 = (ulong *)0x3db082120;
  local_580 = __ptr;
  uVar8 = osal_thread_id();
  local_500 = CONCAT44(local_500._4_4_,uVar8);
  FUN_001df8d4(&local_508,0xc);
  do {
    uVar16 = 1;
    iVar9 = (int)lVar20;
    local_508 = (ulong *)0x0;
    local_500 = 0;
    iVar18 = 0;
    local_4f8 = 0;
    uStack_4f0 = 0;
    local_4e8 = 0;
    uStack_4e0 = 0;
    local_4d8 = 0;
    uStack_4d0 = 0;
    bVar7 = false;
    puVar17 = puVar21;
    uVar19 = 0;
    piVar24 = (int *)(param_1 + 500);
    do {
      uVar23 = uVar19;
      if ((piVar24[6] != -1) && (*(char *)((long)piVar24 + lVar20 * 0x106 + 0x48) != '\0')) {
        local_560 = 0x2120;
        local_55c = 1;
        local_55e = 0xdb09;
        local_558 = osal_thread_id();
        local_54c = local_54c & 0xffffffff00000000;
        local_554 = iVar9;
        local_550 = iVar18;
        FUN_001df8d4(&local_560,0x18);
        uVar23 = (ulong)((int)uVar19 + 1);
        uVar13 = *(undefined8 *)(piVar24 + 2);
        if (puVar17[0x24] == '\0') {
          uVar16 = 0;
        }
        (&local_508)[uVar19 * 4] = *(ulong **)piVar24;
        (&local_500)[uVar19 * 4] = uVar13;
        uVar13 = *(undefined8 *)(piVar24 + 6);
        (&local_4f8)[uVar19 * 4] = *(ulong *)(piVar24 + 4);
        *(undefined8 *)((long)&stack0xfffffffffffffb10 + uVar19 * 0x20) = uVar13;
        auStack_4ec[uVar19 * 0x20] = *puVar17;
        local_590[lVar20] = iVar18;
        bVar7 = true;
      }
      iVar18 = iVar18 + 1;
      piVar24 = piVar24 + 0x162;
      puVar17 = puVar17 + 0x588;
      uVar19 = uVar23;
    } while (iVar18 != 2);
    if (bVar7) {
      local_560 = 0x2120;
      piVar24 = (&local_580)[lVar20];
      *piVar24 = iVar9;
      local_55c = 1;
      local_55e = 0xdb10;
      local_558 = osal_thread_id();
      local_550 = *piVar24;
      local_54c = local_54c & 0xffffffff00000000;
      local_554 = iVar9;
      FUN_001df8d4(&local_560,0x18);
      local_560 = 0x2120;
      piVar24[4] = *piVar11;
      local_55c = 1;
      local_55e = 0xdb11;
      local_558 = osal_thread_id();
      local_554 = *piVar11;
      local_54c = local_54c & 0xffffffff00000000;
      local_550 = piVar24[4];
      FUN_001df8d4(&local_560,0x18);
      piVar24[5] = 0;
      iVar18 = *(int *)(param_1 + 300);
      *(undefined1 *)((long)piVar24 + 0x19) = uVar16;
      *(bool *)(piVar24 + 6) = iVar18 == 2;
      *(undefined1 *)((long)piVar24 + 0x1a) = puVar21[-0xd0];
      uVar16 = puVar21[-0xcc];
      piVar24[8] = (int)uVar23;
      *(undefined1 *)((long)piVar24 + 0x1b) = uVar16;
      for (lVar15 = 0; uVar23 << 5 != lVar15; lVar15 = lVar15 + 0x20) {
        puVar1 = (undefined8 *)((long)piVar24 + lVar15 + 0x24);
        uVar13 = *(undefined8 *)((long)&local_500 + lVar15);
        *puVar1 = *(undefined8 *)((long)&local_508 + lVar15);
        puVar1[1] = uVar13;
        uVar13 = *(undefined8 *)((long)&stack0xfffffffffffffb10 + lVar15);
        puVar1[2] = *(undefined8 *)((long)&local_4f8 + lVar15);
        puVar1[3] = uVar13;
      }
      *(undefined1 *)(piVar24 + 0x19) = *(undefined1 *)(param_1 + 0xde);
      piVar24[0x574] = param_4;
      *(code **)(piVar24 + 0x576) = _ZN3mpr11stillrecord9_write_dbEPv;
    }
    lVar20 = lVar20 + 1;
    puVar21 = puVar21 + 1;
    piVar11 = piVar11 + 1;
  } while (lVar20 != 4);
  uVar19 = 0;
  local_508 = (ulong *)0x3db142120;
  uVar8 = osal_thread_id();
  local_500 = CONCAT44(local_500._4_4_,uVar8);
  FUN_001df8d4(&local_508,0xc);
  local_5cc = 0;
  local_5d8 = (char *)(param_1 + 0x23c);
  local_598 = param_1;
  do {
    uVar23 = 0;
    bVar7 = true;
    iVar18 = (int)uVar19;
    local_5c8 = local_598;
    __src = local_5d8;
    local_5b0 = (int *)(param_1 + 500);
    do {
      iVar9 = (int)uVar23;
      if ((local_5b0[6] != -1) && (*__src != '\0')) {
        iVar3 = *local_5b0;
        local_508 = (ulong *)0x1db152120;
        uVar8 = osal_thread_id();
        local_500 = CONCAT44(iVar18,uVar8);
        local_4f8 = uVar23 & 0xffffffff;
        FUN_001df8d4(&local_508,0x18);
        FUN_00195048(0x2120,0xffffdb1c,__src);
        if ((param_2 == iVar9 & *(byte *)((long)local_5b0 + uVar19 + 0x44)) == 0) {
          if ((bVar7) && (*(int *)(local_5c8 + 0x228) != 0)) {
            _ZN3mpr2fs10MkMultiDirEPKcjbb(__src,*(int *)(local_5c8 + 0x228),1,0);
          }
          memset(&local_508,0,0x508);
          strncpy((char *)&local_508,__src,0x7f);
          local_f4 = 0;
          local_f8 = 0x206;
          if (0x7bb < *(short *)(param_1 + 2)) {
            local_f4 = *(byte *)(param_1 + 5) & 0x1f | (*(short *)(param_1 + 2) + -0x7bc) * 0x200 |
                       ((short)*(char *)(param_1 + 4) & 0xfU) << 5;
          }
          cVar6 = *(char *)(param_1 + 8);
          iVar22 = 0;
          uVar2 = -((short)cVar6 & 1U);
          if (-1 < cVar6) {
            uVar2 = (short)cVar6 & 1U;
          }
          local_f2 = (short)*(char *)(param_1 + 6) << 0xb | (ushort)((ulong)(long)cVar6 >> 1) & 0x1f
                     | ((short)*(char *)(param_1 + 7) & 0x3fU) << 5;
          local_e8 = *(short *)(param_1 + 10) / 10 + uVar2 * 100;
          local_ec = 0xffff;
          local_ea = 0xffff;
          local_e6 = 0xffff;
          local_e2 = 0xffff;
          local_e0 = 0xffff;
          uVar4 = *(uint *)(param_3 + 0x11f0);
          bVar7 = false;
          uVar14 = local_18;
          for (piVar11 = (int *)(param_3 + 0x1200);
              (int *)(param_3 + (ulong)uVar4 * 0x30 + 0x1200) != piVar11; piVar11 = piVar11 + 0xc) {
            iVar5 = *piVar11;
            if (iVar5 != 0) {
              uVar12 = (ulong)uVar14;
              iVar22 = iVar22 + iVar5;
              uVar14 = uVar14 + 1;
              auStack_d8[uVar12 * 3] = *(ulong *)(piVar11 + 4);
              auStack_d8[uVar12 * 3 + 1] = *(ulong *)(piVar11 + 2);
              auStack_d8[uVar12 * 3 + 2] = (ulong)(iVar5 + 0xfff) & 0xfffff000;
              bVar7 = true;
            }
          }
          if (bVar7) {
            local_18 = uVar14;
          }
          local_10 = _ZN3mpr11stillrecord17fsys_callbackfuncEPviPK22FSYSWRITEDIRECTFILERES;
          local_f0 = local_f4;
          local_ee = local_f2;
          local_e4 = local_e8;
          local_dc = iVar22;
          __dest = malloc(0x1680);
          local_8 = __dest;
          if (param_2 == iVar9) {
            if (iVar3 == 1) {
              uVar13 = 0x4c;
            }
            else if (iVar3 == 2) {
              uVar13 = 0x54;
            }
            else {
              if (iVar3 != 4) goto LAB_001f2030;
              uVar13 = 0x50;
            }
            _ZN4sidb13PartialUpdateIjEEijjNS_6COL_IDET_bb
                      (0xffffffff,*(undefined4 *)(local_598 + 0x130),uVar13,iVar22,1,0);
          }
LAB_001f2030:
          local_55c = 3;
          local_560 = 0x2120;
          if (local_590[uVar19] == iVar9) {
            local_55e = 0xdb1e;
            local_558 = osal_thread_id();
            FUN_001df8d4(&local_560,0xc);
            memcpy(__dest,(&local_580)[uVar19],0x15e0);
            *(undefined1 *)((long)__dest + 0x15e4) = 1;
          }
          else {
            local_55e = 0xdb1f;
            local_558 = osal_thread_id();
            FUN_001df8d4(&local_560,0xc);
            *(undefined1 *)((long)__dest + 0x15e4) = 0;
          }
          *(undefined4 *)((long)__dest + 0x15e8) = *(undefined4 *)(local_598 + 0x15c);
          *(int *)((long)__dest + 0x15e0) = iVar18;
          *(undefined4 *)((long)__dest + 0x15ec) = *(undefined4 *)(param_1 + uVar23 * 4 + 0x1ec);
          strncpy((char *)((long)__dest + 0x15f0),__src,0x7f);
          *(long *)((long)__dest + 0x1670) = param_1;
          *(int *)((long)__dest + 0x1678) = param_2;
          if (param_2 == iVar9) {
            iVar9 = Fsys_WriteDirectFileAsyncV2(&local_508);
            local_5cc = local_5cc + 1;
            local_560 = 0x2120;
            local_55c = 2;
            local_55e = 0xdbff;
            local_558 = osal_thread_id();
            local_554 = iVar9;
            FUN_001df8d4(&local_560,0x10);
            if (iVar9 != 0) {
              local_560 = 0x2120;
              iVar3 = local_590[uVar19];
              local_55e = 0x5020;
              local_55c = 1;
              local_54c = CONCAT44(local_54c._4_4_,param_2);
              local_558 = iVar9;
              local_554 = iVar18;
              local_550 = iVar3;
              FUN_001df8d4(&local_560,0x18);
              pvVar25 = (&local_580)[uVar19];
              *(int *)((long)pvVar25 + 0x14) = iVar9;
              if (iVar3 == param_2) {
                _ZN3mpr27evtStillWriteCountdownParamC2Ev(&local_560);
                local_558 = (&DAT_00474d2c)[uVar19 * 0x20];
                local_554 = *(int *)((long)pvVar25 + 0x10);
                local_550 = *(int *)((long)pvVar25 + 0x20);
                local_54c = *(ulong *)((long)pvVar25 + 0x24);
                uStack_544 = *(undefined8 *)((long)pvVar25 + 0x2c);
                local_53c = *(undefined8 *)((long)pvVar25 + 0x34);
                uStack_534 = *(undefined8 *)((long)pvVar25 + 0x3c);
                local_52c = *(undefined8 *)((long)pvVar25 + 0x44);
                uStack_524 = *(undefined8 *)((long)pvVar25 + 0x4c);
                local_51c = *(undefined8 *)((long)pvVar25 + 0x54);
                uStack_514 = *(undefined8 *)((long)pvVar25 + 0x5c);
                local_50c = *(undefined4 *)((long)pvVar25 + 0x14);
                local_55c = *(undefined4 *)((long)pvVar25 + 0x15d0);
                _ZN3mpr11stillrecord30SetEvtStillWriteCountdownParamEPNS_18recordStillParam_tEjNS_5EVTIDERNS_27evtStillWriteCountdownParamEm
                          (param_1,uVar19 & 0xffffffff,0x100d,&local_560,0x58);
                _ZN3mpr11stillrecord15SetWriteDbParamEPNS_18recordStillParam_tEjRN9filequeue6exec_tE
                          (param_1,uVar19 & 0xffffffff,pvVar25);
              }
              goto LAB_001f2234;
            }
          }
          else {
LAB_001f2234:
            free(__dest);
          }
          bVar7 = false;
        }
        else {
          local_508 = &local_4f8;
          sVar10 = strlen(__src);
          FUN_001df468(&local_508,__src,__src + sVar10);
          _ZN3mpr29ClearStillCacheForPseudoStillEj(uVar23 & 0xffffffff);
          _ZN3mpr27SetStillCacheForPseudoStillEjRNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
                    (uVar23 & 0xffffffff,&local_508);
          if (local_590[uVar19] == iVar9) {
            _ZN3mpr11stillrecord15SetWriteDbParamEPNS_18recordStillParam_tEjRN9filequeue6exec_tE
                      (param_1,uVar19 & 0xffffffff,(&local_580)[uVar19]);
          }
          _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv(&local_508);
        }
      }
      uVar23 = uVar23 + 1;
      __src = __src + 0x588;
      local_5b0 = local_5b0 + 0x162;
      local_5c8 = local_5c8 + 0x588;
    } while (uVar23 != 2);
    uVar19 = uVar19 + 1;
    local_5d8 = local_5d8 + 0x106;
    local_598 = local_598 + 4;
    if (uVar19 == 4) {
      _ZN3mpr11stillrecord24SetFsysWriteRequestCountEPNS_18recordStillParam_tEjj
                (param_1,param_2,local_5cc);
      free(__ptr);
      local_508 = (ulong *)0x150302120;
      uVar8 = osal_thread_id();
      local_500 = CONCAT44((int)param_1,uVar8);
      local_4f8 = CONCAT44((int)param_3,param_2);
      FUN_001df8d4(&local_508,0x18);
      return 0;
    }
  } while( true );
}


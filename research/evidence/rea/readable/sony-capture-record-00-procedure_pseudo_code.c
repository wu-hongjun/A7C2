// Decompiled pseudocode, not original source.
// REA Evidence: ev_4df7acb937603845a260f23974024ab82c15e4a1c4246c036b396f86eb2f1095
// Source record: research/evidence/rea/sony-capture-record/00-procedure_pseudo_code.json

undefined4
_ZN3mpr11RecordStillERKNS_18recordStillInParamEPNS_19recordStillOutParamE
          (undefined8 *param_1,long param_2)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 uVar6;
  long lVar7;
  long lVar8;
  ulong uVar9;
  undefined4 *puVar10;
  void *__src;
  char *pcVar11;
  undefined4 *puVar12;
  ulong uVar13;
  undefined8 local_688;
  undefined8 local_680;
  undefined8 local_678;
  undefined8 local_670;
  undefined8 local_668;
  undefined8 local_660;
  undefined2 local_658 [2];
  undefined1 uStack_654;
  undefined1 uStack_653;
  undefined1 uStack_652;
  undefined1 uStack_651;
  undefined8 local_650 [6];
  undefined4 local_620;
  undefined8 local_61c;
  undefined8 uStack_614;
  undefined8 local_60c;
  undefined8 uStack_604;
  undefined4 local_5fc;
  undefined4 local_5f8;
  undefined1 auStack_5f4 [384];
  undefined1 auStack_474 [672];
  undefined1 auStack_1d4 [452];
  long local_10;
  code *local_8;

  FUN_001df8d4(param_1,0x150);
  _ZNK3mpr18recordStillInParam8DbgPrintEv(param_1);
  pthread_mutex_lock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
  lVar7 = _ZStmiIPN3mpr18recordStillParam_tERS2_PS2_ENSt15_Deque_iteratorIT_T0_T1_E15difference_typeERKS9_SC_
                    (0x4817e8,0x4817c8);
  if (lVar7 == 0) {
    uStack_653 = 0;
    lVar7 = 0;
    uStack_654 = 4;
    uStack_652 = 0;
    local_658[0] = 0x2120;
    uStack_651 = 0;
    local_658[1] = 0x3010;
    FUN_001df8d4(local_658,8);
  }
  else {
    _ZNSt15_Deque_iteratorIPN3mpr18recordStillParam_tERS2_PS2_EC1ERKS5_(local_658,0x4817c8);
    lVar7 = *(long *)CONCAT17(uStack_651,
                              CONCAT16(uStack_652,
                                       CONCAT15(uStack_653,
                                                CONCAT14(uStack_654,
                                                         CONCAT22(local_658[1],local_658[0])))));
  }
  iVar5 = _ZN3mpr11stillrecord12_check_errorEPNS_18recordStillParam_tERKNS_18recordStillInParamE
                    (lVar7,param_1);
  if (iVar5 == 0) {
    if (DAT_004b33a8 != '\0') {
      _ZN9stillutil5dummy16export_stillinfoERKN3mpr18recordStillInParamEPKNS1_18recordStillParam_tE
                (param_1,lVar7);
    }
    if (*(int *)(lVar7 + 0xc) == 1) {
      local_8 = _ZN3mpr11stillrecord16_record_trimmingEPv;
      local_658[0] = (undefined2)*(undefined4 *)((long)param_1 + 4);
      local_658[1] = (undefined2)((uint)*(undefined4 *)((long)param_1 + 4) >> 0x10);
      uVar3 = *(uint *)((long)param_1 + 0x44);
      uStack_654 = (undefined1)uVar3;
      uStack_653 = (undefined1)(uVar3 >> 8);
      uStack_652 = (undefined1)(uVar3 >> 0x10);
      uStack_651 = (undefined1)(uVar3 >> 0x18);
      if (uVar3 == 0) {
        local_668 = 0x430302120;
        FUN_001df8d4(&local_668,8);
        goto LAB_001eb364;
      }
      lVar8 = 0;
      while (lVar8 + 0x10 != (ulong)uVar3 * 0x10 + 0x10) {
        puVar1 = (undefined8 *)((long)param_1 + lVar8 + 0x48);
        uVar2 = puVar1[1];
        *(undefined8 *)((long)local_658 + lVar8 + 8) = *puVar1;
        *(undefined8 *)((long)local_650 + lVar8 + 8) = uVar2;
        lVar8 = lVar8 + 0x10;
      }
      local_620 = *(undefined4 *)(param_1 + 0xf);
      uVar13 = 0;
      local_61c = *(undefined8 *)((long)param_1 + 0x8c);
      uStack_614 = *(undefined8 *)((long)param_1 + 0x94);
      local_60c = *(undefined8 *)((long)param_1 + 0x9c);
      uStack_604 = *(undefined8 *)((long)param_1 + 0xa4);
      local_5fc = *(undefined4 *)((long)param_1 + 0xac);
      local_5f8 = *(undefined4 *)(param_1 + 0x16);
      do {
        __src = (void *)param_1[1];
        if (__src == (void *)0x0) {
          memset(auStack_5f4 + (uVar13 & 0xffffffff) * 0xc0,0,0xc0);
        }
        else {
          memcpy(auStack_5f4 + uVar13 * 0xc0,__src,0xc0);
        }
        if ((void *)param_1[5] == (void *)0x0) {
          memset(auStack_1d4 + (uVar13 & 0xffffffff) * 0xe0,0,0xe0);
        }
        else {
          memcpy(auStack_1d4 + uVar13 * 0xe0,(void *)param_1[5],0xe0);
        }
        uVar9 = uVar13 & 0xffffffff;
        uVar13 = uVar13 + 1;
        memset(auStack_474 + uVar9 * 0x150,0,0x150);
        param_1 = param_1 + 1;
      } while (uVar13 != 2);
      local_10 = lVar7;
      _ZN9execqueueIN3mpr11stillrecord19make_header_input_tEE4execERNS3_6exec_tE
                (_ZN3mpr11stillrecord18make_header_threadE,local_658);
      uVar6 = 0;
    }
    else {
      uVar6 = _ZN3mpr11stillrecord13_record_stillERKNS_18recordStillInParamEPNS_18recordStillParam_tE
                        (param_1,lVar7);
    }
    pcVar11 = (char *)(lVar7 + 0x23c);
    puVar12 = (undefined4 *)(lVar7 + 0x130);
    iVar5 = *(int *)(lVar7 + 0x20c);
    *(undefined4 *)(param_2 + 4) = 0;
    puVar10 = &DAT_00474d2c;
    do {
      if (((iVar5 != -1) && (*pcVar11 != '\0')) ||
         ((*(int *)(lVar7 + 0x794) != -1 && (pcVar11[0x588] != '\0')))) {
        uVar3 = *(uint *)(param_2 + 4);
        uVar4 = *puVar12;
        lVar8 = param_2 + (ulong)uVar3 * 8;
        *(undefined4 *)(lVar8 + 8) = *puVar10;
        *(undefined4 *)(lVar8 + 0xc) = uVar4;
        *(uint *)(param_2 + 4) = uVar3 + 1;
      }
      pcVar11 = pcVar11 + 0x106;
      puVar10 = puVar10 + 0x20;
      puVar12 = puVar12 + 1;
    } while ((char *)(lVar7 + 0x654) != pcVar11);
    _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE9pop_frontEv(_ZN3mpr4fifoE);
    pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
    _ZNK3mpr19recordStillOutParam8DbgPrintEv(param_2);
    FUN_001df8d4(param_2,0x28);
  }
  else {
LAB_001eb364:
    local_688 = param_1[1];
    local_678 = param_1[3];
    local_668 = param_1[5];
    local_680 = param_1[2];
    local_670 = param_1[4];
    local_660 = param_1[6];
    memset(local_658,0,0x540);
    _ZN3mpr11stillrecord5_freeERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tEb
              (param_1,lVar7,&local_688,&local_678,&local_668,local_658,0);
    if (param_2 != 0) {
      *(undefined4 *)(param_2 + 4) = 0;
    }
    uVar6 = 3;
    pthread_mutex_unlock((pthread_mutex_t *)_ZN3mpr15g_integApiMutexE);
    _ZNK3mpr19recordStillOutParam8DbgPrintEv(param_2);
    FUN_001df8d4(param_2,0x28);
    _ZNSt5dequeIPN3mpr18recordStillParam_tESaIS2_EE9pop_frontEv(_ZN3mpr4fifoE);
  }
  return uVar6;
}

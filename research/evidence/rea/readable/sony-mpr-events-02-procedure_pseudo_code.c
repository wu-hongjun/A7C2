// Decompiled pseudocode, not original source.
// REA Evidence: ev_5c0d700af2a5d89aa8d9d7050ce9e4e12fc5d77e3aa411926ce790f579068df4
// Source record: research/evidence/rea/sony-mpr-events/02-procedure_pseudo_code.json

undefined8
_ZN3mpr13SetEventParamEjNS_5EVTIDEPvm(int param_1,int param_2,void *param_3,size_t param_4)

{
  ulong uVar1;
  long *__dest;
  long lVar2;
  long lVar3;
  size_t sVar4;
  long *plVar5;
  long lVar6;
  void *__dest_00;

  __dest_00 = (void *)0x0;
  if (param_1 == 1) {
    __dest_00 = malloc(param_4);
    memcpy(__dest_00,param_3,param_4);
  }
  pthread_mutex_lock((pthread_mutex_t *)&DAT_00475790);
  if (DAT_00475d08 != DAT_00475d18 + -4) {
    if (DAT_00475d08 != (int *)0x0) {
      *DAT_00475d08 = param_1;
      DAT_00475d08[1] = param_2;
      *(void **)(DAT_00475d08 + 2) = __dest_00;
    }
    DAT_00475d08 = DAT_00475d08 + 4;
    goto LAB_001b4b5c;
  }
  if (DAT_00475ce0 - ((long)DAT_00475d20 - _ZN3mpr15s_evtParamQueueE >> 3) < 2) {
    lVar6 = (long)DAT_00475d20 - (long)DAT_00475d00 >> 3;
    lVar2 = lVar6 + 2;
    if ((ulong)(lVar2 * 2) < DAT_00475ce0) {
      sVar4 = (long)DAT_00475d20 + (8 - (long)DAT_00475d00);
      plVar5 = (long *)(_ZN3mpr15s_evtParamQueueE + (DAT_00475ce0 - lVar2 >> 1) * 8);
      if (plVar5 < DAT_00475d00) {
        __dest = plVar5;
        if ((long)sVar4 >> 3 != 0) {
LAB_001b4bf4:
          memmove(__dest,DAT_00475d00,sVar4);
        }
      }
      else if ((long)sVar4 >> 3 != 0) {
        __dest = (long *)((long)plVar5 + ((lVar6 + 1) * 8 - sVar4));
        goto LAB_001b4bf4;
      }
    }
    else {
      uVar1 = DAT_00475ce0;
      if (DAT_00475ce0 == 0) {
        uVar1 = 1;
      }
      uVar1 = DAT_00475ce0 + 2 + uVar1;
      if (0x1fffffffffffffff < uVar1) {
        _ZSt17__throw_bad_allocv();
      }
      lVar3 = _Znwm(uVar1 * 8);
      plVar5 = (long *)(lVar3 + (uVar1 - lVar2 >> 1) * 8);
      sVar4 = (long)DAT_00475d20 + (8 - (long)DAT_00475d00);
      if ((long)sVar4 >> 3 != 0) {
        memmove(plVar5,DAT_00475d00,sVar4);
      }
      _ZdlPv(_ZN3mpr15s_evtParamQueueE);
      _ZN3mpr15s_evtParamQueueE = lVar3;
      DAT_00475ce0 = uVar1;
    }
    DAT_00475cf0 = *plVar5;
    DAT_00475cf8 = DAT_00475cf0 + 0x200;
    DAT_00475d20 = plVar5 + lVar6;
    DAT_00475d10 = (int *)plVar5[lVar6];
    DAT_00475d18 = DAT_00475d10 + 0x80;
    DAT_00475d00 = plVar5;
  }
  plVar5 = DAT_00475d20;
  lVar2 = _Znwm(0x200);
  plVar5[1] = lVar2;
  if (DAT_00475d08 != (int *)0x0) {
    *DAT_00475d08 = param_1;
    DAT_00475d08[1] = param_2;
    *(void **)(DAT_00475d08 + 2) = __dest_00;
  }
  DAT_00475d08 = (int *)DAT_00475d20[1];
  DAT_00475d18 = DAT_00475d08 + 0x80;
  DAT_00475d10 = DAT_00475d08;
  DAT_00475d20 = DAT_00475d20 + 1;
LAB_001b4b5c:
  pthread_cond_signal((pthread_cond_t *)&DAT_004757c0);
  pthread_mutex_unlock((pthread_mutex_t *)&DAT_00475790);
  return 0;
}

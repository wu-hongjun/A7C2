// Decompiled pseudocode, not original source.
// REA Evidence: ev_99234143578af70a289a75454472ce5f3b1d445add8b905a8b6ce02824d81341
// Source record: research/evidence/rea/libmpr/07-procedure_pseudo_code.json

void _ZN3mpr12ResetLutDataEv(void)

{
  long *plVar1;
  
  if (_ZN3mpr14sp_lutDataAddrE != (void *)0x0) {
    free(_ZN3mpr14sp_lutDataAddrE);
    _ZN3mpr14sp_lutDataAddrE = (void *)0x0;
  }
  plVar1 = &DAT_00478c20;
  do {
    if (*plVar1 != 0) {
      FUN_00219e14();
    }
    plVar1 = plVar1 + 0x358;
  } while (plVar1 != (long *)0x47f720);
  DAT_00478aa0 = 0;
  DAT_00478aa8 = 0;
  DAT_00478ab8 = 0;
  DAT_00478ab0 = 0;
  DAT_00478aba = 0;
  DAT_00478ac2 = 0;
  DAT_00478aca = 0;
  DAT_00478ad2 = 0;
  DAT_00478ada = 0;
  DAT_00478ae2 = 0;
  DAT_00478aea = 0;
  DAT_00478af2 = 0;
  return;
}


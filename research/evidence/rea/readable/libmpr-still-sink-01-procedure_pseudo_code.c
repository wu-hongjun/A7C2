// Decompiled pseudocode, not original source.
// REA Evidence: ev_4d2dbc5a514a974884b72f2bbde26491e5a4547c6c13d12926d4889b5f16aed3
// Source record: research/evidence/rea/libmpr-still-sink/01-procedure_pseudo_code.json

undefined8 _ZN9stillutil7createrEN3mpr9StillTypeE(int param_1)

{
  undefined8 uVar1;
  
  if (param_1 == 1) {
    uVar1 = _Znwm(0x1558);
    FUN_0036d090();
  }
  else if (param_1 == 2) {
    uVar1 = _Znwm(0x14d8);
    FUN_00367478();
  }
  else {
    if (param_1 != 4) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("0",
                    "/home/jp10239/git-work/build/git_root/BuildWorkSpace/Mpr-Library_201H/userspace/app/still/stillutil.h"
                    ,0x6b,"libstill* stillutil::creater(mpr::StillType)");
    }
    uVar1 = _Znwm(0x1560);
    FUN_00369fb0();
  }
  return uVar1;
}


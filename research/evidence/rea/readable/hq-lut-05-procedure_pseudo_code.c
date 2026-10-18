// Decompiled pseudocode, not original source.
// REA Evidence: ev_0ff7de2c20f1f0daf4c082b1849d833d029ff82957361d1fd366db94059cb469
// Source record: research/evidence/rea/hq-lut/05-procedure_pseudo_code.json

void FUN_037f3d30(undefined8 param_1,undefined4 param_2,long param_3)

{
  long lVar1;
  long *plVar2;
  long *plVar3;
  
  lVar1 = _Znwm(0x5a0);
  *(undefined1 *)(lVar1 + 0xe8) = 0;
  *(undefined1 *)(lVar1 + 0xe9) = 0;
  *(undefined8 *)(lVar1 + 0xf8) = 0;
  *(undefined1 *)(lVar1 + 0x104) = 0;
  plVar3 = (long *)(lVar1 + 0x3a0);
  *(undefined1 *)(lVar1 + 0x105) = 0;
  *(undefined1 *)(lVar1 + 0x13b) = 0;
  *(undefined1 *)(lVar1 + 0x13c) = 0;
  *(undefined1 *)(lVar1 + 0x13d) = 0;
  *(undefined1 *)(lVar1 + 0x13e) = 0;
  *(undefined4 *)(lVar1 + 0xec) = 0;
  *(undefined4 *)(lVar1 + 0xf0) = 0;
  *(undefined8 *)(lVar1 + 0x140) = 0;
  *(undefined8 *)(lVar1 + 0x148) = 0;
  *(undefined8 *)(lVar1 + 0x150) = 0;
  *(undefined8 *)(lVar1 + 0x158) = 0;
  *(undefined8 *)(lVar1 + 0x160) = 0;
  *(undefined8 *)(lVar1 + 0x168) = 0;
  *(undefined8 *)(lVar1 + 0x170) = 0;
  *(undefined8 *)(lVar1 + 0x178) = 0;
  *(undefined8 *)(lVar1 + 0x180) = 0;
  *(undefined8 *)(lVar1 + 0x188) = 0;
  *(undefined8 *)(lVar1 + 400) = 0;
  *(undefined8 *)(lVar1 + 0x198) = 0;
  *(undefined8 *)(lVar1 + 0x1a0) = 0;
  *(undefined8 *)(lVar1 + 0x1a8) = 0;
  *(undefined8 *)(lVar1 + 0x1b0) = 0;
  plVar2 = (long *)(lVar1 + 0x280);
  *(undefined8 *)(lVar1 + 0x1b8) = 0;
  *(undefined8 *)(lVar1 + 0x1c0) = 0;
  *(undefined8 *)(lVar1 + 0x1c8) = 0;
  *(undefined8 *)(lVar1 + 0x1d0) = 0;
  *(undefined8 *)(lVar1 + 0x1d8) = 0;
  *(undefined8 *)(lVar1 + 0x1e0) = 0;
  *(undefined4 *)(lVar1 + 0x1e8) = 0;
  do {
    *(undefined1 *)(plVar2 + 2) = 0;
    *plVar2 = (long)(plVar2 + 2);
    plVar2[1] = 0;
    plVar2 = plVar2 + 4;
  } while (plVar2 != plVar3);
  do {
    *(undefined1 *)(plVar3 + 2) = 0;
    *plVar3 = (long)(plVar3 + 2);
    plVar3[1] = 0;
    plVar3 = plVar3 + 4;
  } while ((long *)(lVar1 + 0x4c0) != plVar3);
  FUN_037ef480((long *)(lVar1 + 0x4c0),&DAT_04d59fb2);
  FUN_037ef480(lVar1 + 0x4e0,&DAT_04d59fb2);
  *(undefined8 *)(lVar1 + 0x508) = 0;
  *(undefined4 *)(lVar1 + 0x500) = 0;
  *(undefined8 *)(lVar1 + 0x510) = 0;
  *(undefined8 *)(lVar1 + 0x518) = 0;
  *(undefined8 *)(lVar1 + 0x520) = 0;
  *(undefined8 *)(lVar1 + 0x528) = 0;
  *(undefined8 *)(lVar1 + 0x530) = 0;
  *(undefined8 *)(lVar1 + 0x538) = 0;
  *(undefined8 *)(lVar1 + 0x540) = 0;
  *(undefined8 *)(lVar1 + 0x548) = 0;
  *(undefined8 *)(lVar1 + 0x550) = 0;
  *(undefined8 *)(lVar1 + 0x558) = 0;
  *(undefined8 *)(lVar1 + 0x560) = 0;
  *(undefined8 *)(lVar1 + 0x568) = 0;
  *(undefined8 *)(lVar1 + 0x570) = 0;
  *(undefined8 *)(lVar1 + 0x578) = 0;
  *(undefined8 *)(lVar1 + 0x580) = 0;
  *(undefined8 *)(lVar1 + 0x588) = 0;
  *(undefined8 *)(lVar1 + 0x590) = 0;
  *(undefined8 *)(lVar1 + 0x598) = 0;
  FUN_037f3924(param_1,lVar1,param_3 + 0x750,param_3 + 0x768,param_3 + 0x780,param_3 + 0x798,
               param_3 + 0x7b0,0x14);
  FUN_037f2c58(param_1,param_2,param_3 + 0x720);
  FUN_037f2cf8(param_1,lVar1,0x14,param_3 + 0x738);
  FUN_037f057c(lVar1);
  _ZdlPvm(lVar1,0x5a0);
  return;
}


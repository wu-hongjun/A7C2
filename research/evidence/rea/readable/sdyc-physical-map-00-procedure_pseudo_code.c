// Decompiled pseudocode, not original source.
// REA Evidence: ev_678ca8995805aaec36cf6b6b872738ba0f8716c3fdcb10a5eb3f551caddf936d
// Source record: research/evidence/rea/sdyc-physical-map/00-procedure_pseudo_code.json

void _ZN12AvbbcPhycMemC2EmjNS_10CACHE_MODEEj
               (undefined8 *param_1,ulong param_2,int param_3,int param_4,undefined4 param_5)

{
  ulong uVar1;
  int iVar2;
  long lVar3;
  ulong uVar4;
  int *piVar5;
  void *pvVar6;
  undefined8 uVar7;
  char *pcVar8;
  
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  *(undefined4 *)(param_1 + 5) = 0;
  *(undefined4 *)((long)param_1 + 0x2c) = 0;
  *param_1 = 0x2b2a70;
  *(undefined4 *)(param_1 + 6) = param_5;
  *(undefined4 *)(param_1 + 1) = 0xffffffff;
  lVar3 = sysconf(0x1e);
  uVar4 = (ulong)(int)lVar3;
  *(int *)((long)param_1 + 0x2c) = param_3;
  iVar2 = 2;
  param_1[4] = param_2;
  uVar1 = 0;
  if (uVar4 != 0) {
    uVar1 = param_2 / uVar4;
  }
  param_2 = param_2 - uVar1 * uVar4;
  *(int *)(param_1 + 5) = param_3 + (int)param_2;
  if (param_4 != 0) {
    iVar2 = 0x101002;
  }
  iVar2 = open("/dev/mem",iVar2);
  *(int *)(param_1 + 1) = iVar2;
  iVar2 = *(int *)(param_1 + 1);
  if (iVar2 < 0) {
    piVar5 = __errno_location();
    _ZN17AvbbcBaseClassLog7SetLog0Ejjjjt(0x201e0a6,*(undefined4 *)(param_1 + 6),*piVar5,iVar2,0x3c);
    if (0 < avbizPrintLevel) {
      uVar7 = 0x3d;
      pcVar8 = "[AvbbcPhycMem]Open /dev/mem Error!!";
LAB_00240280:
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz(0x55,6,3,uVar7,pcVar8);
      return;
    }
  }
  else {
    param_2 = param_2 & 0xffffffff;
    pvVar6 = mmap((void *)0x0,(ulong)*(uint *)(param_1 + 5),3,1,iVar2,param_1[4] - param_2);
    param_1[3] = pvVar6;
    if (pvVar6 == (void *)0xffffffffffffffff) {
      piVar5 = __errno_location();
      _ZN17AvbbcBaseClassLog7SetLog0Ejjjjt
                (0x201e0a7,*(undefined4 *)(param_1 + 6),*piVar5,*(undefined4 *)(param_1 + 1),0x47);
      if (0 < avbizPrintLevel) {
        uVar7 = 0x48;
        pcVar8 = "[AvbbcPhycMem]Memory Mapping Error!!";
        goto LAB_00240280;
      }
    }
    else {
      param_1[2] = (long)pvVar6 + param_2;
    }
  }
  return;
}


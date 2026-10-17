// Decompiled pseudocode, not original source.
// REA Evidence: ev_f9d2b24b358a3220d5f0981bedf702aa24c281ef716f8ee75172b57714588bfa
// Source record: research/evidence/rea/im/02-procedure_pseudo_code.json

undefined8 FUN_00405f5c(long param_1,long *param_2)

{
  uint uVar1;
  char *pcVar2;
  long lVar3;
  code *pcVar4;
  long lVar5;
  undefined8 uVar6;
  
  pcVar2 = "/usr/lib/libObj.so";
  if (*(short *)(param_1 + 0xe) == 0) {
    pcVar2 = *(char **)(param_1 + 0x10);
  }
  lVar3 = dlopen(pcVar2,DAT_0041973c);
  if (lVar3 == 0) {
    if ((DAT_00419738 >> 0xe & 1) != 0) {
      return 0xffffffff;
    }
    dlerror();
    return 0xffffffff;
  }
  if ((*(char **)(param_1 + 0x18) != (char *)0x0) && (**(char **)(param_1 + 0x18) != '\0')) {
    uVar1 = DAT_00419738 & 0x4000;
    if ((DAT_00419738 >> 0xe & 1) == 0) {
      dlerror();
    }
    pcVar4 = (code *)dlsym(lVar3,*(undefined8 *)(param_1 + 0x18));
    if ((uVar1 == 0) && (lVar5 = dlerror(), lVar5 != 0)) {
      if (*(short *)(param_1 + 0xe) == 0) {
        dlclose(lVar3);
      }
      uVar6 = 0xfffffffe;
      goto LAB_00405fdc;
    }
    if (pcVar4 != (code *)0x0) {
      (*pcVar4)();
    }
  }
  uVar6 = 0;
LAB_00405fdc:
  *param_2 = lVar3;
  return uVar6;
}


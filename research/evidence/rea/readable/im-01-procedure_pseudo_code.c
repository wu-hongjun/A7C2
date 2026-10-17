// Decompiled pseudocode, not original source.
// REA Evidence: ev_0cb75d1f8999a3c48a9aae44586146a220531911b120f269d2d5eb9e74d74762
// Source record: research/evidence/rea/im/01-procedure_pseudo_code.json

undefined8 FUN_00405e24(long param_1,long *param_2)

{
  uint uVar1;
  undefined8 *puVar2;
  int iVar3;
  long lVar4;
  code *pcVar5;
  long lVar6;
  undefined8 uVar7;
  undefined1 auStack_c [4];
  undefined4 *local_8;
  
  uVar7 = *(undefined8 *)(param_1 + 0x10);
  if (((DAT_00419738 >> 0xd & 1) != 0) &&
     (iVar3 = osal_valloc_msg_wait(0x80920021,&local_8,0x18,1), iVar3 == 0)) {
    *local_8 = 0x922108;
    local_8[1] = 0;
    *(undefined1 **)(local_8 + 2) = auStack_c;
    local_8[5] = 0;
    osal_snd_msg(0x80920021);
  }
  if (*(short *)(param_1 + 0xe) != 0) {
    uVar7 = 0;
  }
  lVar4 = dlopen(uVar7,DAT_0041973c);
  if (lVar4 == 0) {
    uVar7 = 0xffffffff;
    if ((DAT_00419738 >> 0xe & 1) == 0) {
      dlerror();
    }
  }
  else {
    uVar7 = 0;
    if ((*(char **)(param_1 + 0x18) != (char *)0x0) && (**(char **)(param_1 + 0x18) != '\0')) {
      uVar1 = DAT_00419738 & 0x4000;
      if ((DAT_00419738 >> 0xe & 1) == 0) {
        dlerror();
      }
      pcVar5 = (code *)dlsym(lVar4,*(undefined8 *)(param_1 + 0x18));
      if ((uVar1 == 0) && (lVar6 = dlerror(), lVar6 != 0)) {
        uVar7 = 0xfffffffe;
        dlclose(lVar4);
      }
      else {
        puVar2 = (undefined8 *)(param_1 + 0x38);
        if (uVar1 == 0) {
          puVar2 = (undefined8 *)(param_1 + 0x40);
        }
        if (pcVar5 != (code *)0x0) {
          (*pcVar5)(*puVar2);
        }
        uVar7 = 0;
        DAT_0041974c = 1;
      }
    }
    *param_2 = lVar4;
  }
  return uVar7;
}


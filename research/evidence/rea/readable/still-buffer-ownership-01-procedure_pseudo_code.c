// Decompiled pseudocode, not original source.
// REA Evidence: ev_75574657a0ff8ef125e4c8c07d1ceaa3ad4dad738878881e8abf65e772faedcd
// Source record: research/evidence/rea/still-buffer-ownership/01-procedure_pseudo_code.json

undefined8
_ZN3mpr11stillrecord5_freeERKNS_18recordStillInParamEPNS_18recordStillParam_tERA2_PN9DefStruct7JpgInfoERA2_PNS6_7RawInfoERA2_PNS6_7HifInfoERA2_N5still8common_tEb
          (long param_1,long param_2,long param_3,long param_4,long param_5,int *param_6,
          char param_7)

{
  undefined4 uVar1;
  int iVar2;
  long lVar3;
  long lVar4;
  int *piVar5;
  uint uVar6;
  undefined1 auStack_100 [256];
  
  lVar4 = 0;
  piVar5 = (int *)(param_2 + 500);
  uVar1 = 0;
  memset(auStack_100,0,0x100);
  do {
    if (*(long *)(param_3 + lVar4) != 0) {
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE(auStack_100,uVar1);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_3 + lVar4) + 0xa0);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_3 + lVar4) + 0xb0);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_3 + lVar4) + 0x90);
    }
    if (*(long *)(param_4 + lVar4) != 0) {
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE(auStack_100,uVar1);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_4 + lVar4) + 0x90);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_4 + lVar4) + 0x110);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_4 + lVar4) + 0x130);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_4 + lVar4) + 0xa0);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_4 + lVar4) + 0x120);
    }
    if (*(long *)(param_5 + lVar4) != 0) {
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE(auStack_100,uVar1);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_5 + lVar4) + 0xa0);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_5 + lVar4) + 0xb0);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_5 + lVar4) + 0x90);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_5 + lVar4) + 0xc0);
      uVar1 = _ZN3mpr11stillrecord14_free_areaInfoEPjjPN9DefStruct8AreaInfoE
                        (auStack_100,uVar1,*(long *)(param_5 + lVar4) + 0xd0);
    }
    if (param_2 == 0) {
      if (*param_6 != 0) {
        FUN_003b24f0();
      }
      if (param_6[0x54] != 0) {
        FUN_003b24f0();
      }
      if (param_6[0x60] != 0) {
        FUN_003b24f0();
      }
      if ((*(long *)(param_3 + lVar4) != 0) && (*(int *)(*(long *)(param_3 + lVar4) + 0x80) != 0)) {
        FUN_003b24f0();
      }
LAB_001e3e78:
      lVar3 = *(long *)(param_5 + lVar4);
LAB_001e3e64:
      if (lVar3 != 0) {
        iVar2 = *(int *)(lVar3 + 0x80);
        goto LAB_001e3dd8;
      }
    }
    else if (param_7 == '\0') {
      if (*(int *)(param_2 + 0xc) == 2) {
        if (*piVar5 == 1) {
          lVar3 = *(long *)(param_3 + lVar4);
          goto LAB_001e3e64;
        }
        if (*piVar5 == 4) goto LAB_001e3e78;
      }
    }
    else {
      if (*piVar5 == 1) {
        iVar2 = *param_6;
      }
      else {
        if (*piVar5 != 4) goto LAB_001e3de0;
        if (*param_6 != 0) {
          FUN_003b24f0();
        }
        if (param_6[0x54] != 0) {
          FUN_003b24f0();
        }
        iVar2 = param_6[0x60];
      }
LAB_001e3dd8:
      if (iVar2 != 0) {
        FUN_003b24f0();
      }
    }
LAB_001e3de0:
    lVar4 = lVar4 + 8;
    param_6 = param_6 + 0xa8;
    piVar5 = piVar5 + 0x162;
    if (lVar4 == 0x10) {
      piVar5 = (int *)(param_1 + 0x48);
      for (uVar6 = 0; uVar6 < *(uint *)(param_1 + 0x44); uVar6 = uVar6 + 1) {
        if (*piVar5 != 0) {
          FUN_003b24e4();
        }
        piVar5 = piVar5 + 4;
      }
      return 0;
    }
  } while( true );
}


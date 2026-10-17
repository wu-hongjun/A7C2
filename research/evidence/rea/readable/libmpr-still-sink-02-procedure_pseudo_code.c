// Decompiled pseudocode, not original source.
// REA Evidence: ev_3bb6d43c041745510f2a2e0c4f5502fa9c12757da49e8c30623270f8a6267487
// Source record: research/evidence/rea/libmpr-still-sink/02-procedure_pseudo_code.json

undefined8 FUN_00370b2c(long *param_1,undefined8 *param_2)

{
  long lVar1;
  char cVar2;
  short sVar3;
  uint uVar4;
  undefined4 uVar5;
  long *plVar6;
  void *pvVar7;
  undefined8 uVar8;
  
  FUN_00370aec();
  if (*(uint *)((long)param_1 + 0x13e4) < 2) {
    memcpy(param_1 + 1,param_2,0x870);
    cVar2 = *(char *)(param_2 + 0x7a);
    param_1[0x7c] = (long)param_1 + 0xff4;
    if ((cVar2 == '\0') || ((char *)param_2[0x7b] == (char *)0x0)) {
      *(undefined1 *)((long)param_1 + 0xff4) = 0;
    }
    else {
      strncpy((char *)((long)param_1 + 0xff4),(char *)param_2[0x7b],0x3f);
    }
    cVar2 = *(char *)(param_2 + 0x7c);
    param_1[0x7e] = (long)param_1 + 0x1034;
    if ((cVar2 == '\0') || ((char *)param_2[0x7d] == (char *)0x0)) {
      *(undefined1 *)((long)param_1 + 0x1034) = 0;
    }
    else {
      strncpy((char *)((long)param_1 + 0x1034),(char *)param_2[0x7d],0x3f);
    }
    param_1[0x7f] = (long)param_1 + 0x1074;
    if ((char *)param_2[0x7e] == (char *)0x0) {
      *(undefined1 *)((long)param_1 + 0x1074) = 0;
    }
    else {
      strncpy((char *)((long)param_1 + 0x1074),(char *)param_2[0x7e],0x3f);
    }
    if ((undefined2 *)*param_2 == (undefined2 *)0x0) {
      memset((void *)((long)param_1 + 0x10b4),0,0xbc);
    }
    else {
      uVar5 = FUN_003733c8(*(undefined2 *)*param_2);
      pvVar7 = (void *)*param_2;
      *(undefined4 *)(param_1 + 0x17f) = uVar5;
      memcpy((void *)((long)param_1 + 0x10b4),pvVar7,0xbc);
    }
    if ((void *)param_2[0x8a] == (void *)0x0) {
      memset(param_1 + 399,0,0x90);
    }
    else {
      pvVar7 = memcpy(param_1 + 399,(void *)param_2[0x8a],0x90);
      param_1[0x8b] = (long)pvVar7;
    }
    if ((void *)param_2[0x17] == (void *)0x0) {
      param_1[0x22e] = 0;
      param_1[0x22f] = 0;
      param_1[0x230] = 0;
      param_1[0x231] = 0;
      param_1[0x232] = 0;
      param_1[0x233] = 0;
      param_1[0x234] = 0;
      param_1[0x235] = 0;
      param_1[0x236] = 0;
      param_1[0x237] = 0;
      param_1[0x238] = 0;
      param_1[0x239] = 0;
      param_1[0x23a] = 0;
    }
    else {
      memcpy(param_1 + 0x22e,(void *)param_2[0x17],0x68);
    }
    FUN_003704e0(param_1,param_2,param_1 + 0x1a1);
    uVar4 = (int)param_1[0xbe] - 1;
    if (uVar4 < 8) {
      plVar6 = param_1 + 0xc0;
      do {
        sVar3 = *(short *)((long)plVar6 + -0xc);
        if (sVar3 == 0x203a) {
          lVar1 = plVar6[1];
          *(long *)((long)param_1 + 0xbfc) = *plVar6;
          *(long *)((long)param_1 + 0xc04) = lVar1;
          lVar1 = plVar6[3];
          *(long *)((long)param_1 + 0xc0c) = plVar6[2];
          *(long *)((long)param_1 + 0xc14) = lVar1;
        }
        else if (sVar3 == 0x203c) {
          lVar1 = plVar6[1];
          *(long *)((long)param_1 + 0xc1c) = *plVar6;
          *(long *)((long)param_1 + 0xc24) = lVar1;
        }
        else if (sVar3 == 0x2049) {
          *(undefined1 *)((long)param_1 + 0xfca) = 1;
          lVar1 = plVar6[1];
          *(long *)((long)param_1 + 0xfcc) = *plVar6;
          *(long *)((long)param_1 + 0xfd4) = lVar1;
        }
        plVar6 = (long *)((long)plVar6 + 0x2c);
      } while ((long *)((long)param_1 + (ulong)uVar4 * 0x2c + 0x62c) != plVar6);
    }
    (**(code **)(*param_1 + 0x68))(param_1,param_2,2,(long)param_1 + 0xc2c);
    (**(code **)(*param_1 + 0x68))(param_1,param_2,1,(long)param_1 + 0xc3c);
    (**(code **)(*param_1 + 0x68))(param_1,param_2,0,(long)param_1 + 0xc4c);
    uVar8 = 0;
    (**(code **)(*param_1 + 0x68))(param_1,param_2,3,(long)param_1 + 0xc5c);
    *(undefined4 *)((long)param_1 + 0x13e4) = 1;
  }
  else {
    uVar8 = 0xffffffff;
  }
  FUN_00370b0c(param_1);
  return uVar8;
}


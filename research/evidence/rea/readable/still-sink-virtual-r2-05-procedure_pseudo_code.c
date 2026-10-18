// Decompiled pseudocode, not original source.
// REA Evidence: ev_8dcec464385e3426507da1382fc4302d132ec32c619b023c7d791ef68882af19
// Source record: research/evidence/rea/still-sink-virtual-r2/05-procedure_pseudo_code.json

undefined8
FUN_0035bd78(undefined8 param_1,long *param_2,void *param_3,long *param_4,long *param_5,
            undefined4 *param_6)

{
  int iVar1;
  long lVar2;
  uint uVar3;
  undefined8 uVar4;
  long *plVar5;
  long *plVar6;
  uint uVar7;
  long lVar8;
  int iVar9;
  long lVar10;
  uint local_18 [2];
  undefined1 local_10 [16];
  
  FUN_0035e790(0,0,0xd,0x17c,0,0);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x17f,"setImageAreaAndParamForEdit");
  FUN_0035e7a8(1,"sizeof cod %d\n",0x3f0);
  plVar5 = param_4;
  iVar9 = 0;
  do {
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x188,"setImageAreaAndParamForEdit");
    FUN_0035e7a8(1,"image[%d].phsyAddr     is 0x%lX\n",iVar9,*plVar5);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x189,"setImageAreaAndParamForEdit");
    FUN_0035e7a8(1,"image[%d].logicAddr    is 0x%lX\n",iVar9,plVar5[1]);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x18a,"setImageAreaAndParamForEdit");
    iVar1 = iVar9 + 1;
    FUN_0035e7a8(1,"image[%d].size         is 0x%lX\n",iVar9,(int)plVar5[2]);
    plVar5 = plVar5 + 3;
    iVar9 = iVar1;
  } while (iVar1 != 3);
  lVar10 = *param_5;
  uVar3 = *(int *)(lVar10 + 4) * *(int *)(lVar10 + 8);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",400,"setImageAreaAndParamForEdit");
  FUN_0035e7a8(1,"gridNumColumn:%d gridNumRow:%d\n",*(undefined4 *)(lVar10 + 4),
               *(undefined4 *)(lVar10 + 8));
  if (uVar3 < 10) {
    *param_6 = *(undefined4 *)(lVar10 + 0x20);
    FUN_0035bc6c(param_1,param_3,lVar10,param_4);
    uVar7 = 0;
    plVar5 = param_2;
    do {
      if (uVar7 < uVar3) {
        lVar10 = param_4[1];
        *plVar5 = *param_4;
        plVar5[1] = lVar10;
        plVar5[2] = param_4[2];
      }
      else {
        *plVar5 = 0;
        plVar5[1] = 0;
        plVar5[2] = 0;
      }
      uVar7 = uVar7 + 1;
      plVar5 = plVar5 + 3;
    } while (uVar7 != 9);
    local_18[1] = 10;
    local_10[0] = 0x53;
    local_10[1] = 0x43;
    local_10[3] = 0;
    lVar10 = 0;
    local_10[2] = 0x4e;
    local_10[4] = 0;
    local_10[5] = 0x54;
    local_10[8] = 0;
    local_10[6] = 0x48;
    local_10[9] = 0;
    local_10[7] = 0x4d;
    local_18[0] = 9;
    do {
      param_5 = param_5 + 2;
      param_3 = (void *)((long)param_3 + 0x150);
      plVar5 = param_4 + 3;
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1aa,"setImageAreaAndParamForEdit");
      FUN_0035e7a8(1,"Start %s\n",local_10 + lVar10 * 5);
      if ((*plVar5 == 0) || (lVar8 = *param_5, lVar8 == 0)) {
        FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1b0,"setImageAreaAndParamForEdit");
        FUN_0035e7a8(1,"physic NULL\n");
        plVar6 = param_2 + (ulong)local_18[lVar10] * 3;
        *plVar6 = 0;
        plVar6[1] = 0;
        plVar6[2] = 0;
        memset(param_3,0,0x150);
      }
      else {
        lVar2 = param_4[4];
        plVar6 = param_2 + (ulong)local_18[lVar10] * 3;
        *plVar6 = *plVar5;
        plVar6[1] = lVar2;
        plVar6[2] = param_4[5];
        FUN_0035bc6c(param_1,param_3,lVar8,plVar5);
      }
      lVar10 = lVar10 + 1;
      param_4 = plVar5;
    } while (lVar10 != 2);
    param_2[0x21] = 0;
    param_2[0x22] = 0;
    param_2[0x23] = 0;
    FUN_0035e790(0,1,0xd,0x1b8,0,0);
    uVar4 = 0;
  }
  else {
    FUN_0035e790(0,1,0xd,0x192,0,0);
    FUN_0035e7a8(0,"HEIFLib LIB :ERROR %4d %s ",0x193,"setImageAreaAndParamForEdit");
    FUN_0035e7a8(0,"maxgridnum %d\n",uVar3);
    uVar4 = 1;
  }
  return uVar4;
}


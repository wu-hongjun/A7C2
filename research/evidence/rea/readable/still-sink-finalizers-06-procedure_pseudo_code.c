// Decompiled pseudocode, not original source.
// REA Evidence: ev_1bf0766470c216fe7766405746246a2facf6f3e3120414aa38c54c598478a256
// Source record: research/evidence/rea/still-sink-finalizers/06-procedure_pseudo_code.json

undefined8 FUN_003653f4(long param_1,long param_2,undefined4 *param_3,undefined8 *param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  long lVar5;
  undefined8 *puVar6;
  long lVar7;
  long lVar8;
  undefined8 uVar9;
  ulong uVar10;
  ulong uVar11;
  int iVar12;
  undefined8 *puVar13;
  
  FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x39,&DAT_003dd3c0);
  FUN_0035e7a8(1,"start---------------------------------\n");
  if (*(long *)(param_1 + 0x18) + 0x138U < *(ulong *)(param_1 + 0x28)) {
    if (*(int *)(*(long *)(param_1 + 0x30) + 0x1e0) == 0) {
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x42,&DAT_003dd3c0);
      FUN_0035e7a8(1,"addcount 0\n");
      lVar5 = *(long *)(param_1 + 0x30);
      puVar6 = (undefined8 *)(param_2 + 0x1c);
      uVar10 = 0;
      uVar4 = *(undefined8 *)(param_2 + 0x518);
      *(undefined8 *)(lVar5 + 0x1b0) = *(undefined8 *)(param_2 + 0x510);
      *(undefined8 *)(lVar5 + 0x1b8) = uVar4;
      lVar7 = *(long *)(param_2 + 0x510);
      *(undefined8 *)(lVar5 + 0x1c0) = *(undefined8 *)(param_2 + 0x520);
      *(bool *)(lVar5 + 0x1a5) = lVar7 != 0;
      do {
        lVar7 = *(long *)(param_1 + 0x30);
        lVar8 = uVar10 * 0x88;
        uVar11 = uVar10 & 0xffffffff;
        lVar5 = lVar7 + lVar8;
        iVar12 = 0;
        *(undefined4 *)(lVar7 + lVar8) = *(undefined4 *)(puVar6 + -3);
        *(undefined4 *)(lVar5 + 4) = *(undefined4 *)((long)puVar6 + -0x14);
        *(undefined4 *)(lVar5 + 8) = *(undefined4 *)(puVar6 + -2);
        *(undefined4 *)(lVar5 + 0xc) = *(undefined4 *)((long)puVar6 + -0xc);
        lVar5 = lVar7 + (uVar10 & 0xffffffff) * 0x88;
        puVar13 = (undefined8 *)(lVar7 + lVar8 + 0x18);
        *(undefined4 *)(lVar5 + 0x10) = *(undefined4 *)(puVar6 + -1);
        *(undefined4 *)(lVar5 + 0x14) = *(undefined4 *)((long)puVar6 + -4);
        uVar4 = puVar6[1];
        *puVar13 = *puVar6;
        puVar13[1] = uVar4;
        puVar13[2] = puVar6[2];
        *(undefined4 *)(puVar13 + 3) = *(undefined4 *)(puVar6 + 3);
        puVar13 = (undefined8 *)((long)puVar6 + 0x1c);
        do {
          FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x54,&DAT_003dd3c0);
          FUN_0035e7a8(1,"pParam->FixInfo[%d].HVCCInfoParamSet[%d].physicAddr is 0x%lX\n",uVar11,
                       iVar12,*puVar13);
          FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x56,&DAT_003dd3c0);
          FUN_0035e7a8(1,"pParam->FixInfo[%d].HVCCInfoParamSet[%d].logicAddr  is 0x%lX\n",uVar11,
                       iVar12,puVar13[1]);
          FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x58,&DAT_003dd3c0);
          FUN_0035e7a8(1,"pParam->FixInfo[%d].HVCCInfoParamSet[%d].size       is 0x%X\n",uVar11,
                       iVar12,*(undefined4 *)(puVar13 + 2));
          lVar5 = (long)iVar12;
          lVar7 = *(long *)(param_1 + 0x30);
          iVar12 = iVar12 + 1;
          uVar4 = puVar13[1];
          lVar5 = lVar7 + uVar11 * 0x88 + lVar5 * 0x18;
          *(undefined8 *)(lVar5 + 0x38) = *puVar13;
          *(undefined8 *)(lVar5 + 0x40) = uVar4;
          *(undefined8 *)(lVar5 + 0x48) = puVar13[2];
          puVar13 = puVar13 + 3;
        } while (iVar12 != 3);
        uVar10 = uVar10 + 1;
        puVar6 = puVar6 + 0x2a;
      } while (uVar10 != 3);
      uVar4 = *(undefined8 *)(param_2 + 0x548);
      uVar9 = *(undefined8 *)(param_2 + 0x550);
      *(undefined4 *)(lVar7 + 0x198) = 0xa0;
      *(undefined4 *)(lVar7 + 0x19c) = 0x78;
      *(undefined8 *)(lVar7 + 0x1c8) = uVar4;
      *(undefined8 *)(lVar7 + 0x1d0) = uVar9;
      uVar4 = *(undefined8 *)(param_2 + 0x558);
      *(undefined4 *)(lVar7 + 0x1a8) = *(undefined4 *)(param_2 + 0x540);
      uVar3 = *(undefined4 *)(param_2 + 0x544);
      *(undefined8 *)(lVar7 + 0x1d8) = uVar4;
      *(undefined4 *)(lVar7 + 0x1ac) = uVar3;
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x61,&DAT_003dd3c0);
      FUN_0035e7a8(1,"color = %d\n",*(undefined4 *)(*(long *)(param_1 + 0x30) + 0x1ac));
    }
    lVar5 = *(long *)(param_1 + 0x30);
    *(bool *)(lVar5 + 0x1a0) = *(long *)(param_2 + 0x3f0) != 0;
    *(bool *)(lVar5 + 0x1a1) = *(long *)(param_2 + 0x4c8) != 0;
    *(bool *)(lVar5 + 0x1a2) = *(long *)(param_2 + 0x4e0) != 0;
    *(bool *)(lVar5 + 0x1a3) = *(long *)(param_2 + 0x4f8) != 0;
    *(int *)(lVar5 + 0x1e0) = *(int *)(lVar5 + 0x1e0) + 1;
    uVar10 = 0;
    puVar6 = (undefined8 *)(param_2 + 0x3f0);
    do {
      lVar5 = *(long *)(param_1 + 0x18) + uVar10 * 0x18;
      uVar4 = puVar6[1];
      *(undefined8 *)(lVar5 + 0x18) = *puVar6;
      *(undefined8 *)(lVar5 + 0x20) = uVar4;
      *(undefined8 *)(lVar5 + 0x28) = puVar6[2];
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x6c,&DAT_003dd3c0);
      FUN_0035e7a8(1,"pParam->Image[%d].physicAddr is 0x%lX\n",uVar10,*puVar6);
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x6d,&DAT_003dd3c0);
      FUN_0035e7a8(1,"pParam->Image[%d].logicAddr  is 0x%lX\n",uVar10,puVar6[1]);
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x6e,&DAT_003dd3c0);
      uVar1 = (int)uVar10 + 1;
      FUN_0035e7a8(1,"pParam->Image[%d].size       is 0x%lX\n",uVar10,*(undefined4 *)(puVar6 + 2));
      uVar10 = (ulong)uVar1;
      puVar6 = puVar6 + 3;
    } while (uVar1 != 9);
    puVar6 = *(undefined8 **)(param_1 + 0x18);
    uVar4 = *(undefined8 *)(param_2 + 0x4c8);
    uVar9 = *(undefined8 *)(param_2 + 0x4d0);
    puVar6[0x20] = *(undefined8 *)(param_2 + 0x4d8);
    puVar6[0x1e] = uVar4;
    puVar6[0x1f] = uVar9;
    uVar4 = *(undefined8 *)(param_2 + 0x4e8);
    uVar9 = *(undefined8 *)(param_2 + 0x4f0);
    *puVar6 = *(undefined8 *)(param_2 + 0x4e0);
    puVar6[1] = uVar4;
    puVar6[2] = uVar9;
    uVar4 = *(undefined8 *)(param_2 + 0x500);
    uVar9 = *(undefined8 *)(param_2 + 0x508);
    puVar6[0x24] = *(undefined8 *)(param_2 + 0x4f8);
    puVar6[0x25] = uVar4;
    puVar6[0x26] = uVar9;
    puVar6 = (undefined8 *)(param_2 + 0x4c8);
    iVar12 = 9;
    do {
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x75,&DAT_003dd3c0);
      FUN_0035e7a8(1,"pParam->Image[%d].physicAddr is 0x%lX\n",iVar12,*puVar6);
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x76,&DAT_003dd3c0);
      FUN_0035e7a8(1,"pParam->Image[%d].logicAddr  is 0x%lX\n",iVar12,puVar6[1]);
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x77,&DAT_003dd3c0);
      iVar2 = iVar12 + 1;
      FUN_0035e7a8(1,"pParam->Image[%d].size       is 0x%lX\n",iVar12,*(undefined4 *)(puVar6 + 2));
      puVar6 = puVar6 + 3;
      iVar12 = iVar2;
    } while (iVar2 != 0xc);
    lVar5 = *(long *)(param_1 + 0x18);
    uVar4 = *(undefined8 *)(param_2 + 0x530);
    *(undefined8 *)(lVar5 + 0x108) = *(undefined8 *)(param_2 + 0x528);
    *(undefined8 *)(lVar5 + 0x110) = uVar4;
    lVar7 = *(long *)(param_1 + 0x30);
    *(undefined8 *)(lVar5 + 0x118) = *(undefined8 *)(param_2 + 0x538);
    *(bool *)(lVar7 + 0x1a4) = *(long *)(param_2 + 0x528) != 0;
    *(long *)(param_1 + 0x18) = lVar5 + 0x138;
    *param_3 = 0;
    uVar10 = 0;
    do {
      FUN_0035e7a8(1,"HEIFLib CORE:%4d %s ",0x81,&DAT_003dd3c0);
      uVar1 = (int)uVar10 + 1;
      FUN_0035e7a8(1,"exist %d %d\n",uVar10,
                   *(undefined1 *)(*(long *)(param_1 + 0x30) + uVar10 + 0x1a0));
      uVar10 = (ulong)uVar1;
    } while (uVar1 != 6);
    uVar4 = param_4[1];
    *(undefined8 *)(param_1 + 0x38) = *param_4;
    *(undefined8 *)(param_1 + 0x40) = uVar4;
    *(undefined8 *)(param_1 + 0x48) = param_4[2];
    uVar4 = 0;
  }
  else {
    FUN_0035e7a8(0,"HEIFLib CORE:ERROR %4d %s ",0x3b,&DAT_003dd3c0);
    FUN_0035e7a8(0,"our of range\n");
    FUN_0035e794(0,1,0x3c,0,0);
    uVar4 = 1;
  }
  return uVar4;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_f8404b6f1960ed97690e79f3f493a5faeecc2c94bb94e94f8ef1c857b51ff101
// Source record: research/evidence/rea/still-sink-virtual-r2/03-procedure_pseudo_code.json

ulong FUN_0035c3ac(undefined8 *param_1,long *param_2,undefined8 param_3)

{
  int iVar1;
  undefined1 *puVar2;
  int *piVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  char cVar7;
  uint uVar8;
  int *piVar9;
  int iVar10;
  long lVar11;
  ulong uVar12;
  uint uVar13;
  long *plVar14;
  long *plVar15;
  undefined8 *puVar16;
  ulong uVar17;
  uint local_5a0 [2];
  undefined1 local_598 [56];
  int local_560 [14];
  undefined8 local_528;
  undefined8 local_520;
  int local_518;
  int local_500;
  int local_4e8;
  int local_410 [168];
  undefined8 local_170 [2];
  int local_160 [62];
  long local_68;
  long local_60;
  uint local_58;
  undefined4 local_54;
  long local_50;
  long lStack_48;
  long local_40;
  long local_38;
  long lStack_30;
  undefined8 local_28;
  undefined4 local_20;
  uint local_1c;
  long local_18;
  long lStack_10;
  long local_8;
  
  FUN_0035e790(0,0,2,0x1fb,0,0);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1fc,&DAT_003dc098);
  FUN_0035e7a8(1,"start  ------------------------\n");
  FUN_0035e790(0,0,0xc,0xe3,0,0);
  plVar14 = param_2;
  iVar10 = 0;
  do {
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0xed,"setImageAreaAndParam");
    FUN_0035e7a8(1,"image[%d].phsyAddr  is 0x%lX\n",iVar10,*plVar14);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0xee,"setImageAreaAndParam");
    FUN_0035e7a8(1,"image[%d].logicAddr is 0x%lX\n",iVar10,plVar14[1]);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0xef,"setImageAreaAndParam");
    iVar1 = iVar10 + 1;
    FUN_0035e7a8(1,"image[%d].size      is 0x%lX\n",iVar10,(int)plVar14[2]);
    plVar14 = plVar14 + 3;
    iVar10 = iVar1;
  } while (iVar1 != 4);
  lVar11 = param_2[1];
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0xf4,"setImageAreaAndParam");
  FUN_0035e7a8(1,"MAIN:pStillInfo = 0x%lx\n",lVar11);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0xf5,"setImageAreaAndParam");
  FUN_0035e7a8(1,"MAIN:pStillInfo_physicAddr = 0x%lx\n",*param_2);
  cVar7 = *(char *)(lVar11 + 0x188);
  uVar8 = *(int *)(lVar11 + 0xc) * *(int *)(lVar11 + 0x10);
  if (9 < uVar8) {
    puts("HEIFASSERT");
    printf("max grid");
    FUN_0035e794(3,0,0xfe,0,0);
  }
  puVar16 = local_170;
  FUN_0035b21c(local_560,lVar11,param_2);
  uVar13 = 0;
  piVar9 = (int *)(lVar11 + 0x38);
  do {
    uVar4 = *(undefined4 *)((long)param_2 + 0x14);
    if (uVar13 < uVar8) {
      *puVar16 = local_528;
      puVar16[1] = local_520;
      iVar10 = local_4e8 + *piVar9 + local_518 + local_500;
      *(int *)(puVar16 + 2) = iVar10;
      *(undefined4 *)((long)puVar16 + 0x14) = uVar4;
      if (uVar8 - 1 == uVar13) {
        *(int *)(puVar16 + 2) = local_560[0] + iVar10;
      }
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x124,"setImageAreaAndParam");
      FUN_0035e7a8(1,"pCoreInfo->Image[%d].phsyAddr  is 0x%lX\n",uVar13,*puVar16);
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x125,"setImageAreaAndParam");
      FUN_0035e7a8(1,"pCoreInfo->Image[%d].logicAddr is 0x%lX\n",uVar13,puVar16[1]);
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x126,"setImageAreaAndParam");
      FUN_0035e7a8(1,"pCoreInfo->Image[%d].size      is 0x%lX\n",uVar13,*(undefined4 *)(puVar16 + 2)
                  );
    }
    else {
      *puVar16 = 0;
      puVar16[1] = 0;
      *(undefined4 *)(puVar16 + 2) = 0;
      *(undefined4 *)((long)puVar16 + 0x14) = uVar4;
    }
    uVar13 = uVar13 + 1;
    puVar16 = puVar16 + 3;
    piVar9 = piVar9 + 10;
  } while (uVar13 != 9);
  piVar9 = local_410;
  local_5a0[1] = 10;
  local_598[3] = 0;
  local_598[0] = 0x53;
  local_598[4] = 0;
  uVar12 = 0;
  local_598[1] = 0x43;
  local_598[8] = 0;
  local_598[2] = 0x4e;
  local_598[9] = 0;
  local_598[5] = 0x54;
  local_598[6] = 0x48;
  local_598[7] = 0x4d;
  local_5a0[0] = 9;
  plVar14 = param_2;
  do {
    plVar15 = plVar14 + 3;
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x138,"setImageAreaAndParam");
    puVar2 = local_598 + uVar12 * 5;
    FUN_0035e7a8(1,"Start %s\n",puVar2);
    if (*plVar15 == 0) {
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x157,"setImageAreaAndParam");
      FUN_0035e7a8(1,"physic NULL\n");
      uVar17 = (ulong)local_5a0[uVar12];
      piVar3 = local_560 + uVar17 * 6 + 0xfc;
      piVar3[0] = 0;
      piVar3[1] = 0;
      local_170[uVar17 * 3 + 1] = 0;
      (local_160 + uVar17 * 6)[0] = 0;
      (local_160 + uVar17 * 6)[1] = 0;
      memset(piVar9,0,0x150);
    }
    else {
      lVar11 = plVar14[4];
      FUN_0035b21c(piVar9,lVar11,plVar15);
      uVar17 = (ulong)local_5a0[uVar12];
      iVar10 = piVar9[0x12];
      iVar1 = piVar9[0x18];
      iVar5 = piVar9[0x1e];
      local_170[uVar17 * 3] = *(undefined8 *)(piVar9 + 0xe);
      local_170[uVar17 * 3 + 1] = *(undefined8 *)(piVar9 + 0x10);
      iVar6 = *(int *)((long)plVar14 + 0x2c);
      local_160[uVar17 * 6] = iVar5 + *piVar9 + iVar10 + iVar1 + *(int *)(lVar11 + 0x38);
      local_160[uVar17 * 6 + 1] = iVar6;
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x153,"setImageAreaAndParam");
      FUN_0035e7a8(1,"pCoreInfo->Image[%d].phsyAddr  is 0x%lX\n",uVar12 & 0xffffffff,
                   local_170[uVar17 * 3]);
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x154,"setImageAreaAndParam");
      FUN_0035e7a8(1,"pCoreInfo->Image[%d].logicAddr is 0x%lX\n",uVar12 & 0xffffffff,
                   local_170[uVar17 * 3 + 1]);
      FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x155,"setImageAreaAndParam");
      FUN_0035e7a8(1,"pCoreInfo->Image[%d].size      is 0x%lX\n",uVar12 & 0xffffffff,
                   local_160[uVar17 * 6]);
    }
    uVar12 = uVar12 + 1;
    piVar9 = piVar9 + 0x54;
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x15c,"setImageAreaAndParam");
    FUN_0035e7a8(1,"%s:pStillInfo            = 0x%lx\n",puVar2,lVar11);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x15d,"setImageAreaAndParam");
    FUN_0035e7a8(1,"%s:pStillInfo_physicAddr = 0x%lx\n",puVar2,*plVar15);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x15e,"setImageAreaAndParam");
    FUN_0035e7a8(1,"%s:offset_of_data_addr   = 0x%lx\n",puVar2,*(undefined8 *)(lVar11 + 0x30));
    plVar14 = plVar15;
  } while (uVar12 != 2);
  local_68 = param_2[9];
  local_60 = param_2[10];
  local_54 = *(undefined4 *)((long)param_2 + 0x5c);
  local_58 = (int)param_2[0xb] + 0xfffU & 0xfffff000;
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x167,"setImageAreaAndParam");
  FUN_0035e7a8(1,"pOfsList ===========================\n");
  FUN_0035e790(0,1,0xc,0x168,0,0);
  local_18 = param_2[0x12];
  lStack_10 = param_2[0x13];
  local_8 = param_2[0x14];
  local_50 = param_2[0xc];
  lStack_48 = param_2[0xd];
  local_40 = param_2[0xe];
  local_38 = param_2[0xf];
  lStack_30 = param_2[0x10];
  local_28._4_4_ = (undefined4)((ulong)param_2[0x11] >> 0x20);
  local_28 = CONCAT44(local_28._4_4_,(int)param_2[0x11] + 0xfff) & 0xfffffffffffff000;
  local_20 = (undefined4)param_2[0x15];
  local_1c = (uint)(cVar7 == 'W');
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x211,&DAT_003dc098);
  FUN_0035e7a8(1,"param color = %d\n",local_1c);
  if (*(uint *)(param_2 + 0x14) < 0x1000) {
    puts("HEIFASSERT");
    printf("headarealess");
    FUN_0035e794(3,0,0x214,0,0);
  }
  uVar12 = FUN_003653f4(*param_1,local_560,local_598,param_3);
  if ((int)uVar12 == 0) {
    FUN_0035e790(uVar12,1,2,0x21d,0,0);
  }
  return uVar12 & 0xffffffff;
}


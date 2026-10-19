// Decompiled pseudocode, not original source.
// REA Evidence: ev_42b72fdc24d01213a92c061a7bf2818364057bd25511587c3bae9fdcb583ad29
// Source record: research/evidence/rea/sony-fsys-completion/03-procedure_pseudo_code.json

undefined8 _ZN13InfraFsysMcmd17IFSYS_WriteDirectEiPPc(int param_1,long param_2)

{
  char cVar1;
  int iVar2;
  undefined8 uVar3;
  ulong uVar4;
  ulong uVar5;
  ulong uVar6;
  char *pcVar7;
  undefined4 *puVar8;
  uint uVar9;
  undefined8 *puVar10;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined4 local_48;
  undefined2 local_44;
  undefined8 local_40;
  undefined8 uStack_38;
  undefined8 local_30;
  undefined8 uStack_28;
  undefined8 local_20;
  undefined8 uStack_18;
  undefined8 local_10;
  undefined8 uStack_8;

  if (param_1 < 4) {
    _ZNK7MonCtrl12ConPutFewArgEv(_ZN13InfraFsysMcmd6mp_msgE);
    _ZNK7MonCtrl16ConPrintfMessageEPKcz
              (_ZN13InfraFsysMcmd6mp_msgE,
               "writedirect [path] [filesize], [listcount] (add1, size1, addr2, size2, .....)\n");
    _ZNK7MonCtrl16ConPrintfMessageEPKcz
              (_ZN13InfraFsysMcmd6mp_msgE,
               "  <path>    : /dev/mmca, /dev/mmca1 /dev/sxs_blk0, /dev/sxs_blk1,...\n");
  }
  else {
    local_44 = 0;
    local_48 = 0;
    local_58 = 0;
    uStack_50 = 0;
    cVar1 = _ZN13InfraFsysMcmd14GetCuttentTimeER8FSYSTIME();
    if (cVar1 == '\0') {
      _ZNK7MonCtrl9ConPrintfEPKcz
                (_ZN13InfraFsysMcmd6mp_msgE,"%s %d: GetCuttentTime Error.\n","IFSYS_WriteDirect",
                 0x5b7);
    }
    else {
      uVar3 = *(undefined8 *)(param_2 + 8);
      uVar4 = strtol(*(char **)(param_2 + 0x10),(char **)0x0,0);
      uVar5 = strtol(*(char **)(param_2 + 0x18),(char **)0x0,0);
      _ZNK7MonCtrl9ConPrintfEPKcz(_ZN13InfraFsysMcmd6mp_msgE,"filepath  = %s \n",uVar3);
      _ZNK7MonCtrl9ConPrintfEPKcz
                (_ZN13InfraFsysMcmd6mp_msgE,"filesize  = 0x%x \n",uVar4 & 0xffffffff);
      _ZNK7MonCtrl9ConPrintfEPKcz(_ZN13InfraFsysMcmd6mp_msgE,"listcount = %d \n",uVar5 & 0xffffffff)
      ;
      _ZNK7MonCtrl9ConPrintfEPKcz(_ZN13InfraFsysMcmd6mp_msgE,"[FSYS_DIRECT_AREA List]\n");
      local_40 = 0;
      uStack_38 = 0;
      local_30 = 0;
      uStack_28 = 0;
      local_20 = 0;
      uStack_18 = 0;
      local_10 = 0;
      uStack_8 = 0;
      puVar8 = (undefined4 *)((long)&local_40 + 4);
      puVar10 = (undefined8 *)(param_2 + 0x20);
      for (uVar9 = 0; uVar9 < (uint)uVar5; uVar9 = uVar9 + 1) {
        uVar6 = strtoul((char *)*puVar10,(char **)0x0,0);
        puVar8[-1] = (int)uVar6;
        uVar6 = strtoul((char *)puVar10[1],(char **)0x0,0);
        *puVar8 = (int)uVar6;
        _ZNK7MonCtrl9ConPrintfEPKcz
                  (_ZN13InfraFsysMcmd6mp_msgE,"list[%d].addr = 0x%08x \n",uVar9,puVar8[-1]);
        _ZNK7MonCtrl9ConPrintfEPKcz
                  (_ZN13InfraFsysMcmd6mp_msgE,"list[%d].size = 0x%08x \n",uVar9,*puVar8);
        puVar8 = puVar8 + 2;
        puVar10 = puVar10 + 2;
      }
      iVar2 = Fsys_WriteDirectFile(uVar3,&local_58,uVar4 & 0xffffffff,&local_40,uVar5 & 0xffffffff);
      if (iVar2 == 0) {
        iVar2 = 0;
        pcVar7 = "Fsys_WriteDirectFile OK\n";
      }
      else {
        pcVar7 = "Fsys_WriteDirectFile Error(0x%08x)\n";
      }
      _ZNK7MonCtrl9ConPrintfEPKcz(_ZN13InfraFsysMcmd6mp_msgE,pcVar7,iVar2);
    }
  }
  return 1;
}

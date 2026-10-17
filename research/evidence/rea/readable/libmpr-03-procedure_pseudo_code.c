// Decompiled pseudocode, not original source.
// REA Evidence: ev_b3e7abfe8fe63f25f98d9abad2dd8c04ac562b3bbd63278e6c4e8020ec8f5788
// Source record: research/evidence/rea/libmpr/03-procedure_pseudo_code.json

int FUN_0027e880(undefined8 param_1,char *param_2,undefined8 param_3,undefined8 param_4,int *param_5
                )

{
  char cVar1;
  int iVar2;
  FILE *__stream;
  undefined2 *puVar3;
  undefined8 uVar4;
  undefined8 local_e0;
  undefined8 uStack_d8;
  undefined8 local_d0;
  int local_b8;
  int local_b4;
  ulong local_b0;
  undefined2 local_a8;
  undefined2 local_a6;
  undefined1 local_a4;
  undefined1 local_a3;
  undefined1 local_a2;
  undefined1 local_a1;
  int local_a0;
  undefined2 local_98;
  undefined2 uStack_96;
  undefined1 uStack_94;
  undefined1 uStack_93;
  undefined1 uStack_92;
  undefined1 uStack_91;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 uStack_84;
  undefined2 local_80;
  undefined2 local_7e;
  undefined1 local_7c;
  undefined1 local_7b;
  undefined1 local_7a;
  undefined1 local_79;
  undefined4 local_50;
  
  *param_5 = 0;
  __stream = fopen(param_2,"r");
  if (__stream == (FILE *)0x0) {
    local_7b = 0;
    local_7c = 4;
    local_7a = 0;
    local_80 = 0x2304;
    local_79 = 0;
    local_7e = 0;
    FUN_0027db18(&local_80,8);
    return 4;
  }
  puVar3 = &local_98;
  iVar2 = FUN_003cbd78(param_2,&local_80);
  if (iVar2 == 0) {
    local_b0 = 0;
    local_98 = 0;
    uStack_96 = 0;
    uStack_94 = 0;
    uStack_93 = 0;
    uStack_92 = 0;
    uStack_91 = 0;
    local_90 = 0;
    local_b8 = 0;
    local_8c = 0x3f800000;
    local_88 = 0x3f800000;
    uStack_84 = 0x3f800000;
    FUN_0027dfc8(param_1,__stream,local_50,&local_b8,puVar3);
    if (local_b8 == 0) {
      uVar4 = 8;
      local_a4 = 4;
      local_a6 = 2;
    }
    else {
      local_b4 = 0;
      iVar2 = FUN_0027e380(param_1,local_b8,param_4);
      if (iVar2 != 0) {
        local_a3 = 0;
        local_a4 = 3;
        local_a2 = 0;
        local_a8 = 0x2304;
        local_a1 = 0;
        local_a6 = 3;
        local_a0 = iVar2;
        FUN_0027db18(&local_a8,0xc);
        fclose(__stream);
        return iVar2;
      }
      FUN_0027e3ac(param_1,local_b8,local_b4,param_4);
      FUN_0027e174(param_1,local_b4 + 0x40,param_4);
      local_e0 = CONCAT17(uStack_91,
                          CONCAT16(uStack_92,
                                   CONCAT15(uStack_93,
                                            CONCAT14(uStack_94,CONCAT22(uStack_96,local_98)))));
      uStack_d8 = CONCAT44(local_8c,local_90);
      local_d0 = CONCAT44(uStack_84,local_88);
      FUN_0027e250(param_1,local_b8,&local_e0,local_b4 + 0x80,param_4);
      FUN_0027e18c(param_1,local_b4 + 0xc0,param_4);
      FUN_0027e1a4(param_1,param_3,local_b4 + 0x100,param_4);
      FUN_0027e21c(param_1,local_b4 + 0x400,param_4);
      FUN_0027e22c(param_1,local_b4 + 0x500,param_4);
      cVar1 = FUN_0027e500(param_1,local_b0 & 0xffffffff,local_b4 + 0xd00,local_50,local_b8,__stream
                           ,param_4);
      if (cVar1 != '\0') {
        fclose(__stream);
        iVar2 = FUN_0027e368(param_1,local_b8);
        *param_5 = local_b4 + iVar2;
        return 0;
      }
      uVar4 = 0xc;
      local_a4 = 3;
      local_a6 = 4;
      local_a0 = 0x4f;
    }
    local_a1 = 0;
    local_a2 = 0;
    local_a3 = 0;
    local_a8 = 0x2304;
    puVar3 = &local_a8;
  }
  else {
    uStack_93 = 0;
    uStack_92 = 0;
    uVar4 = 8;
    uStack_94 = 4;
    uStack_91 = 0;
    local_98 = 0x2304;
    uStack_96 = 1;
  }
  FUN_0027db18(puVar3,uVar4);
  fclose(__stream);
  return 2;
}


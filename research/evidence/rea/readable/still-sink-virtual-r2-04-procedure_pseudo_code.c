// Decompiled pseudocode, not original source.
// REA Evidence: ev_d69f88319e76a52d79574122a78d7141a4d298be3a7a1ee2527eeb9cb514a7e6
// Source record: research/evidence/rea/still-sink-virtual-r2/04-procedure_pseudo_code.json

ulong FUN_0035c118(undefined8 *param_1,long param_2,undefined8 param_3)

{
  uint uVar1;
  ulong uVar2;
  undefined8 uVar3;
  ulong uVar4;
  undefined4 local_5b4;
  undefined8 local_5b0;
  undefined8 uStack_5a8;
  undefined8 local_5a0;
  undefined1 auStack_598 [56];
  undefined1 auStack_560 [1008];
  undefined1 auStack_170 [288];
  undefined8 local_50;
  undefined8 local_48;
  ulong local_40;
  undefined8 local_38;
  undefined8 local_30;
  undefined8 local_28;
  undefined4 local_20;
  undefined4 local_1c;
  undefined8 local_18;
  undefined8 uStack_10;
  undefined8 local_8;
  
  FUN_0035e790(0,0,7,0x1c4,0,0);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1c6,"AddForEdit");
  FUN_0035e7a8(1,"start  ------------------------\n");
  uVar1 = FUN_0035bd78(param_1,auStack_170,auStack_560,param_2,param_3,&local_5b4);
  uVar4 = (ulong)uVar1;
  if (uVar1 == 0) {
    local_50 = *(undefined8 *)(param_2 + 0x60);
    local_48 = *(undefined8 *)(param_2 + 0x68);
    local_1c = local_5b4;
    local_40 = *(ulong *)(param_2 + 0x70);
    local_38 = *(undefined8 *)(param_2 + 0x78);
    local_30 = *(undefined8 *)(param_2 + 0x80);
    local_28._4_4_ = (undefined4)((ulong)*(undefined8 *)(param_2 + 0x88) >> 0x20);
    local_28 = CONCAT44(local_28._4_4_,(int)*(undefined8 *)(param_2 + 0x88) + 0xfff) &
               0xfffffffffffff000;
    local_20 = *(undefined4 *)(param_2 + 0xa8);
    local_18 = *(undefined8 *)(param_2 + 0x90);
    uStack_10 = *(undefined8 *)(param_2 + 0x98);
    local_8 = *(undefined8 *)(param_2 + 0xa0);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1dd,"AddForEdit");
    FUN_0035e7a8(1,"xmp : p:%08X l:%08X s:%d\n",local_50,local_48,local_40 & 0xffffffff);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1de,"AddForEdit");
    FUN_0035e7a8(1,"exif: p:%08X l:%08X s:%d\n",local_38,local_30,local_28 & 0xffffffff);
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x1df,"AddForEdit");
    FUN_0035e7a8(1,"param color = %d\n",local_1c);
    if (*(uint *)(param_2 + 0xa0) < 0x1000) {
      puts("HEIFASSERT");
      printf("headarealess");
      FUN_0035e794(3,0,0x1e2,0,0);
    }
    local_5b0 = 0;
    uStack_5a8 = 0;
    local_5a0 = 0;
    uVar2 = FUN_003653f4(*param_1,auStack_560,auStack_598,&local_5b0);
    uVar4 = uVar2 & 0xffffffff;
    if ((int)uVar2 == 0) {
      FUN_0035e790(uVar2,1,7,0x1ee,0,0);
      return uVar4;
    }
    FUN_0035e790(0,1,7,0x1e9,0,0);
    uVar3 = 0x1ea;
  }
  else {
    FUN_0035e798(local_5b4,0,0x1ce);
    FUN_0035e790(0,1,7,0x1cf,0,0);
    uVar3 = 0x1d0;
  }
  FUN_0035e7a8(0,"HEIFLib LIB :ERROR %4d %s ",uVar3,"AddForEdit");
  FUN_0035e7a8(0,"ret %d\n",uVar4);
  return uVar4;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_3de9c2a8388089c0443c6e23a5eccbb8538dc13e2386f82df448a7e1b01d5694
// Source record: research/evidence/rea/still-sink-virtual-r2/06-procedure_pseudo_code.json

undefined8
_ZN7HEIFLib3FinEPN10HEIFCommon17AREA_MULTI_LIST_TEbPN3mpr8libcrypt10assetParamE
          (undefined8 *param_1,uint *param_2,undefined1 param_3,undefined8 param_4)

{
  long lVar1;
  uint uVar2;
  long lVar3;
  int iVar4;
  undefined2 local_10;
  undefined2 local_e;
  undefined1 local_c;
  undefined1 local_b;
  undefined1 local_a;
  undefined1 local_9;
  undefined4 local_8;
  undefined4 uStack_4;
  
  local_c = 2;
  local_10 = 0x2261;
  local_e = 0x2001;
  local_b = 0;
  local_a = 0;
  local_9 = 0;
  local_8 = 0;
  uStack_4 = 0;
  FUN_0035b938(&local_10,0x10);
  FUN_0035e790(0,0,3,0x22d,0,0);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x22e,&DAT_003dc0a8);
  FUN_0035e7a8(1,"start  ------------------------\n");
  iVar4 = 0;
  _ZN10HEIFCoreIf3FinEPN10HEIFCommon17AREA_MULTI_LIST_TEbPN3mpr8libcrypt10assetParamE
            (*param_1,param_2,param_3,param_4);
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x231,&DAT_003dc0a8);
  FUN_0035e7a8(1,"WriteList\n");
  for (uVar2 = 0; uVar2 < *param_2; uVar2 = uVar2 + 1) {
    FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x235,&DAT_003dc0a8);
    lVar3 = (ulong)uVar2 * 0x18;
    lVar1 = *(long *)(param_2 + 2) + lVar3;
    FUN_0035e7a8(1,"%d p:%08X l:%08X %d\n",uVar2,*(undefined8 *)(*(long *)(param_2 + 2) + lVar3),
                 *(undefined8 *)(lVar1 + 8),*(undefined4 *)(lVar1 + 0x10));
    iVar4 = iVar4 + *(int *)(*(long *)(param_2 + 2) + lVar3 + 0x10);
  }
  FUN_0035e7a8(1,"HEIFLib LIB :%4d %s ",0x238,&DAT_003dc0a8);
  FUN_0035e7a8(1,"WriteSize Total %08X\n",iVar4);
  FUN_0035e790(0,1,3,0x23a,0,0);
  return 0;
}


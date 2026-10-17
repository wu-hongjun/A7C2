// Decompiled pseudocode, not original source.
// REA Evidence: ev_df87b49af99b2aba53509aaaddb33fb98400e55af90dc7e594f5710cfc49c028
// Source record: research/evidence/rea/libmpr-extra/00-procedure_pseudo_code.json

undefined8
_ZN3mpr12KlvConverter28GetSerialize_SonyLookControlEjPhPj
          (uint param_1,long param_2,undefined4 *param_3)

{
  int iVar1;
  long lVar2;
  undefined8 local_18;
  undefined8 uStack_10;
  undefined4 local_8;
  
  *param_3 = 0;
  if (param_1 != 0) {
    *(undefined2 *)(param_2 + 0x20) = 0xfe;
    *(undefined1 *)(param_2 + 0x26) = 1;
    *(undefined4 *)(param_2 + 0x22) = 0x1000083;
    iVar1 = param_1 + 0xd;
    *(uint *)(param_2 + 0x29) =
         (param_1 >> 0x10 & 0xff) << 8 | (param_1 >> 8 & 0xff) << 0x10 | param_1 << 0x18 | 0x83;
    lVar2 = 0;
    *(undefined2 *)(param_2 + 0x27) = 0x1fe;
    do {
      *(undefined1 *)((long)&local_18 + lVar2) =
           (&_ZN3mpr12KlvConverter27UDAMSetSonyLookControlValueE)[lVar2];
      lVar2 = lVar2 + 1;
    } while (lVar2 != 0x10);
    local_8._3_1_ = (undefined1)iVar1;
    local_8._0_3_ = CONCAT12((char)((uint)iVar1 >> 8),CONCAT11((char)((uint)iVar1 >> 0x10),0x83));
    *(undefined4 *)(param_2 + 0x1c) = local_8;
    *(undefined8 *)(param_2 + 0xc) = local_18;
    *(undefined8 *)(param_2 + 0x14) = uStack_10;
    *param_3 = 0x2d;
    return 0;
  }
  return 2;
}


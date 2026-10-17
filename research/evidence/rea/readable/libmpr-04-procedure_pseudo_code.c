// Decompiled pseudocode, not original source.
// REA Evidence: ev_6e91b376ba3e4572703d2c63ea6862e3ed188346c00bca73a3eb900e1bb094ac
// Source record: research/evidence/rea/libmpr/04-procedure_pseudo_code.json

void FUN_0027dfc8(undefined8 param_1,FILE *param_2,uint param_3,undefined4 *param_4,
                 undefined8 param_5,long *param_6)

{
  int iVar1;
  size_t sVar2;
  ulong __off;
  uint local_214;
  undefined2 local_210;
  undefined2 local_20e;
  undefined1 local_20c;
  undefined1 local_20b;
  undefined1 local_20a;
  undefined1 local_209;
  undefined4 local_208;
  undefined1 auStack_200 [512];
  
  __off = 0;
  *param_4 = 0;
  local_214 = 0;
  do {
    if (param_3 <= __off) {
LAB_0027e134:
      *param_6 = local_214 + __off;
      local_20e = 0x13;
      local_208 = (undefined4)(local_214 + __off);
LAB_0027e05c:
      local_209 = 0;
      local_20a = 0;
      local_20b = 0;
      local_20c = 3;
      local_210 = 0x2304;
      FUN_0027db18(&local_210,0xc);
      return;
    }
    iVar1 = fseek(param_2,__off,0);
    if (iVar1 != 0) {
      local_20e = 0x10;
      local_208 = 0x76;
      goto LAB_0027e05c;
    }
    sVar2 = fread(auStack_200,1,0x200,param_2);
    if ((int)sVar2 == 0) {
      local_20e = 0x11;
      local_208 = 0x7b;
      goto LAB_0027e05c;
    }
    FUN_0027dd3c(param_1,auStack_200,sVar2 & 0xffffffff,param_5);
    iVar1 = FUN_0027dee0(param_1,auStack_200,param_4,&local_214);
    if (iVar1 == 0) {
      local_20b = 0;
      local_20c = 3;
      local_20a = 0;
      local_210 = 0x2304;
      local_209 = 0;
      local_20e = 0x12;
      local_208 = *param_4;
      FUN_0027db18(&local_210,0xc);
      goto LAB_0027e134;
    }
    __off = __off + (long)(int)sVar2;
  } while( true );
}


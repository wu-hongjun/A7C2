// Decompiled pseudocode, not original source.
// REA Evidence: ev_c4f5ec1e828fa2c6f801202abaebaf8abd0939a4ecc412a8153e5c1d1d9898c8
// Source record: research/evidence/rea/jiritsu-exact/00-procedure_pseudo_code.json

void adj_cmd_set_file_path(long param_1,long *param_2)

{
  byte *pbVar1;
  uint uVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  void *__ptr;
  ulong uVar6;
  long lVar7;
  
  uVar5 = *(uint *)(param_1 + 0x20);
  if (uVar5 - 2 < 0x200) {
    uVar2 = uVar5 - 1;
    uVar3 = _ZN6DmmMgr13CheckUnitTypeEm((ulong)uVar2);
    iVar4 = _ZN6DmmMgr9dmm_allocEj(param_2,uVar3);
    if (iVar4 != 0) {
      uVar5 = 0x15;
      goto LAB_00108df8;
    }
    if (*(char *)(param_1 + 0x28) == '\0') {
      lVar7 = *(long *)(*param_2 + 8);
      if (DAT_001487e0 != (void *)0x0) {
        free(DAT_001487e0);
      }
      DAT_001487e0 = calloc((ulong)uVar5,1);
      __ptr = memcpy(DAT_001487e0,(void *)(lVar7 + 1),(ulong)uVar2);
      lVar7 = 0;
      do {
        if (uVar2 <= (uint)lVar7) {
          uVar5 = 0;
          goto LAB_00108dfc;
        }
        if (*(char *)((long)__ptr + lVar7) == '\\') {
          *(undefined1 *)((long)__ptr + lVar7) = 0x2f;
        }
        pbVar1 = (byte *)((long)__ptr + lVar7);
        lVar7 = lVar7 + 1;
        uVar6 = ~(0x100000002400U >> ((ulong)*pbVar1 & 0x3f));
        if (0x2c < *pbVar1) {
          uVar6 = 1;
        }
      } while ((uVar6 & 1) != 0);
      free(__ptr);
      DAT_001487e0 = (void *)0x0;
    }
  }
  else {
    _Z18jiritsu_cpp_logrechPKcj
              (0xffffff81,"adj_cmd_set_file_path: Request raw data size error.(max_size:%d)\n",0x201
              );
  }
  uVar5 = 0x201;
LAB_00108df8:
  uVar5 = uVar5 | 0x10000000;
LAB_00108dfc:
  *(uint *)(param_1 + 0x1c) = uVar5;
  return;
}


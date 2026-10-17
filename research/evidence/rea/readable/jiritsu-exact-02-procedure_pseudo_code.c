// Decompiled pseudocode, not original source.
// REA Evidence: ev_6476b18e182c7f44707503d750feb903ec997a29e08db6b596787848365166a2
// Source record: research/evidence/rea/jiritsu-exact/02-procedure_pseudo_code.json

uint adj_cmd_set_file2(long param_1,long *param_2)

{
  char cVar1;
  byte bVar2;
  uint uVar3;
  long *plVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  uint uVar8;
  undefined4 uVar9;
  int iVar10;
  undefined8 uVar11;
  size_t sVar12;
  char *pcVar13;
  ulong uVar14;
  uint uVar15;
  long lVar16;
  ulong __n;
  long lVar17;
  char local_a40 [520];
  undefined1 auStack_838 [2104];
  
  _ZN10libJiritsu11CFileAccessC2Ev(auStack_838);
  memset(local_a40,0,0x202);
  uVar15 = *(uint *)(param_1 + 0x20);
  cVar1 = *(char *)(param_1 + 0x28);
  uVar3 = uVar15 - 3;
  if (cVar1 == '\x01') {
    pcVar13 = "/mnt";
    *(undefined1 *)(param_1 + 0x29) = 0xff;
LAB_00109928:
    if ((((1 < *(byte *)(param_1 + 0x2a)) ||
         (get_access_filename(*(undefined1 *)(param_1 + 0x29),pcVar13,local_a40,0x202),
         local_a40[0] == '\0')) || (iVar10 = strcmp(local_a40,"/tmp/jiritsu/jit.log"), iVar10 == 0))
       || (iVar10 = strcmp(local_a40,"/tmp/jiritsu/history.txt"), iVar10 == 0)) goto LAB_001098d4;
    uVar8 = 0;
    iVar10 = strcmp(local_a40,"/tmp/jiritsu/libJit.so");
    if (iVar10 == 0) {
      uVar8 = sys_file_chk_exist("/tmp/jbusy");
      if (uVar8 == 0) {
        uVar15 = 0x11;
        goto LAB_001098dc;
      }
    }
    if ((*(char *)(param_1 + 0x28) == '\0') &&
       (iVar10 = sys_get_dir_size(pcVar13,1), 0x1900000U - iVar10 < uVar3)) {
      _Z18jiritsu_cpp_logrechPKcj
                (0xffffff81,"adj_cmd_set_file2: file size over err size = %u",uVar3);
      uVar15 = 0xf;
    }
    else {
      iVar10 = sys_dir_create2(local_a40);
      if (iVar10 == 0) {
        uVar9 = _ZN6DmmMgr13CheckUnitTypeEm(uVar3 + (*(char *)(param_1 + 0x2a) == '\x01') * uVar3);
        iVar10 = _ZN6DmmMgr9dmm_allocEj(param_2,uVar9);
        if (iVar10 != 0) goto LAB_00109c78;
        lVar17 = *(long *)(*param_2 + 8);
        if (*(char *)(param_1 + 0x2a) == '\x01') {
          libCommuTalk(lVar17 + 3,lVar17 + (ulong)uVar15,uVar3);
        }
        uVar8 = _ZN10libJiritsu11CFileAccess4openEPKci(auStack_838,local_a40,0x241);
        if ((int)uVar8 < 0) {
          _Z18jiritsu_cpp_logrechPKcj(0xffffff81,"adj_cmd_set_file2: open err ret = %d",uVar8);
          sVar12 = strlen(local_a40);
          _Z22jiritsu_cppdump_logrechPKcPhj
                    (0xffffff84,"adj_cmd_set_file2: file",local_a40,sVar12 & 0xffffffff);
          uVar15 = 8;
        }
        else {
          lVar16 = lVar17 + (ulong)uVar15;
          if (*(char *)(param_1 + 0x2a) != '\x01') {
            lVar16 = lVar17 + 3;
          }
          uVar8 = _ZN10libJiritsu11CFileAccess5writeEPKvm(auStack_838,lVar16,uVar3);
          _ZN10libJiritsu11CFileAccess5fsyncEv(auStack_838);
          _ZN10libJiritsu11CFileAccess5closeEv(auStack_838);
          if (uVar3 == uVar8) {
            uVar15 = 0;
            iVar10 = strcmp(local_a40,"/tmp/jiritsu/libJit.so");
            if (iVar10 != 0) goto LAB_00109a24;
            _Z22jiritsu_cppdump_logrechPKcPhj
                      (0xffffff84,"adj_cmd_set_file2: file","/tmp/jiritsu/libJit.so",0x16);
            lVar17 = dlopen("/tmp/jiritsu/libJit.so",1);
            if (lVar17 == 0) {
              _Z22jiritsu_cppdump_logrechPKcPhj
                        (0xffffff84,"adj_cmd_set_file2: file","/tmp/jiritsu/libJit.so",0x16);
              uVar15 = 0x10000000;
              goto LAB_00109a24;
            }
            dlclose();
            goto LAB_00109a20;
          }
          _Z18jiritsu_cpp_logrechPKcj(0xffffff81,"adj_cmd_set_file2: write err fd_mem = %d",uVar8);
          uVar15 = 9;
        }
      }
      else {
        _Z18jiritsu_cpp_logrechPKc(0xffffff81,"adj_cmd_set_file2: directory create error.");
        uVar15 = 0x1c;
      }
    }
  }
  else {
    if (cVar1 == '\0') {
      if (*(char *)(param_1 + 0x29) != '\x02') {
        uVar8 = sys_mount("/dev/nflasha6","/tmp/jiritsu");
        pcVar13 = "/tmp/jiritsu";
        if (uVar8 == 0) goto LAB_00109928;
        uVar15 = 4;
        goto LAB_001098dc;
      }
    }
    else if (cVar1 == -1) {
      __n = (ulong)uVar3;
      uVar9 = _ZN6DmmMgr13CheckUnitTypeEm(uVar3);
      iVar10 = _ZN6DmmMgr9dmm_allocEj(param_2,uVar9);
      if (iVar10 != 0) {
        uVar8 = 0;
LAB_00109c78:
        uVar15 = 0x15;
        goto LAB_001098dc;
      }
      bVar2 = *(byte *)(param_1 + 0x2a);
      bVar5 = false;
      bVar6 = false;
      bVar7 = false;
      if (uVar3 < 0x500001) {
        uVar15 = (uint)bVar2;
        bVar7 = SBORROW4(uVar15,0x11);
        bVar5 = (int)(uVar15 - 0x11) < 0;
        bVar6 = uVar15 == 0x11;
      }
      if (!bVar6 && bVar5 == bVar7) goto LAB_001098d4;
      if (*DAT_00148bf0 == 0) {
        uVar15 = 0x106;
        uVar8 = 0;
        goto LAB_001098dc;
      }
      uVar14 = (ulong)*(byte *)(param_1 + 0x29);
      lVar17 = *DAT_00148bf0 + (ulong)*(byte *)(param_1 + 0x29) * 8;
      lVar16 = *(long *)(*param_2 + 8);
      if (*(long *)(lVar17 + 0x28) == 0) {
LAB_001099c4:
        lVar17 = *DAT_00148bf0;
        uVar11 = _Znam(__n);
        *(undefined8 *)(lVar17 + uVar14 * 8 + 0x28) = uVar11;
        *(ulong *)(*DAT_00148bf0 + uVar14 * 8 + 0x1028) = __n;
      }
      else if (*(ulong *)(lVar17 + 0x1028) < __n) {
        _ZdaPv();
        *(undefined8 *)(*DAT_00148bf0 + uVar14 * 8 + 0x28) = 0;
        goto LAB_001099c4;
      }
      plVar4 = DAT_00148bf0;
      memcpy(*(void **)(*DAT_00148bf0 + uVar14 * 8 + 0x28),(void *)(lVar16 + 3),__n);
      *(uint *)(*plVar4 + uVar14 * 4 + 0xc28) = uVar3;
      *(uint *)(*plVar4 + uVar14 * 4 + 0x828) = (uint)bVar2;
      uVar8 = 0;
LAB_00109a20:
      uVar15 = 0;
      goto LAB_00109a24;
    }
LAB_001098d4:
    uVar8 = 0;
    uVar15 = 0x201;
  }
LAB_001098dc:
  uVar15 = uVar15 | 0x10000000;
LAB_00109a24:
  if (DAT_001487e0 != (void *)0x0) {
    free(DAT_001487e0);
    DAT_001487e0 = (void *)0x0;
  }
  *(uint *)(param_1 + 0x1c) = uVar15;
  _ZN10libJiritsu11CFileAccess5closeEv(auStack_838);
  return uVar8;
}


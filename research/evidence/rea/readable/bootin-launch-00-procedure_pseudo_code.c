// Decompiled pseudocode, not original source.
// REA Evidence: ev_2163673152ffede8b9b70d71f6f902a3ce1b7307ea4e296036d8795d8704189b
// Source record: research/evidence/rea/bootin-launch/00-procedure_pseudo_code.json

undefined8 FUN_00402b90(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  ssize_t sVar4;
  char *pcVar5;
  size_t sVar6;
  char *pcVar7;
  undefined8 *puVar8;
  long lVar9;
  char local_528 [8];
  char *local_520;
  char local_518 [528];
  int local_308;
  char acStack_304 [4];
  undefined8 auStack_300 [63];
  undefined1 local_105;
  undefined8 auStack_100 [32];
  
  memset(&local_308,0,0x308);
  FUN_00401cf8(0,"Bb fork_imelf()");
  if ((DAT_004151c8 >> 3 & 1) != 0) {
    setenv("MALLOC_OPTIONS","2W",1);
  }
  if ((DAT_004151c8 >> 1 & 1) != 0) {
    FUN_00403520();
  }
  if (((DAT_004151e8 == (char *)0x0) || (*DAT_004151e8 == '\0')) ||
     (iVar1 = strcmp(DAT_004151e8," "), iVar1 == 0)) {
    iVar1 = FUN_00402198();
    if (iVar1 == 0x10000) {
      DAT_004151e8 = " target=resub ";
    }
    else if ((DAT_004151c8 >> 2 & 1) == 0) {
      iVar1 = FUN_00402b24();
      if (iVar1 == 0) {
        DAT_004151e8 = " ";
      }
      else {
        DAT_004151e8 = " target=test ";
      }
    }
    else {
      DAT_004151e8 = " target=qemu ";
    }
  }
  FUN_00401cf8(2,"Bb cmdline");
  if ((DAT_004151c8 >> 6 & 1) != 0) {
    local_518[0] = '\0';
    memset(local_518 + 1,0,0x1ff);
    iVar1 = open("/setting/mode/preload",0);
    if (-1 < iVar1) {
      sVar4 = read(iVar1,local_518,0x200);
      close(iVar1);
      iVar1 = (int)sVar4;
      if (-1 < iVar1) {
        if (0x1ff < iVar1) {
          iVar1 = 0x1ff;
        }
        local_518[iVar1] = '\0';
        lVar9 = 0;
        do {
          iVar1 = (int)lVar9;
          if (lVar9 == 4) {
            FUN_00401cf8(2,local_518);
            goto LAB_00402cdc;
          }
          pcVar5 = (&PTR_s__usr_tool_libLeakTracer_so_00403f08)[lVar9 * 2];
          sVar6 = strlen(pcVar5);
          iVar3 = strncmp(pcVar5,local_518,sVar6);
          lVar9 = lVar9 + 1;
        } while (iVar3 != 0);
        snprintf(local_518,0x20b,"LD_PRELOAD=%s",pcVar5);
        FUN_00401cf8(2,local_518);
        setenv("LD_PRELOAD",pcVar5,1);
        pcVar5 = (&PTR_DAT_00403f10)[(long)iVar1 * 2];
        local_520 = (char *)0x0;
        builtin_strncpy(local_528," \t\r\n",5);
        strncpy(acStack_304,pcVar5,0x200);
        sVar6 = strlen(pcVar5);
        if (0x1ff < sVar6) {
          local_105 = 0;
        }
        pcVar5 = acStack_304;
        while ((local_308 < 0x10 &&
               (pcVar7 = strtok_r(pcVar5,local_528,&local_520), pcVar5 = pcVar7,
               pcVar7 != (char *)0x0))) {
          for (; *pcVar5 != '\0'; pcVar5 = pcVar5 + 1) {
            if (*pcVar5 == '=') {
              if (pcVar5 != pcVar7) {
                *pcVar5 = '\0';
                auStack_300[((long)local_308 + 0x20) * 2] = pcVar7;
                auStack_300[((long)local_308 + 0x20) * 2 + 1] = pcVar5 + 1;
                local_308 = local_308 + 1;
              }
              break;
            }
          }
          pcVar5 = (char *)0x0;
        }
        iVar3 = local_308;
        puVar8 = auStack_100;
        for (iVar1 = 0; iVar1 < iVar3; iVar1 = iVar1 + 1) {
          setenv((char *)*puVar8,(char *)puVar8[1],1);
          puVar8 = puVar8 + 2;
        }
      }
    }
  }
LAB_00402cdc:
  pcVar5 = &DAT_004151fc;
  if ((DAT_004151c8 >> 5 & 1) != 0) {
    pcVar5 = (char *)FUN_0040205c(&DAT_004151fc,0x400);
  }
  if ((DAT_004151c8 >> 4 & 1) != 0) {
    pcVar5 = strncat(pcVar5,"gdb --args ",(long)(0x4155fc - (int)pcVar5));
  }
  pcVar5 = strncat(pcVar5,"/usr/bin/im.elf ",(long)(0x4155fc - (int)pcVar5));
  pcVar5 = strncat(pcVar5,"debug ",(long)(0x4155fc - (int)pcVar5));
  pcVar5 = strncat(pcVar5,PTR_DAT_004151e0,(long)(0x4155fc - (int)pcVar5));
  pcVar5 = strncat(pcVar5,DAT_004151e8,(long)(0x4155fc - (int)pcVar5));
  pcVar5 = strncat(pcVar5,PTR_DAT_004151d8,(long)(0x4155fc - (int)pcVar5));
  iVar1 = FUN_00402a3c();
  if (iVar1 != 0) {
    pcVar5 = strncat(pcVar5,"cho ",(long)(0x4155fc - (int)pcVar5));
  }
  iVar1 = FUN_00402abc();
  if (iVar1 != 0) {
    pcVar5 = strncat(pcVar5,"cipa ",(long)(0x4155fc - (int)pcVar5));
  }
  iVar1 = FUN_00402a7c();
  if (iVar1 != 0) {
    pcVar5 = strncat(pcVar5,"usbj ",(long)(0x4155fc - (int)pcVar5));
  }
  iVar1 = FUN_00402af8();
  if (iVar1 != 0) {
    pcVar5 = strncat(pcVar5,"dummy_susres ",(long)(0x4155fc - (int)pcVar5));
  }
  pcVar5 = strncat(pcVar5,"lazy-global ",(long)(0x4155fc - (int)pcVar5));
  if ((DAT_004151c8 >> 4 & 1) == 0) {
    strncat(pcVar5,"& ",(long)(0x4155fc - (int)pcVar5));
  }
  FUN_00401cf8(2,"Bb enable Ctrl-C");
  uVar2 = fcntl(0,3,0);
  fcntl(0,4,(ulong)(uVar2 & 0xfffff7ff));
  FUN_00401cf8(2,"Bb system()");
  FUN_00401aa4(&DAT_004151fc);
  FUN_00401cf8(2,"Bb clearing env");
  if ((DAT_004151c8 >> 4 & 1) != 0) {
    daemon(0,0);
  }
  if ((DAT_004151c8 >> 1 & 1) != 0) {
    FUN_0040362c();
  }
  iVar3 = local_308;
  puVar8 = auStack_100;
  for (iVar1 = 0; iVar1 < iVar3; iVar1 = iVar1 + 1) {
    unsetenv((char *)*puVar8);
    puVar8 = puVar8 + 2;
  }
  if ((DAT_004151c8 >> 6 & 1) != 0) {
    unsetenv("LD_PRELOAD");
  }
  if ((DAT_004151c8 >> 3 & 1) != 0) {
    unsetenv("MALLOC_OPTIONS");
  }
  FUN_00401cf8(1,"Bb fork_imelf()");
  return 0;
}


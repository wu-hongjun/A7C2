// Decompiled pseudocode, not original source.
// REA Evidence: ev_8e65d6fe9177deb7ac2497ef48843d55020d68d4bbc9859741b97281eeb5a18e
// Source record: research/evidence/rea/jiritsu-exact/04-procedure_pseudo_code.json

int _ZN10libJiritsu11CFileAccess11convertPathEPKcS2_Pci
              (char *param_1,char *param_2,char *param_3,int param_4)

{
  char *pcVar1;
  int iVar2;
  size_t sVar3;
  
  iVar2 = strncmp(param_2,"/mnt",4);
  if (iVar2 == 0) {
    sVar3 = strlen(param_1);
    pcVar1 = "/";
    if (param_1[sVar3 - 1] == '\\' || param_1[sVar3 - 1] == '/') {
      pcVar1 = "";
    }
    iVar2 = snprintf(param_3,(long)param_4,"%s%s%s",param_1,pcVar1,param_2 + 5);
    return iVar2;
  }
  return iVar2;
}


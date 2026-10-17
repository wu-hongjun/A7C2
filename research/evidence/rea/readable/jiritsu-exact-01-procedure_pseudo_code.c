// Decompiled pseudocode, not original source.
// REA Evidence: ev_3b863799a6ad3cc732e2c621bef960c072be5c7f0b1dab0eb2af4a6d9d37d2ff
// Source record: research/evidence/rea/jiritsu-exact/01-procedure_pseudo_code.json

char * get_access_filename(byte param_1,char *param_2,char *param_3,int param_4)

{
  int iVar1;
  size_t __n;
  char *__format;
  char *__s2;
  size_t __n_00;
  
  __n_00 = (size_t)param_4;
  memset(param_3,0,__n_00);
  __s2 = DAT_001487e0;
  if (param_1 == 2) {
    __s2 = "/tmp/jiritsu/libJit.so";
  }
  else if (param_1 < 3) {
    if (param_1 == 0) {
      __s2 = "/tmp/jiritsu/tmmc.jit";
    }
    else {
      if (param_1 != 1) {
        return param_3;
      }
      __s2 = "/tmp/jiritsu/DIAC.csv";
    }
  }
  else if (param_1 == 4) {
    __s2 = "/tmp/jiritsu/history.txt";
  }
  else if (param_1 < 4) {
    __s2 = "/tmp/jiritsu/jit.log";
  }
  else {
    if (param_1 != 0xff) {
      return param_3;
    }
    if (DAT_001487e0 == (char *)0x0) {
      return param_3;
    }
    __n = strlen(param_2);
    iVar1 = strncmp(param_2,__s2,__n);
    if (iVar1 != 0) {
      if (*__s2 == '/') {
        __format = "%s%s";
      }
      else {
        __format = "%s/%s";
      }
      snprintf(param_3,__n_00,__format,param_2,__s2);
      return param_3;
    }
  }
  snprintf(param_3,__n_00,"%s",__s2);
  return param_3;
}


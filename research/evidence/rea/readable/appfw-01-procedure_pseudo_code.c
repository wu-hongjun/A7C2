// Decompiled pseudocode, not original source.
// REA Evidence: ev_6ae78afdc73922acb103e7105bec02b8bd35669450f2b09185460d8a7d000622
// Source record: research/evidence/rea/appfw/01-procedure_pseudo_code.json

char FUN_03528510(int param_1,int param_2,undefined4 *param_3,undefined4 *param_4)

{
  char cVar1;
  int iVar2;
  undefined4 local_10 [4];
  
  *param_3 = 0;
  *param_4 = 0;
  if ((param_2 == 0) || (param_1 < 3)) {
    return '\x01';
  }
  if (param_1 < 0x13) {
    cVar1 = FUN_03121228(param_1 + -3,local_10);
    if ((cVar1 != '\0') && (cVar1 = FUN_031210c8(local_10), cVar1 != '\0')) {
      *param_3 = DAT_09208708._4_4_;
      *param_4 = local_10[0];
      return cVar1;
    }
  }
  else {
    FUN_037e9310("assert at %s:%d","sequence/sequence_select_lut.cpp",0x41);
    FUN_037e9310("function:%s",
                 "static bool model::sequence_select_lut::send_lut(model::setting_select_lut_t, model::is_exist_user_lut_t, uint32_t*, uint32_t*)"
                );
    FUN_037804c8("assert at %s:%d","sequence/sequence_select_lut.cpp",0x41);
    FUN_037804c8("function:%s",
                 "static bool model::sequence_select_lut::send_lut(model::setting_select_lut_t, model::is_exist_user_lut_t, uint32_t*, uint32_t*)"
                );
    printf("assert at %s:%d\n","sequence/sequence_select_lut.cpp",0x41);
    printf("function:%s\n",
           "static bool model::sequence_select_lut::send_lut(model::setting_select_lut_t, model::is_exist_user_lut_t, uint32_t*, uint32_t*)"
          );
    iVar2 = usleep(100000);
    FUN_036aa4b4(iVar2);
  }
  return '\0';
}


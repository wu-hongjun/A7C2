// Decompiled pseudocode, not original source.
// REA Evidence: ev_5c944520e9ca3fac5127a2d7065e36c4718747817c6f319a7a856bf43810ca2e
// Source record: research/evidence/rea/avbbc-encoder-transport/04-procedure_pseudo_code.json

void _ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj
               (long param_1,ulong param_2,undefined2 *param_3,undefined4 param_4)

{
  undefined2 uVar1;
  undefined2 uVar2;
  bool bVar3;
  bool bVar4;
  char cVar5;
  uint uVar6;
  undefined4 uVar7;
  ulong uVar8;
  undefined8 uVar9;
  uint uVar10;
  undefined1 uVar11;
  ulong uVar12;
  undefined1 auStack_1a0 [8];
  undefined1 auStack_198 [408];
  
  _ZN16DataflowInfraMsgC1Ev(auStack_198);
  _ZN19DataflowInfraSenderC1Ev(auStack_1a0);
  uVar1 = *param_3;
  uVar2 = param_3[1];
  uVar10 = (uint)param_2;
  uVar11 = 0;
  if (uVar10 == 0x80c70047) {
    uVar11 = (undefined1)param_3[0xd];
LAB_00239220:
    uVar8 = _ZN16DataflowInfraMsg12SetGlobalMsgEPvt(auStack_198,param_3,param_4);
    uVar12 = uVar8 & 0xffffffff;
    if ((int)uVar8 == 0) {
      uVar7 = _ZN24ExcludionEnvironmentUtil16ConvertSetSrcQidEjj
                        (*(undefined4 *)(param_1 + 8),param_2 & 0xffffffff);
      _ZN16DataflowInfraMsg8SetSrcIdEj(auStack_198,uVar7);
      if (uVar10 == 0x8055068a) {
        if (*(int *)(param_1 + 8) != -0x7faafee1) {
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0xe5,
                       "[InfBase][InfraAvbbcMsgManager](Self) OsalSndMsg 0x%x to 0x%x.",
                       *(int *)(param_1 + 8));
          }
          _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                    (*(undefined4 *)(param_1 + 0x18),0x201e808,*(undefined4 *)(param_1 + 8),uVar2,
                     0xe6,0);
        }
        uVar7 = _ZN24ExcludionEnvironmentUtil18ConvertOsalSendQidEj(*(undefined4 *)(param_1 + 8));
        uVar8 = _ZN19DataflowInfraSender9SendASyncEjP16DataflowInfraMsg
                          (auStack_1a0,uVar7,auStack_198);
        uVar12 = uVar8 & 0xffffffff;
        if ((int)uVar8 == 0) goto LAB_002394fc;
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,6,3,0xeb,"[InfraAvbbcMsgManager] SendASync err = 0x%x",uVar12);
        }
        uVar9 = 0xec;
      }
      else {
        cVar5 = _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                          (*(undefined8 *)(param_1 + 0x10),*(undefined4 *)(param_1 + 8),
                           param_2 & 0xffffffff,9,param_3,param_4);
        if (cVar5 == '\0') goto LAB_002394fc;
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,7,3,0xf5,"[InfBase][InfraAvbbcMsgManager] OsalSndMsg 0x%x to 0x%x.",
                     *(undefined4 *)(param_1 + 8),param_2 & 0xffffffff);
        }
        _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                  (*(undefined4 *)(param_1 + 0x18),0x201e808,param_2 & 0xffffffff,uVar2,uVar1,uVar11
                  );
        uVar7 = _ZN24ExcludionEnvironmentUtil18ConvertOsalSendQidEj(param_2 & 0xffffffff);
        uVar8 = _ZN19DataflowInfraSender9SendASyncEjP16DataflowInfraMsg
                          (auStack_1a0,uVar7,auStack_198);
        uVar12 = uVar8 & 0xffffffff;
        if ((int)uVar8 == 0) goto LAB_002394fc;
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,6,3,0xfe,"[InfraAvbbcMsgManager] SendASync err = 0x%x",uVar12);
        }
        uVar9 = 0xff;
      }
    }
    else {
      if (0 < avbizPrintLevel) {
        _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                  (0x55,6,3,0x104,"[InfraAvbbcMsgManager] SetGlobalMsg err = 0x%x",uVar12);
      }
      uVar9 = 0x105;
    }
LAB_0023954c:
    uVar10 = 0xe8a6;
  }
  else {
    if (0x80c70047 < uVar10) {
      if (uVar10 != 0x90560013) {
        if (uVar10 < 0x90560014) {
          if (uVar10 != 0x8283075a) {
            if (uVar10 < 0x8283075b) {
              if (uVar10 == 0x827e073f) goto LAB_00239368;
              if (uVar10 < 0x827e073f) goto LAB_0023925c;
              bVar3 = 5 < uVar10 + 0x7d7cf8ae;
              bVar4 = uVar10 + 0x7d7cf8ae == 6;
            }
            else {
              if (uVar10 == 0x90060197) goto LAB_00239220;
              if (uVar10 < 0x90060198) {
                uVar6 = 0x82830760;
                goto LAB_002392d0;
              }
              bVar3 = uVar10 != 0x9056000f;
              bVar4 = uVar10 == 0x90560010;
            }
            if (bVar3 && !bVar4) goto LAB_0023925c;
          }
        }
        else if (uVar10 != 0x90590818) {
          if (uVar10 < 0x90590819) {
            if ((uVar10 != 0x9058003f) && (uVar10 != 0x9059000e)) {
              uVar6 = 0x905601b3;
LAB_002392d0:
              if (uVar10 != uVar6) goto LAB_0023925c;
            }
          }
          else {
            if (uVar10 == 0x9061003c) goto LAB_00239220;
            if (uVar10 < 0x9061003d) {
              uVar6 = 0x905a0048;
              goto LAB_002392d0;
            }
            if (uVar10 != 0x90b201aa) {
              uVar6 = 0x92840747;
              goto LAB_00239218;
            }
          }
        }
      }
      goto LAB_00239368;
    }
    if (uVar10 == 0x805502f2) goto LAB_00239220;
    if (0x805502f2 < uVar10) {
      if (uVar10 == 0x8057026d) goto LAB_00239220;
      if (uVar10 < 0x8057026e) {
        if (uVar10 == 0x8055068a) goto LAB_00239220;
        if (uVar10 != 0x805700f9) {
          uVar6 = 0x80550689;
          goto LAB_002392d0;
        }
      }
      else if (uVar10 != 0x8057071f) {
        if (uVar10 < 0x80570720) {
          uVar6 = 0x8057060a;
          goto LAB_002392d0;
        }
        if (uVar10 != 0x805720f9) {
          uVar6 = 0x8061003c;
          goto LAB_00239218;
        }
      }
LAB_00239368:
      uVar8 = _ZN16DataflowInfraMsg14SetDataflowMsgEPvt(auStack_198,param_3,param_4);
      uVar12 = uVar8 & 0xffffffff;
      if ((int)uVar8 == 0) {
        _ZN16DataflowInfraMsg10SetReplyIdEj(auStack_198,*(undefined4 *)(param_1 + 8));
        uVar7 = _ZN24ExcludionEnvironmentUtil16ConvertSetSrcQidEjj
                          (*(undefined4 *)(param_1 + 8),param_2 & 0xffffffff);
        _ZN16DataflowInfraMsg8SetSrcIdEj(auStack_198,uVar7);
        if (uVar10 == 0x80550689) {
          if (*(int *)(param_1 + 8) != -0x7faafee1) {
            if (0 < avbizPrintLevel) {
              _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                        (0x55,7,3,0x96,
                         "[InfBase][InfraAvbbcMsgManager](Self) OsalSndMsg 0x%x to 0x%x.",
                         *(int *)(param_1 + 8));
            }
            _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                      (*(undefined4 *)(param_1 + 0x18),0x201e808,*(undefined4 *)(param_1 + 8),uVar2,
                       0,0);
          }
          uVar7 = _ZN24ExcludionEnvironmentUtil18ConvertOsalSendQidEj(*(undefined4 *)(param_1 + 8));
          uVar8 = _ZN19DataflowInfraSender9SendASyncEjP16DataflowInfraMsg
                            (auStack_1a0,uVar7,auStack_198);
          uVar12 = uVar8 & 0xffffffff;
          if ((int)uVar8 == 0) goto LAB_002394fc;
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,6,3,0x9e,"[InfraAvbbcMsgManager] SendASync err = 0x%x",uVar12);
          }
          uVar9 = 0x9f;
        }
        else {
          cVar5 = _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                            (*(undefined8 *)(param_1 + 0x10),*(undefined4 *)(param_1 + 8),
                             param_2 & 0xffffffff,9,param_3,param_4);
          if (cVar5 == '\0') goto LAB_002394fc;
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0xa7,"[InfBase][InfraAvbbcMsgManager] OsalSndMsg 0x%x to 0x%x.",
                       *(undefined4 *)(param_1 + 8),param_2 & 0xffffffff);
          }
          _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                    (*(undefined4 *)(param_1 + 0x18),0x201e807,param_2 & 0xffffffff,uVar1,uVar2,0);
          uVar7 = _ZN24ExcludionEnvironmentUtil18ConvertOsalSendQidEj(param_2 & 0xffffffff);
          uVar8 = _ZN19DataflowInfraSender9SendASyncEjP16DataflowInfraMsg
                            (auStack_1a0,uVar7,auStack_198);
          uVar12 = uVar8 & 0xffffffff;
          if ((int)uVar8 == 0) goto LAB_002394fc;
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,6,3,0xb0,"[InfraAvbbcMsgManager] SendASync err = 0x%x",uVar12);
          }
          uVar9 = 0xb1;
        }
      }
      else {
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,6,3,0xb6,"[InfraAvbbcMsgManager] SetDataflowMsg err = 0x%x",uVar12);
        }
        uVar9 = 0xb7;
      }
      goto LAB_0023954c;
    }
    if (uVar10 == 0x80050395) goto LAB_00239220;
    if (uVar10 < 0x80050396) {
      if (0x5503de < uVar10) {
        if (0x5503e0 < uVar10) {
          uVar6 = 0x61003c;
LAB_00239218:
          if (uVar10 != uVar6) goto LAB_0023925c;
        }
        goto LAB_00239220;
      }
    }
    else if ((uVar10 == 0x8055001f) || ((0x8055001e < uVar10 && (uVar10 + 0x7faafee2 < 6))))
    goto LAB_00239220;
LAB_0023925c:
    if ((((uint)((param_2 & 0xffffffff) >> 0x10) & 0xfff) < 0x3b) &&
       ((1L << ((param_2 & 0xffffffff) >> 0x10 & 0x3f) & 0x62cc00000000000U) != 0)) {
      uVar8 = _ZN16DataflowInfraMsg12SetGlobalMsgEPvt(auStack_198,param_3,param_4);
      uVar12 = uVar8 & 0xffffffff;
      if ((int)uVar8 == 0) {
        uVar7 = _ZN24ExcludionEnvironmentUtil16ConvertSetSrcQidEjj
                          (*(undefined4 *)(param_1 + 8),param_2 & 0xffffffff);
        _ZN16DataflowInfraMsg8SetSrcIdEj(auStack_198,uVar7);
        cVar5 = _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                          (*(undefined8 *)(param_1 + 0x10),*(undefined4 *)(param_1 + 8),
                           param_2 & 0xffffffff,9,param_3,param_4);
        if (cVar5 == '\0') goto LAB_002394fc;
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,7,3,0x122,"[InfBase][InfraAvbbcMsgManager] OsalSndMsg 0x%x to 0x%x.",
                     *(undefined4 *)(param_1 + 8),param_2 & 0xffffffff);
        }
        _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                  (*(undefined4 *)(param_1 + 0x18),0x201e808,param_2 & 0xffffffff,uVar2,uVar1,0x123)
        ;
        uVar7 = _ZN24ExcludionEnvironmentUtil18ConvertOsalSendQidEj(param_2 & 0xffffffff);
        uVar8 = _ZN19DataflowInfraSender9SendASyncEjP16DataflowInfraMsg
                          (auStack_1a0,uVar7,auStack_198);
        uVar12 = uVar8 & 0xffffffff;
        if ((int)uVar8 == 0) goto LAB_002394fc;
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,6,3,0x129,"[InfraAvbbcMsgManager] SendASync err = 0x%x",uVar12);
        }
        uVar9 = 0x12a;
      }
      else {
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,6,3,0x12d,"[InfraAvbbcMsgManager] SetGlobalMsg err = 0x%x",uVar12);
        }
        uVar9 = 0x12e;
      }
      goto LAB_0023954c;
    }
    if (0 < avbizPrintLevel) {
      _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                (0x55,7,3,0x133,
                 "[InfBase][InfraAvbbcMsgManager] ERR!! osalId Not Match  osalId = 0x%x",
                 param_2 & 0xffffffff);
    }
    uVar12 = param_2 & 0xffffffff;
    uVar9 = 0x134;
    uVar10 = 0xe8a7;
  }
  _ZN13InfraAvbbcLog7SetLog0Ejjjjt
            (*(undefined4 *)(param_1 + 0x18),uVar10 | 0x2010000,uVar9,uVar12,0);
LAB_002394fc:
  _ZN19DataflowInfraSenderD1Ev(auStack_1a0);
  _ZN16DataflowInfraMsgD1Ev(auStack_198);
  return;
}


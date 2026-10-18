// Decompiled pseudocode, not original source.
// REA Evidence: ev_32b5a491f34dfb99bb2237166ce13a336fc6b9e5ba976da2176e336323671c4a
// Source record: research/evidence/rea/heif-dispatch-binding/01-procedure_pseudo_code.json

void _ZN20InfraAvbbcMsgManager10OsalRcvMsgEP11IInfraAvbbc(long param_1,long *param_2)

{
  uint uVar1;
  bool bVar2;
  char cVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  code *pcVar11;
  char cVar12;
  undefined1 local_148;
  byte local_147;
  undefined2 local_146;
  uint local_144;
  undefined1 auStack_140 [16];
  undefined1 auStack_130 [48];
  undefined4 local_100;
  undefined4 uStack_f8;
  ushort local_de;
  char local_dc;
  undefined1 local_d9;
  
  _ZN19BizDataflowInfraMsgC2Ej(auStack_130,*(undefined4 *)(param_1 + 8));
  _ZN24BizDataflowInfraReceiverC1Ej(auStack_140,*(undefined4 *)(param_1 + 8));
  cVar12 = *(char *)(param_1 + 0x1c);
  local_146 = 0;
  if (cVar12 != '\0') {
    _ZN24BizDataflowInfraReceiver12ReceiveASyncEjP19BizDataflowInfraMsgj
              (auStack_140,*(undefined4 *)(param_1 + 8),auStack_130,0xffffffff);
    iVar5 = _ZN19BizDataflowInfraMsg10GetMsgTypeEv(auStack_130);
    _ZN19BizDataflowInfraMsg8GetSrcIdEPj(auStack_130,&local_144);
    uVar1 = local_144 >> 0x10 & 0xfff;
    uVar4 = _ZN19BizDataflowInfraMsg13GetTopMsgSizeEv(auStack_130);
    while (iVar6 = _ZN19BizDataflowInfraMsg6GetMsgEPvt(auStack_130,&local_100,0x100),
          uVar7 = local_100, iVar6 != -0x1b) {
      if (iVar5 == 1) {
        if ((local_100 & 0xffff) == 0xffff) {
          if ((uint)local_100._2_2_ == (local_100 & 0xffff)) {
            *(undefined1 *)(param_1 + 0x1c) = 0;
            break;
          }
LAB_00239fc8:
          _ZN11ObjAvbbcCmn13ConvertResMidEttPtPh
                    (*(undefined8 *)(param_1 + 0x10),(undefined2)local_100,local_100._2_2_,
                     &local_146,&local_148);
          _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                    (*(undefined8 *)(param_1 + 0x10),local_144,*(undefined4 *)(param_1 + 8),10,
                     &local_100,uVar4);
        }
        else if ((local_100 & 0xffff) == 0x2003) {
          if (local_100._2_2_ != 0x8201) goto LAB_00239fc8;
          _ZN11ObjAvbbcCmn13ConvertResMidEttPtPh
                    (*(undefined8 *)(param_1 + 0x10),(undefined2)local_100,uStack_f8._2_2_,
                     &local_146,&local_148);
          _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                    (*(undefined8 *)(param_1 + 0x10),local_144,*(undefined4 *)(param_1 + 8),10,
                     &local_100,uVar4);
          if ((uStack_f8._2_2_ & 0xffef) == 10) {
            cVar12 = '\0';
          }
          cVar3 = _ZN20InfraAvbbcMsgManager13NeedSetEncUcsEt(param_1);
          if (cVar3 != '\0') {
            _ZN6UcsMgr11getInstanceEv();
            iVar6 = _ZN6UcsMgr11GetUcsStateEv();
            if (iVar6 == 0x1f) {
              local_d9 = 2;
            }
            else if (iVar6 == 0x21) {
              local_d9 = 3;
            }
            else {
              local_d9 = iVar6 == 10;
            }
          }
          if (local_144 == 0x805720f9) {
            _ZN6UcsMgr11getInstanceEv();
            iVar6 = _ZN6UcsMgr11GetUcsStateEv();
            bVar2 = iVar6 != 0xf;
          }
          else {
            if (local_144 != 0x8057071f) {
              local_147 = 0;
              iVar6 = _ZN11AvbbcBackup9Read1ByteEjPh(0x550007,&local_147);
              if ((iVar6 != 0) && (0 < avbizPrintLevel)) {
                _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                          (0x55,7,3,0x1f8,
                           "[InfBase]InfraAvbbcMsgManager::OsalMkPublicQid OK id:0x%x",0x550007);
              }
              if (local_147 - 4 < 3) {
                local_de = 0;
              }
              goto LAB_00239e84;
            }
            bVar2 = local_dc == '\x13';
          }
          local_de = (ushort)bVar2;
        }
        else {
          if (((local_100 & 0xffff) != 8) || (local_100._2_2_ != 0x8701)) goto LAB_00239fc8;
          local_148 = 0;
          local_146 = 0x9001;
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0x210,
                       "[InfBase][InfraAvbbcMsgManager] OsalRcvMsg AVBBC Response, 0x%x from 0x%x. MID 0x%x"
                       ,*(undefined4 *)(param_1 + 8),local_144,0x9001);
          }
          _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                    (*(undefined4 *)(param_1 + 0x18),0x201e80d,local_144,0,0,0);
        }
LAB_00239e84:
        if (0 < avbizPrintLevel) {
          _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                    (0x55,7,3,0x21b,
                     "[InfBase][InfraAvbbcMsgManager] OsalRcvMsg MSG_TYPE_DATAFLOW, 0x%x from 0x%x. CATID 0x%x, MID 0x%x ConvMID %#x"
                     ,*(undefined4 *)(param_1 + 8),local_144,(undefined2)local_100,local_100 >> 0x10
                     ,local_146);
        }
        if (cVar12 != '\0') {
          uVar7 = 0xe80a;
          uVar10 = local_100._2_2_;
          uVar9 = (undefined2)local_100;
          goto LAB_0023a04c;
        }
LAB_00239e9c:
        pcVar11 = *(code **)(*param_2 + 0x38);
LAB_00239eac:
        (*pcVar11)(param_2,&local_100,local_146,local_148);
      }
      else {
        if (local_144 == 0x61003c) {
          _ZN11ObjAvbbcCmn21ConvertResMidFromMmgrEtPtPh
                    (*(undefined8 *)(param_1 + 0x10),(undefined2)local_100,&local_146,&local_148);
          _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                    (*(undefined8 *)(param_1 + 0x10),local_144,*(undefined4 *)(param_1 + 8),10,
                     &local_100,0x100);
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0x22e,
                       "[InfBase][InfraAvbbcMsgManager] OsalRcvMsg OSAL_MSGQ_GID_DMM_CTRL, 0x%x from 0x%x. MID 0x%x"
                       ,*(undefined4 *)(param_1 + 8),local_144,(undefined2)local_100);
          }
          uVar10 = 0;
          uVar7 = 0xe80b;
          uVar9 = (undefined2)local_100;
LAB_0023a04c:
          _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                    (*(undefined4 *)(param_1 + 0x18),uVar7 | 0x2010000,local_144,uVar9,uVar10,0);
          goto LAB_00239e9c;
        }
        if (local_144 == 0x80050395) {
          _ZN11ObjAvbbcCmn13ConvertResMidEttPtPh
                    (*(undefined8 *)(param_1 + 0x10),(undefined2)local_100,local_100._2_2_,
                     &local_146,&local_148);
          _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                    (*(undefined8 *)(param_1 + 0x10),local_144,*(undefined4 *)(param_1 + 8),10,
                     &local_100,0x100);
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0x23d,
                       "[InfBase][InfraAvbbcMsgManager] OsalRcvMsg OSAL_MSGQ_GID_CAMERA_RAW_DEVELOP, 0x%x from 0x%x. MID 0x%x"
                       ,*(undefined4 *)(param_1 + 8),local_144,local_100._2_2_);
          }
          uVar10 = 0;
          uVar7 = 0xe882;
          uVar9 = local_100._2_2_;
          goto LAB_0023a04c;
        }
        if (local_144 == 0x8055001f) {
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0x24a,
                       "[InfBase][InfraAvbbcMsgManager] ExecuteCommand to 0x%x from 0x%x",
                       *(undefined4 *)(param_1 + 8),local_100);
          }
          _ZN13InfraAvbbcLog7SetLog1Ejjjttt(*(undefined4 *)(param_1 + 0x18),0x1e80c,local_144,0,0,0)
          ;
          pcVar11 = *(code **)(*param_2 + 0x30);
          puVar8 = &uStack_f8;
LAB_0023a218:
          (*pcVar11)(param_2,puVar8,uVar7);
        }
        else {
          if (local_144 + 0x7faafee2 < 6 || local_144 == 0x805502f2) {
            local_148 = 0;
            local_146 = 0x9001;
            if (local_144 != 0x8055011f) {
              if (0 < avbizPrintLevel) {
                _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                          (0x55,7,3,0x260,
                           "[InfBase][InfraAvbbcMsgManager] OsalRcvMsg AVBBC Response, 0x%x from 0x%x. MID 0x%x"
                           ,*(undefined4 *)(param_1 + 8),local_144,0x9001);
              }
              uVar10 = 0;
              uVar9 = 0;
              uVar7 = 0xe80d;
              goto LAB_0023a04c;
            }
            goto LAB_00239e9c;
          }
          puVar8 = &local_100;
          if (local_144 == 8) {
            if (0 < avbizPrintLevel) {
              _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                        (0x55,7,3,0x268,"[InfBase][DummyMPR]ExecuteCommand. \n");
            }
            pcVar11 = *(code **)(*param_2 + 0x30);
            uVar7 = 0x805502f2;
            goto LAB_0023a218;
          }
          if (local_144 == 0x92840747) {
            if (0 < avbizPrintLevel) {
              _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                        (0x55,7,3,0x26c,"[InfBase]Receive from InfraBB. \n");
            }
            local_146 = local_100._2_2_;
            local_148 = 0;
            pcVar11 = *(code **)(*param_2 + 0x38);
            goto LAB_00239eac;
          }
          if ((uVar1 < 0x3b) && ((1L << ((ulong)uVar1 & 0x3f) & 0x62cc00000000000U) != 0)) {
            _ZN11ObjAvbbcCmn15CheckAndHookMsgEjjN10DefBizTest8MSG_TYPEEPvj
                      (*(undefined8 *)(param_1 + 0x10),local_144,*(undefined4 *)(param_1 + 8),10,
                       &local_100,0x100);
            if (0 < avbizPrintLevel) {
              _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                        (0x55,7,3,0x280,
                         "[InfBase][InfraAvbbcMsgManager] OsalRcvMsg PinCommand, 0x%x from 0x%x.",
                         *(undefined4 *)(param_1 + 8),local_144);
            }
            _ZN13InfraAvbbcLog7SetLog1Ejjjttt
                      (*(undefined4 *)(param_1 + 0x18),0x201e80e,local_144,0,0,0);
            pcVar11 = *(code **)(*param_2 + 0x40);
            uVar7 = local_144;
            goto LAB_0023a218;
          }
          if (0 < avbizPrintLevel) {
            _ZN17OBJAVBBCBASECLASS9BizPrintfEtiijPKcz
                      (0x55,7,3,0x286,
                       "[InfBase][InfraAvbbcMsgManager] ERR!! OsalId Not Much osalID = 0x%x",
                       local_144);
          }
          _ZN13InfraAvbbcLog7SetLog0Ejjjjt
                    (*(undefined4 *)(param_1 + 0x18),0x201e8a7,0x287,local_144,0);
        }
      }
    }
  }
  _ZN24BizDataflowInfraReceiverD2Ev(auStack_140);
  _ZN19BizDataflowInfraMsgD2Ev(auStack_130);
  return;
}


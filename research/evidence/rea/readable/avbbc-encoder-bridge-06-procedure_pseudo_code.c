// Decompiled pseudocode, not original source.
// REA Evidence: ev_86d623bef36238f0b7e7e0223a74ec6c34a3b0cd71fd39760ece4fe4fa30f1d4
// Source record: research/evidence/rea/avbbc-encoder-bridge/06-procedure_pseudo_code.json

undefined8 _ZN21ExecCmd_MC_EncodeHeif7ExecCmdEPKh(long param_1,undefined8 param_2)

{
  undefined8 local_60;
  undefined8 uStack_58;
  undefined8 local_50;
  undefined8 uStack_48;
  undefined8 local_40;
  undefined8 uStack_38;
  undefined8 local_30;
  undefined8 uStack_28;
  undefined8 local_20;
  undefined8 uStack_18;
  undefined8 local_10;
  undefined8 uStack_8;
  
  (**(code **)(**(long **)(param_1 + 0x18) + 0xb0))(*(long **)(param_1 + 0x18),0x104a);
  local_60 = 0;
  uStack_58 = 0;
  local_50 = 0;
  uStack_48 = 0;
  local_40 = 0;
  uStack_38 = 0;
  local_30 = 0;
  uStack_28 = 0;
  local_20 = 0;
  uStack_18 = 0;
  local_10 = 0;
  uStack_8 = 0;
  _ZN21ExecCmd_MC_EncodeHeif22MakeMcEncodeFrameParamEPKN17StillEncoderParam23RESIZEENC_PARAM_PAYLOADEPK18dmm_comp_open_unitPN2MC29MCODEC_STILL_ENC_ENCODE_FRAMEE
            (param_1,0,param_2,&local_60);
  _ZN13InfraAvbbcLog7SetLog2Ejjttttt(0x11,0x201004f,local_60 & 0xffff,local_60._2_2_,0,0,0);
  _ZN20InfraAvbbcMsgManager10OsalSndMsgEjPKhj
            (*(undefined8 *)(param_1 + 0x10),0x8057060a,&local_60,0x60);
  return 0;
}


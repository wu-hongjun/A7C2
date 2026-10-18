// Decompiled pseudocode, not original source.
// REA Evidence: ev_9e950c358e2e08b59752799069fc49350f003b4bdb55a45be25ba346905640c4
// Source record: research/evidence/rea/still-sink-virtual-r2/09-procedure_pseudo_code.json

void _ZN3mpr11stillrecord17fsys_callbackfuncEPviPK22FSYSWRITEDIRECTFILERES
               (undefined8 param_1,undefined4 param_2,long param_3)

{
  undefined2 local_28;
  undefined2 local_26;
  undefined1 local_24;
  undefined1 local_23;
  undefined1 local_22;
  undefined1 local_21;
  undefined8 local_20;
  undefined4 local_18;
  long local_10;
  code *local_8;
  
  local_20 = 0x351002120;
  local_18 = param_2;
  FUN_001df8d4(&local_20,0xc);
  local_8 = _ZN3mpr11stillrecord15_write_finishedEPv;
  local_10 = (ulong)(uint)(*(int *)(param_3 + 0x440) * *(int *)(param_3 + 0x444)) *
             (ulong)*(uint *)(param_3 + 0x42c);
  local_20 = param_1;
  local_18 = param_2;
  _ZN9execqueueIN3mpr11stillrecord22write_finished_input_tEE4execERNS3_6exec_tE
            (_ZN3mpr11stillrecord21write_finished_threadE,&local_20);
  local_28 = 0x2120;
  local_23 = 0;
  local_24 = 4;
  local_22 = 0;
  local_26 = 0x510f;
  local_21 = 0;
  FUN_001df8d4(&local_28,8);
  return;
}


// Decompiled pseudocode, not original source.
// REA Evidence: ev_2aa67e717a02dca8696be1cd266af68515ba7c52e26c4d5b18752bdc5d51a16e
// Source record: research/evidence/rea/sony-mpr-completion/03-procedure_pseudo_code.json

void _ZN9execqueueIN3mpr11stillrecord22write_finished_input_tEE7_threadEPv(long param_1)

{
  pthread_mutex_t *__mutex;
  code *pcVar1;
  long lVar2;

  __mutex = (pthread_mutex_t *)(param_1 + 0x58);
  while (*(char *)(param_1 + 0xbc) != '\0') {
    pthread_mutex_lock(__mutex);
    if (*(long *)(param_1 + 0x38) == *(long *)(param_1 + 0x18)) {
      pthread_cond_wait((pthread_cond_t *)(param_1 + 0x88),__mutex);
    }
    lVar2 = *(long *)(param_1 + 0x18);
    pthread_mutex_unlock(__mutex);
    if (*(__useconds_t *)(param_1 + 0xb8) != 0) {
      usleep(*(__useconds_t *)(param_1 + 0xb8));
    }
    pcVar1 = *(code **)(lVar2 + 0x18);
    if (pcVar1 != (code *)0x0) {
      (*pcVar1)(lVar2);
    }
    pthread_mutex_lock(__mutex);
    if (*(long *)(param_1 + 0x18) == *(long *)(param_1 + 0x28) + -0x20) {
      _ZdlPv(*(undefined8 *)(param_1 + 0x20));
      lVar2 = *(long *)(*(long *)(param_1 + 0x30) + 8);
      *(long *)(param_1 + 0x30) = *(long *)(param_1 + 0x30) + 8;
      *(long *)(param_1 + 0x20) = lVar2;
      *(long *)(param_1 + 0x28) = lVar2 + 0x200;
    }
    else {
      lVar2 = *(long *)(param_1 + 0x18) + 0x20;
    }
    *(long *)(param_1 + 0x18) = lVar2;
    pthread_mutex_unlock(__mutex);
  }
  return;
}

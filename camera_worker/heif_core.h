/* Internal C boundary for the separate HIF executable. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
int a7_load(const char *directory);
const char *a7_error(void);
void a7_cleanup(void);
int a7_input(void);
int a7_verify_snapshot(const unsigned char *data,unsigned long long size);
unsigned a7_width(void);
unsigned a7_height(void);
unsigned a7_quality(void);
const unsigned char *a7_exif(void);
unsigned a7_exif_size(void);
int a7_cancelled(void);
void a7_apply(float p[3]);
int a7_reserve(void);
int a7_publish(void);
int a7_published(void);
#ifdef __cplusplus
}
#endif

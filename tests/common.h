#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef RINOK
#define RINOK(x) { const int _result_ = (x); if (_result_ != 0) return _result_; }
#endif

void *memcpy(void *dst, const void *src, unsigned long sz);
void *memmove(void *dst, const void *src, unsigned long sz);
void *memset(void *dst, int ch, unsigned long sz);

void *_memcpy(void *dst, const void *src, unsigned long sz);
void *_memmove(void *dst, const void *src, unsigned long sz);
void *_memset(void *dst, int ch, unsigned long sz);

int eq(void *a, void *b, int sz);
void hexdump_mem(char *start, int bytes);

#endif

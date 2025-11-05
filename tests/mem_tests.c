#include "printf.h"
#include "common.h"

#define BUFSIZE	64*1024

char a[BUFSIZE] __attribute__((aligned(8)));
char b[BUFSIZE] __attribute__((aligned(8)));
char c[BUFSIZE] __attribute__((aligned(8)));

void init_a_b(unsigned long w)
{
    unsigned long *al = (void *)&a[0];
    unsigned long *bl = (void *)&b[0];

    for (int i=0; i < BUFSIZE/sizeof(unsigned long); i++)
        al[i] = bl[i] = w;
}

void init_c()
{
    char ch = 42;
    for (int i=0; i<BUFSIZE;i++)
        c[i] = ch++;
}

int memset_test_single(int offset, int ch, int sz)
{
    void *ret;
    if (ch != 255)
        init_a_b(-1);
    else
        init_a_b(0);

    memset(&a[offset], ch, sz);
    ret = _memset(&b[offset], ch, sz);

    if (eq(a,b,BUFSIZE) || ret != (void *)&b[offset]) {
        printf("FAILED!\nfailed: memset(buf+%d, %x, %d) returned %x\n", offset, ch, sz, ret);
        printf("Reference:\n");
        hexdump_mem(a, sz);
        printf("Got:\n");
        hexdump_mem(b, sz);
        return -1;
    }
}

int memset_test_comb(int start_off, int max_offset, int start_sz, int max_sz, int sz_step)
{
    int offset, sz, ch=42;

    for (offset=start_off; offset<=max_offset; offset++) {
        for (sz=start_sz; sz<=max_sz; sz+=sz_step) {
            RINOK(memset_test_single(offset, ch, sz));
            ch++;
        }
    }

    return 0;
}

int memcpy_test_single(int dst_offset, int src_offset, int sz)
{
    static unsigned long w = (unsigned long)0x4847464544434241;
    void *ret;

    init_a_b(w);
    w += (unsigned long)0x0101010101010101;

    memcpy(&a[dst_offset], &c[src_offset], sz);
    ret = _memcpy(&b[dst_offset], &c[src_offset], sz);

    if (eq(a,b,BUFSIZE) || ret != (void *)&b[dst_offset]) {
        printf("FAILED!\nfailed: memcpy(buf+%d, buf+%d, %d) returned %x\n", dst_offset, src_offset, sz, ret);
        printf("Reference:\n");
        hexdump_mem(a, sz);
        printf("Got:\n");
        hexdump_mem(b, sz);
        return -1;
    }
    return 0;
}

int memcpy_test_comb(int dst_offset_max, int src_offset_max, int sz)
{
    int dst_offset, src_offset;

    for (dst_offset=0; dst_offset<=dst_offset_max; dst_offset++) {
        for (src_offset=0; src_offset<=src_offset_max; src_offset++) {
            RINOK(memcpy_test_single(dst_offset,src_offset,sz));
        }
    }
    return 0;
}

int memmove_test_single(int dst_offset, int src_offset, int sz)
{
    static unsigned long w = (unsigned long)0x4847464544434241;
    void *ret;

    init_a_b(w);
    w += (unsigned long)0x0101010101010101;

    memmove(&a[32 + dst_offset], &a[32 + src_offset], sz);
    ret = _memmove(&b[32 + dst_offset], &b[32 + src_offset], sz);

    if (eq(a,b,BUFSIZE) || ret != (void *)&b[32 + dst_offset]) {
        printf("FAILED!\nfailed: memmove(buf+%d, buf+%d, %d) returned %x\n", dst_offset, src_offset, sz, ret);
        printf("Reference:\n");
        hexdump_mem(a, sz);
        printf("Got:\n");
        hexdump_mem(b, sz);
        return -1;
    }
    return 0;
}

int memmove_test_comb(int dst_offset_max, int src_offset_max, int sz)
{
    int dst_offset, src_offset;

    for (dst_offset=0; dst_offset<=dst_offset_max; dst_offset++) {
        for (src_offset=0; src_offset<=src_offset_max; src_offset++) {
            RINOK(memmove_test_single(dst_offset,src_offset,sz));
        }
    }
    return 0;
}

int memset_test()
{
#ifndef QUIET_MEMTEST
    printf("memset tests for 0..17 offset and sz=0..1031 (%d total)...\n", 18 * 1032);
#endif
    RINOK(memset_test_comb(0, 17, 0, 1031, 1));

    return 0;
}

int memcpy_test()
{
    int sz;
    init_c();

    for (sz=0;sz <= (sizeof(long)*(2 * 48 + 3) - 1); sz++) {
#ifndef QUIET_MEMTEST
        printf("memcpy tests for sz=%d (%d tests)...\n", sz, ((sizeof(long)*2+1)*((sizeof(long)*2+1))));
#endif
        RINOK(memcpy_test_comb(sizeof(long)*2+1, sizeof(long)*2+1, sz));
    }

    for (sz = BUFSIZE-sizeof(long)*31-7;sz <= BUFSIZE-sizeof(long)*31+17; sz++) {
#ifndef QUIET_MEMTEST
        printf("memcpy tests for sz=%d (%d tests)...\n", sz, ((sizeof(long)*2+1)*((sizeof(long)*2+1))));
#endif
        RINOK(memcpy_test_comb(sizeof(long)*2+1, sizeof(long)*2+1, sz));
    }

    return 0;
}

int memmove_test()
{
    int sz;
    init_c();

    for (sz=0;sz <= (sizeof(long)*(2 * 48 + 3) - 1); sz++) {
#ifndef QUIET_MEMTEST
        printf("memmove tests for sz=%d (%d tests)...\n", sz, ((sizeof(long)*2+1)*((sizeof(long)*2+1))));
#endif
        RINOK(memmove_test_comb(sizeof(long)*2+1, sizeof(long)*2+1, sz));
    }

    for (sz = BUFSIZE-sizeof(long)*31-7;sz <= BUFSIZE-sizeof(long)*31+17; sz++) {
#ifndef QUIET_MEMTEST
        printf("memmove tests for sz=%d (%d tests)...\n", sz, ((sizeof(long)*2+1)*((sizeof(long)*2+1))));
#endif
        RINOK(memmove_test_comb(sizeof(long)*2+1, sizeof(long)*2+1, sz));
    }

    return 0;
}

int main()
{
    RINOK(memset_test());
    RINOK(memcpy_test());
    RINOK(memmove_test());

    return 0;
}

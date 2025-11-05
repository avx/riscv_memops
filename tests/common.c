#include "printf.h"

int eq(void *a, void *b, int sz)
{
    unsigned long *la = a, *lb = b;

    if (sz % sizeof(long))
        printf("WARNING: %s: sz(%d) is not aligned to long\n", __func__, sz);

    for (int i=0; i < sz/sizeof(long); i++) {
        if (la[i] != lb[i]) {
            printf("cmp failed: (%x) != (%x)", la[i], lb[i]);
            return -1;
        }
    }

    return 0;
}

void hexdump_mem(char *start, int bytes)
{
    char *m = start;
    int n = bytes;
    char c;

    m = (void*)((unsigned long)start & (~15)) - 16;
    n = (bytes & (~15)) + 64;

    printf("\n");
    for (int i=0; i<n; i++) {
        if (i % 16 == 0)
            printf("0x%x:", m+i);

        c=' ';
        if (&m[i] == start)
            c='<';
        else if (&m[i] == &start[bytes])
            c='>';

        printf("%c%02x", c, (unsigned char)m[i]);

        if (i%16 == 15)
            printf("\n");
    }
    printf("\n");
}

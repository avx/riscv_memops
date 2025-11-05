#include "htif.h"
#include "printf.h"
#include "common.h"
#include "riscv.h"

#define BUFSIZE 32*1024*1024

typedef unsigned long uint64_t;
char buf[BUFSIZE] __attribute__((aligned(8)));

int memset_perf();
int memcpy_perf();
int memmove_perf();

int main()
{
    printf("\nmemset:\n\n");
    RINOK(memset_perf());

    printf("\nmemcpy:\n\n");
    RINOK(memcpy_perf());

    printf("\nmemmove:\n\n");
    RINOK(memmove_perf());

    printf("\n");
    return 0;
}


int memset_perf()
{
    int A, i;
    uint64_t start_inst, stop_inst;
    uint64_t start_cycles, stop_cycles;
    uint64_t inst, cycles = 0;
    unsigned long sz;
    int N;

    A=32;
    sz = A*1024*1024;

    cycles = 0;
    start_cycles=get_cycles();
    _memset(&buf[0], 0x7c1, sz);
    stop_cycles=get_cycles();
    cycles += (stop_cycles - start_cycles);

    printf("_memset(%dM) => %lu cycles, %d/100 bytes/cycles\n", \
           sz/1024/1024, cycles, (sz)*100/(cycles));

    cycles = 0;
    start_cycles=get_cycles();
    memset(&buf[0], 0x7c1, sz);
    stop_cycles=get_cycles();
    cycles += (stop_cycles - start_cycles);

    printf(" memset(%dM) => %lu cycles, %d/100 bytes/cycles\n", \
           sz/1024/1024, cycles, (sz)*100/(cycles));

    return 0;
}

int memcpy_perf()
{
    int A, i, j;
    uint64_t start_cycles, stop_cycles;
    uint64_t cycles = 0;
    unsigned long sz;
    int N;

    sz=31*1024*1024;

    for (j=0;j<8;j++) {
        cycles = 0;
        start_cycles=get_cycles();
        _memcpy(&buf[0+(j<4?j:0)], &buf[64+(j>=4?j:0)], sz);
        stop_cycles=get_cycles();
        if (stop_cycles > start_cycles)
            cycles += (stop_cycles - start_cycles);

        printf("_memcpy(&%d, &%d, %dM) => %lu cycles, %d/100 bytes/cycle\n", \
               (j<4?j:0), (j>=4?j:0), sz/1024/1024, \
               cycles, sz*100/cycles);

        cycles = 0;
        start_cycles=get_cycles();
        memcpy(&buf[0+(j<4?j:0)], &buf[64+(j>=4?j:0)], sz);
        stop_cycles=get_cycles();
        if (stop_cycles > start_cycles)
            cycles += (stop_cycles - start_cycles);

        printf(" memcpy(&%d, &%d, %dM) => %lu cycles, %d/100 bytes/cycle\n", \
               (j<4?j:0), (j>=4?j:0), sz/1024/1024, \
               cycles, sz*100/cycles);
    }

    return 0;
}

int memmove_perf()
{
    int A, i, j;
    uint64_t start_inst, stop_inst;
    uint64_t start_cycles, stop_cycles;
    uint64_t inst, cycles = 0;
    unsigned long sz;
    int N;

    A=31;

    for (j=0;j<16;j++) {
        sz = (A+1)*1024*1024;

        cycles = 0;
        start_cycles=get_cycles();
        _memmove(&buf[0+(j<8?j:0)], &buf[0+(j>=8?j:0)], sz);
        stop_cycles=get_cycles();
        if (stop_cycles > start_cycles)
            cycles += (stop_cycles - start_cycles);

        printf("_memmove(&%d, &%d, %dM) => %lu cycles, %d/100 bytes/cycle\n", \
               (j<8?j:0), (j>=8?j:0), sz/1024/1024, \
               cycles, sz*100/cycles);

        cycles = 0;
        start_cycles=get_cycles();
        memmove(&buf[0+(j<8?j:0)], &buf[0+(j>=8?j:0)], sz);
        stop_cycles=get_cycles();
        if (stop_cycles > start_cycles)
            cycles += (stop_cycles - start_cycles);

        printf(" memmove(&%d, &%d, %dM) => %lu cycles, %d/100 bytes/cycle\n", \
               (j<8?j:0), (j>=8?j:0), sz/1024/1024, \
               cycles, sz*100/cycles);
    }

    return 0;
}

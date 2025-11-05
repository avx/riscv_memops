#ifndef __RISCV_H__
#define __RISCV_H__

#if __riscv_xlen == 32
static unsigned long long get_ticks(void)
{
        unsigned long lo, hi, tmp;
        __asm__ __volatile__("1:\n"
                             "rdtimeh %0\n"
                             "rdtime %1\n"
                             "rdtimeh %2\n"
                             "bne %0, %2, 1b"
                             : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
        return ((unsigned long long)hi << 32) | lo;
}

static unsigned long long get_cycles(void)
{
        unsigned long lo, hi, tmp;
        __asm__ __volatile__("1:\n"
                             "rdcycleh %0\n"
                             "rdcycle %1\n"
                             "rdcycleh %2\n"
                             "bne %0, %2, 1b"
                             : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
        return ((unsigned long long)hi << 32) | lo;
}

#else
static unsigned long long get_ticks(void)
{
        unsigned long n;

        __asm__ __volatile__("rdtime %0" : "=r"(n));
        return n;
}

static unsigned long long get_insn(void)
{
        unsigned long n;

        __asm__ __volatile__("rdinstret %0" : "=r"(n));
        return n;
}

//static unsigned long long get_cycles(void)
static unsigned long long get_cycles(void)
{
        unsigned long n;

        __asm__ __volatile__("rdcycle %0" : "=r"(n));
        return n;
}


#endif

#endif


# risc-v memcpy, memset, memmove functions

Optimized scalar memcpy, memset, memmove functions written in RISC-V assembler.
Code is optimized for in-order multiple issue (superscalar) CPUs.
In order to comply with the compressed ISA specification x0-x15 GP registers
 are used.

Minimal supported risc-v ISA: rv32e for memset, rv32i for memcpy/memmove

## code size

| function | standard ISA (rv64/rv32) | compressed ISA (rv64/rv32) |
|:---------|------:|-------:|
| memset   |   280/268 |    178/172 |
| memcpy   |   408 |    258 |
| memmove  |   416 |    266 |

* numbers are in bytes for rv64

## performance

| arch |      mode | memset | memcpy | memmove |
|-----:|----------:|-------:|-------:|--------:|
| rv32 | unaligned |   3.76 |  0.61  |    0.61 |
| rv32 | aligned   |   3.76 |  1.82  |    1.82 |
| rv64 | unaligned |   7.52 |  1.23  |    1.23 |
| rv64 | aligned   |   7.52 |  3.65  |    3.65 |

* numbers are bytes per cycle (spike)
* for memcpy/memmove aligned/unaligned modes mean mutual alignments of XLEN size for src & dst buffers (memcpy, memmove)

## internal details

### memcpy

memcpy() for mutual unaligned buffers does load/stores of XLEN with logical shift and combine data.
Head and tail misalignments within XLEN size are handled by byte copies (lb/sb).
Parallelism of instructions factor > 2. Endianess supported: big, little.

hot spot for memcpy unaligned (rv64gc, little-endian):
```
    /-> 0107d4b3                srl     s1,a5,a6
    |   6598                    c.ld    a4,8(a1)
    |   01171433                sll     s0,a4,a7
    |   699c                    c.ld    a5,16(a1)
    |   8c45                    c.or    s0,s1
    |   010754b3                srl     s1,a4,a6
    |   e100                    c.sd    s0,0(a0)
    |   01179433                sll     s0,a5,a7
    |   8c45                    c.or    s0,s1
    |   05c1                    c.addi  a1,16
    |   e500                    c.sd    s0,8(a0)
    |   0541                    c.addi  a0,16
    \-- fed560e3                bltu    a0,a3,<_memcpy+0xda>
```

memcpy() for mutual aligned buffers does unrolled loop over load/stores of XLEN register size.
Parallelism of instructions factor is 4.

hot spot for memcpy aligned (rv64gc):
```
    /-> 6198                    c.ld    a4,0(a1)
    |   659c                    c.ld    a5,8(a1)
    |   6980                    c.ld    s0,16(a1)
    |   6d84                    c.ld    s1,24(a1)
    |   e118                    c.sd    a4,0(a0)
    |   e51c                    c.sd    a5,8(a0)
    |   e900                    c.sd    s0,16(a0)
    |   ed04                    c.sd    s1,24(a0)
    |   ....                    ....    .........
    |   71b8                    c.ld    a4,96(a1)
    |   75bc                    c.ld    a5,104(a1)
    |   79a0                    c.ld    s0,112(a1)
    |   7da4                    c.ld    s1,120(a1)
    |   f138                    c.sd    a4,96(a0)
    |   f53c                    c.sd    a5,104(a0)
    |   f920                    c.sd    s0,112(a0)
    |   fd24                    c.sd    s1,120(a0)
    |   95c6                    c.add   a1,a7
    |   9546                    c.add   a0,a7
    \-- fad56ee3                bltu    a0,a3,<_memcpy+0x42>
```

### memmove

memmove() does the same as memcpy() does with respect to fact that buffers may overlap.
Therefore there implemented direction of copying: straight and reverse

### memset

- Uses unrolled loop with 32 register stores of XLEN.
- Maximum parallelism of instructions factor is 32.
- Head and tail misalignments within XLEN size are handled by lb/sb (byte load/store)

hot spot for memset (rv64gc):
```
    /-> e38c                  c.sd    a1,0(a5)
    |   e78c                  c.sd    a1,8(a5)
    |   eb8c                  c.sd    a1,16(a5)
    |   ....                  ....    .........
    |   fbec                  c.sd    a1,240(a5)
    |   ffec                  c.sd    a1,248(a5)
    |   10078793              addi    a5,a5,256
    \-- fae7eee3              bltu    a5,a4,<_memset+0x5e>
```

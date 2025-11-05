    .section .text.startup
    .globl entry, func
entry:
    nop
.option push
.option norelax
    lla gp, __global_pointer$
.option pop
    # set up a stack for C
    lla sp, stack
    call c_start

    # say to spike that we have finished (see spike-cmds)
#    li sp, 0xdeaddead
#    j .

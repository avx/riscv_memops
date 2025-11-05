OUTPUT_ARCH("riscv")
ENTRY(entry)

MEMORY {
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 64M
}

PHDRS
{
    SEG_LOAD_TEXT PT_LOAD FLAGS(5);
    SEG_LOAD_DATA PT_LOAD FLAGS(6);
}

SECTIONS
{
  .text : {
    PROVIDE(stext = .);
    *(.text.startup)
    *(.text .text.*)
    . = ALIGN(8);
    *(.srodata .srodata.*)
    . = ALIGN(8);
    *(.rodata .rodata.*)
    PROVIDE(etext = .);
  } >RAM : SEG_LOAD_TEXT

  .htif (NOLOAD): {
    *(.htif)
  } >RAM : SEG_LOAD_DATA

  .data : {
    __global_pointer$ = . + 0x800;
    . = ALIGN(8);
    *(.sdata .sdata.*)
    . = ALIGN(8);
    *(.data .data.*)
    . = ALIGN(8);
  } >RAM

  .bss (NOLOAD) : {
    . = ALIGN(8);
    PROVIDE(bss_start = .);
    *(.sbss .sbss.*)
    . = ALIGN(8);
    *(.bss .bss.*)
    . = ALIGN(8);
    PROVIDE(bss_end = .);
    . = . + 8*1024;
    . = ALIGN(1024);
    PROVIDE(stack = .);
  } >RAM : NONE
}

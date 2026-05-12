ENTRY(start)

PHDRS
{
  text PT_LOAD FLAGS (READ|EXEC);
  data PT_LOAD FLAGS (READ|WRITE);
}

SECTIONS
{
    . = 0x100000;

    .multiboot : { KEEP(*(.multiboot)) } :text

    .text : {
        *(.text)
    } :text

    .rodata : {
        *(.rodata)
    } :text

    /* GDT placed after code/rodata so data PHDRS starts after text */
    .gdt : {
        __gdt_start = .;
        *(.gdt)
        __gdt_end = .;
    } :data

    .data : {
        *(.data)
    } :data

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    } :data

    end_of_kernel = .;

    .stack (NOLOAD) : {
        __stack_bottom = .;
        . = . + 0x4000;
        __stack_top = .;
    } :data

    /DISCARD/ : { *(.eh_frame) *(.comment) }
}
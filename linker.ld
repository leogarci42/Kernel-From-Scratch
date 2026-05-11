ENTRY(start)

SECTIONS
{
    . = 0x100000; 
    .multiboot : { 
        KEEP(*(.multiboot)) 
    }

    .text : {
        *(.text)
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }

    end_of_kernel = .;

    .stack (NOLOAD) : {
        __stack_bottom = .;
        . = . + 0x4000; 
        __stack_top = .;
    }

    /DISCARD/ : { 
        *(.eh_frame) 
        *(.comment)
    }
}
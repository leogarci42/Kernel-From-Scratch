ENTRY(start)

SECTIONS
{
    KERNEL_VMA = 0xC0000000;
    KERNEL_LMA = 0x00100000;

    . = KERNEL_LMA;
    __boot_phys_start = .;

    .multiboot : {
        KEEP(*(.multiboot))
    }

    .boot.text : {
        *(.boot.text)
    }

    .boot.data : {
        *(.boot.data)
    }

    .boot.bss (NOLOAD) : {
        *(.boot.bss)
    }

    __boot_phys_end = .;

    . += KERNEL_VMA;

    __kernel_virt_start = .;
    __kernel_phys_start = __kernel_virt_start - KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA) {
        *(.text)
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_VMA) {
        *(.rodata)
    }

    .gdt : AT(ADDR(.gdt) - KERNEL_VMA) {
        __gdt_start = .;
        *(.gdt)
        __gdt_end = .;
    }

    .data : AT(ADDR(.data) - KERNEL_VMA) {
        *(.data)
    }

    .bss (NOLOAD) : AT(ADDR(.bss) - KERNEL_VMA) {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    }

    .stack (NOLOAD) : AT(ADDR(.stack) - KERNEL_VMA) {
        __stack_bottom = .;
        . = . + 0x4000;
        __stack_top = .;
    }

    __kernel_virt_end = .;
    __kernel_phys_end = __kernel_virt_end - KERNEL_VMA;
    end_of_kernel = __kernel_virt_end;

    /DISCARD/ : { *(.eh_frame) *(.comment) }
}

; boot.S
[BITS 32]

SECTION .multiboot
    align 4
    MB_MAGIC   equ 0x1BADB002
    MB_FLAGS   equ 0x00000003
    MB_CHECK   equ -(MB_MAGIC + MB_FLAGS)
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECK

SECTION .text
    global start
    extern kernel_main
    extern __stack_top

start:
    mov esp, __stack_top
    push 0
    popf
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

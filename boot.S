; boot.S
[BITS 32]

%define KERNEL_VMA 0xC0000000

SECTION .multiboot
	align 4
	MB_MAGIC   equ 0x1BADB002
	MB_FLAGS   equ 0x00000003
	MB_CHECK   equ -(MB_MAGIC + MB_FLAGS)
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECK

SECTION .boot.data
	mboot_magic_saved: dd 0
	mboot_header_saved: dd 0

SECTION .boot.bss nobits align=4096
	boot_page_dir:  resd 1024
	boot_low_pt:    resd 1024
	boot_high_pt:   resd 1024
	boot_stack:     resb 8192
	boot_stack_top:

SECTION .boot.text
	global start
	global mboot_magic_saved
	extern kernel_main
	extern gdt_init
	extern gdt_install
	extern init_memory_paging
	extern page_dir
	extern __stack_top
	extern __gdt_start
	extern __gdt_end

GDTADDR   equ 0x00000800

start:
	mov [mboot_magic_saved], eax
	mov [mboot_header_saved], ebx
	mov esp, boot_stack_top
	call setup_bootstrap_paging
	call enable_paging
	jmp higher_half_entry

setup_bootstrap_paging:
	cld
	mov edi, boot_page_dir
	xor eax, eax
	mov ecx, 1024
	rep stosd

	mov edi, boot_low_pt
	mov ebx, 0x00000003
	mov ecx, 1024
.fill_tables:
	mov [edi], ebx
	add ebx, 0x1000
	add edi, 4
	loop .fill_tables

	mov esi, boot_low_pt
	mov edi, boot_high_pt
	mov ecx, 1024
	rep movsd

	mov eax, boot_low_pt
	or eax, 0x3
	mov [boot_page_dir + 0 * 4], eax

	mov eax, boot_high_pt
	or eax, 0x3
	mov [boot_page_dir + 768 * 4], eax

	mov eax, boot_page_dir
	mov cr3, eax
	ret

load_page_directory:
	mov eax, page_dir
	sub eax, KERNEL_VMA
	mov cr3, eax
	ret

enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret

SECTION .text

higher_half_entry:
	mov esp, __stack_top
	push 0
	popf
	call gdt_init
	cld
	mov esi, __gdt_start
	mov edi, GDTADDR
	mov ecx, __gdt_end
	sub ecx, __gdt_start
	rep movsb
	call gdt_install
	mov ebx, [mboot_header_saved]
	push ebx
	call init_memory_paging
	add esp, 4
	call load_page_directory
	mov eax, [mboot_magic_saved]
	mov ebx, [mboot_header_saved]
	push ebx
	push eax
	call kernel_main
	jmp .hang

.hang:
	cli
	hlt
	jmp .hang

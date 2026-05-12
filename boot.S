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
	extern gdt_init
	extern gdt_install
	extern init_memory_paging
	extern page_dir
	extern __stack_top
	extern __gdt_start
	extern __gdt_end

GDTADDR   equ 0x00000800

start:
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
	; call init_memory_paging
	; call load_page_directory
	; call enable_paging
	call kernel_main

; load_page_directory:
; 	mov eax, page_dir
; 	mov cr3, eax
; 	ret

; enable_paging:
; 	mov eax, cr0
; 	or eax, 0x80000000
; 	mov cr0, eax
; 	ret

.hang:
	cli
	hlt
	jmp .hang

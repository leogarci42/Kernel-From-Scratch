# KFS'S

This project is about writing a kernel 32bit for i386

## Overview

```
├── boot.S
├── generate_symtab.py
├── header
│   ├── helpers.h
│   ├── kernel.h
│   ├── keyboard_map.h
│   ├── symtab.h
│   └── vga.h
├── kernel-srcs
│   ├── device
│   │   └── keyboard_handler.c
│   ├── helpers
│   │   ├── helpers.c
│   │   ├── helper_vga.c
│   │   └── printf
│   │       ├── printf.c
│   │       └── printf_helpers.c
│   ├── kernel_main.c
│   ├── memory
│   │   ├── DT
│   │   │   ├── gdt.c
│   │   │   └── idt.c
│   │   └── memory_paging.c
│   └── panic.c
├── linker.ld
├── Makefile
└── README.md

7 directories, 20 files
```
## How to run it

### pre-requisite

even though our kernel should not have any dependency (bare metal), to compile and build it it's a different topic!

Here is the list of all the mandatory packages:

	--> Make (How can we use a Makefile without it ?!)
	--> cc (To compile our C code part of the kernel)
	--> nasm (To assemble our assembly (x86) code part of the kernel - boot.S)
	--> ld (to link our *.o to the target architecture aka elf i386 here)
	--> python (Yes, no matter how horrifying it is to use python for a kernel, we must need it. - explanation later)
	--> grub (our bootloader, and more specifically it's tools ex: grub-mkrescue (to build our ISO))
	--> QEMU (our machine emulator even though non technically mandatory, it's better if you dont want to boot straight in your computer :Dd)

Know once we have all our dependency installed, let's try it: 

```
make run
```

## What can you do?

i'll tell you straight, not so much, but admire it, this is our own kernel, isn't it cool ?!

```
halt - halt the kernel

reboot - reboot the kernel

panic - create a kernel panic

stack - print the stack (in a human readable way)

alt+[1,2,3] - to switch terminal

Ctrl + r - to switch color in an arbitrary order
```

## Documentation

The docs we used for this project:

The visitor's guide: [OSDev](https://wiki.osdev.org/Expanded_Main_Page)


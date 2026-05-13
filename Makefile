RESET_CMD  := $(shell tput sgr0 2>/dev/null)
RED_CMD    := $(shell tput setaf 1 2>/dev/null)
GREEN_CMD  := $(shell tput setaf 2 2>/dev/null)
YELLOW_CMD := $(shell tput setaf 3 2>/dev/null)
BLUE_CMD   := $(shell tput setaf 4 2>/dev/null)
BOLD_CMD   := $(shell tput bold 2>/dev/null)

export RESET_CMD RED_CMD GREEN_CMD YELLOW_CMD BLUE_CMD BOLD_CMD

NAME     := kfs
CC       := cc
AS       := nasm
LD       := ld
CFLAGS   := -m32 -ffreestanding -fno-builtin -nostdlib -I./header -O2 -Wall -Wextra -fno-omit-frame-pointer -g
ASFLAGS  := -f elf32
LDFLAGS  := -m elf_i386 -T linker.ld

SRC_BASE := kernel-srcs/kernel_main.c \
			kernel-srcs/panic.c \
            kernel-srcs/helpers/helper_vga.c \
			kernel-srcs/device/keyboard_handler.c \
			kernel-srcs/helpers/helpers.c \
			kernel-srcs/memory/memory_paging.c \
			kernel-srcs/memory/DT/gdt.c \
			kernel-srcs/helpers/printf/printf.c \
			kernel-srcs/helpers/printf/printf_helpers.c

GENERATED_SYM := kernel-srcs/helpers/symtab.c
SRC      := $(SRC_BASE) $(GENERATED_SYM)
ASM_SRC  := boot.S 

OBJ_DIR  := obj
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
OBJS_BASE := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_BASE))
OBJS_SYM := $(OBJ_DIR)/kernel-srcs/helpers/symtab.o
ASM_OBJ := $(OBJ_DIR)/boot.o
OBJS_ALL := $(ASM_OBJ) $(OBJS)
ISO_DIR  := iso
GRUB_DIR := $(ISO_DIR)/boot/grub
ISO      := kernel.iso

.PHONY: all clean fclean re iso run

all: $(NAME)

$(GENERATED_SYM):
	@if [ ! -f $(GENERATED_SYM) ]; then \
		echo "$(BLUE_CMD)Creating stub symbol table...$(RESET_CMD)"; \
		mkdir -p $(dir $(GENERATED_SYM)); \
		echo '#include <stdint.h>' > $(GENERATED_SYM); \
		echo '#include "symtab.h"' >> $(GENERATED_SYM); \
		echo '' >> $(GENERATED_SYM); \
		echo 'symbol_t symtab[] = {};' >> $(GENERATED_SYM); \
		echo 'size_t symtab_size = 0;' >> $(GENERATED_SYM); \
	fi

$(NAME): $(GENERATED_SYM) $(ASM_OBJ) $(OBJS_BASE) $(OBJS_SYM)
	@echo "$(BLUE_CMD)Generating symbol table (pass 1)...$(RESET_CMD)"
	@$(LD) $(LDFLAGS) -o $(NAME).tmp $(ASM_OBJ) $(OBJS_BASE) $(OBJS_SYM)
	@python3 generate_symtab.py $(NAME).tmp $(GENERATED_SYM)
	@echo "$(YELLOW_CMD)Recompiling symtab.c...$(RESET_CMD)"
	@$(CC) $(CFLAGS) -c $(GENERATED_SYM) -o $(OBJS_SYM)
	@echo "$(BLUE_CMD)Generating symbol table (pass 2)...$(RESET_CMD)"
	@$(LD) $(LDFLAGS) -o $(NAME).tmp $(ASM_OBJ) $(OBJS_BASE) $(OBJS_SYM)
	@python3 generate_symtab.py $(NAME).tmp $(GENERATED_SYM)
	@echo "$(BLUE_CMD)Linking $(NAME)...$(RESET_CMD)"
	@$(LD) $(LDFLAGS) -o $(NAME) $(OBJS_ALL)
	@echo "$(GREEN_CMD)Build successful: $(NAME) $(RESET_CMD)"
	@rm -f $(NAME).tmp

$(ASM_OBJ): $(ASM_SRC)
	@mkdir -p $(dir $@)
	@echo "$(YELLOW_CMD)Assembling $<...$(RESET_CMD)"
	@$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c	
	@mkdir -p $(dir $@)	
	@echo "$(YELLOW_CMD)Compiling $<...$(RESET_CMD)"
	@$(CC) $(CFLAGS) -c $< -o $@


iso: $(NAME)
	@echo "$(BLUE_CMD)Creating ISO structure...$(RESET_CMD)"
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(NAME) $(ISO_DIR)/boot/$(NAME)
	@echo 'set timeout=0' > $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "$(NAME)" {' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  multiboot /boot/$(NAME)' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '  boot' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}' >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo "$(YELLOW_CMD)Attempting grub-mkrescue...$(RESET_CMD)"
	-grub-mkrescue -o $(ISO_NAME) $(ISO_DIR) 2>/dev/null || echo "$(RED_CMD)GRUB ISO build failed (missing mformat).$(RESET_CMD)"

run: fclean all
	@echo "$(BLUE_CMD)Starting QEMU (Multiboot Compliant)...$(RESET_CMD)"
	@qemu-system-i386 -kernel $(NAME)

clean:
	@echo "$(RED_CMD)Cleaning...$(RESET_CMD)"
	@rm -rf $(OBJ_DIR) $(ISO_DIR)

fclean: clean
	@echo "$(RED_CMD)Deleting artifacts...$(RESET_CMD)"
	@rm -f $(NAME) $(ISO_NAME) $(GENERATED_SYM)

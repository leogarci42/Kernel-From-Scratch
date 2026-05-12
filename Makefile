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
CFLAGS   := -m32 -ffreestanding -fno-builtin -nostdlib -I./header -O2 -Wall
ASFLAGS  := -f elf32
LDFLAGS  := -m elf_i386 -T linker.ld

SRC      := kernel-srcs/kernel_main.c \
			kernel-srcs/panic.c \
            kernel-srcs/helpers/helper_vga.c \
			kernel-srcs/device/keyboard_handler.c \
			kernel-srcs/helpers/helpers.c \
			kernel-srcs/memory/memory_paging.c \
			kernel-srcs/memory/DT/gdt.c \
			kernel-srcs/helpers/printf/printf.c \
			kernel-srcs/helpers/printf/printf_helpers.c
ASM_SRC  := boot.S 

OBJ_DIR  := obj
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
ASM_OBJ := $(OBJ_DIR)/boot.o
OBJS_ALL := $(ASM_OBJ) $(OBJS)
ISO_DIR  := iso
GRUB_DIR := $(ISO_DIR)/boot/grub
ISO      := kernel.iso

.PHONY: all clean fclean re iso run

all: $(NAME)

$(NAME): $(OBJS_ALL) linker.ld
	@echo "$(BLUE_CMD)Linking $(NAME)...$(RESET_CMD)"
	@$(LD) $(LDFLAGS) -o $(NAME) $(OBJS_ALL)
	@echo "$(GREEN_CMD)Build successful: $(NAME) $(RESET_CMD)"

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

run: all
	@echo "$(BLUE_CMD)Starting QEMU (Multiboot Compliant)...$(RESET_CMD)"
	@qemu-system-i386 -kernel $(NAME)

clean:
	@echo "$(RED_CMD)Cleaning...$(RESET_CMD)"
	@rm -rf $(OBJ_DIR) $(ISO_DIR)

fclean: clean
	@echo "$(RED_CMD)Deleting artifacts...$(RESET_CMD)"
	@rm -f $(NAME) $(ISO_NAME)

# Makefile for My Minimal OS with Shell & Filesystem

ASM         = nasm
CC          = gcc
LD          = ld
GRUB2       = grub2-mkrescue
QEMU        = qemu-system-x86_64
ASMFLAGS    = -f elf32
CFLAGS      = -m32 -ffreestanding -nostdlib -nostartfiles -O2 -Wall -Wextra -Iinclude

SRC_DIR     = src
ARCH_DIR    = $(SRC_DIR)/arch
BUILD_DIR   = build

# Find all .c and .asm files recursively
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c')
ASM_SOURCES := $(shell find $(ARCH_DIR) -name '*.asm')

C_COMMAND_SOURCES := $(wildcard $(COMMAND_SRC_DIR)/*.c)
C_KERNEL_SOURCES := $(filter-out $(C_COMMAND_SOURCES), $(C_SOURCES))
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_KERNEL_SOURCES))

ASM_OBJECTS := $(patsubst $(ARCH_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

KERNEL_BIN  = $(BUILD_DIR)/kernel.bin
ISO_DIR     = iso
ISO         = $(BUILD_DIR)/os.iso
DISK_IMG    = disk.img

# List your commands here
COMMANDS := ls mkdir touch

# Paths
COMMAND_SRC_DIR := src/command
BIN_OUT_DIR := bin

.PHONY: all clean run prepare_iso commands

all: $(ISO) commands

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile all .c files to .o files in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble all .asm files to .o files in build/
$(BUILD_DIR)/%.o: $(ARCH_DIR)/%.asm | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJECTS) $(ASM_OBJECTS) linker.ld
	$(LD) -m elf_i386 -T linker.ld -o $@ $(ASM_OBJECTS) $(OBJECTS)

	install_commands: commands
	mkdir -p $(ISO_DIR)/bin
	cp bin/* $(ISO_DIR)/bin/

prepare_iso: $(KERNEL_BIN) install_commands
	@echo "Setting up ISO structure..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	@cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg

$(ISO): prepare_iso
	$(GRUB2) -o $@ $(ISO_DIR)

run: all
	$(QEMU) -cdrom $(ISO)

# Build rules for each command
$(BIN_OUT_DIR)/%: $(COMMAND_SRC_DIR)/%.c
	mkdir -p $(BIN_OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_OUT_DIR)/$*.o
	$(LD) -m elf_i386 -T linker.ld --unresolved-symbols=ignore-all -o $@ $(BIN_OUT_DIR)/$*.o

# Build all commands
commands: $(addprefix $(BIN_OUT_DIR)/, $(COMMANDS))

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ISO_DIR)/boot
	@rm -f $(DISK_IMG)
	@rm -rf $(BIN_OUT_DIR)/*

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

OBJECTS     := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(ARCH_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

KERNEL_BIN  = $(BUILD_DIR)/kernel.bin
ISO_DIR     = iso
ISO         = $(BUILD_DIR)/os.iso
DISK_IMG    = disk.img

.PHONY: all clean run prepare_iso

all: $(ISO)

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

prepare_iso: $(KERNEL_BIN)
	@echo "Setting up ISO structure..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	@cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg

$(ISO): prepare_iso
	$(GRUB2) -o $@ $(ISO_DIR)

run: all
	$(QEMU) -cdrom $(ISO)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ISO_DIR)/boot
	@rm -f $(DISK_IMG)

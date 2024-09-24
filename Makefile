# Makefile

# ------------------------------------
# Compiler and Tools
# ------------------------------------
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -std=gnu99 -c
LDFLAGS = -T linker.ld -nostdlib

# ------------------------------------
# Directories
# ------------------------------------
BOOTDIR = Bootloader
KERNELDIR = Kernel
INCLUDEDIR = $(KERNELDIR)/include
SRC = $(KERNELDIR)/src
BUILD_DIR = Build

# ------------------------------------
# Output Files
# ------------------------------------
TARGET = os.bin
IMAGE = OS.img

# ------------------------------------
# Source and Object Files
# ------------------------------------
BOOTLOADER = $(BOOTDIR)/bootloader.asm
KERNEL_SOURCES = $(SRC)/kernel.c $(SRC)/gdt.c $(SRC)/paging.c $(SRC)/filesystem.c
LOADER_SOURCE = $(SRC)/loader.asm

# Define object files to be placed in Build/
KERNEL_OBJECTS = $(patsubst $(SRC)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_SOURCES))
LOADER_OBJECT = $(patsubst $(SRC)/%.asm,$(BUILD_DIR)/%.o,$(LOADER_SOURCE))
OBJECTS = $(KERNEL_OBJECTS) $(LOADER_OBJECT)

# ------------------------------------
# Default Target
# ------------------------------------
all: $(TARGET)

# Concatenate bootloader and kernel into the final OS binary
$(TARGET): bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > $(TARGET)

# Compile Bootloader Assembly to binary
bootloader.bin: $(BOOTLOADER)
	$(AS) -f bin $< -o bootloader.bin

# Link Kernel Object Files into kernel.bin
kernel.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJECTS)

# Pattern rule for compiling C source files to Build/*.o
$(BUILD_DIR)/%.o: $(SRC)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) $< -o $@

# Pattern rule for assembling ASM source files to Build/*.o
$(BUILD_DIR)/%.o: $(SRC)/%.asm | $(BUILD_DIR)
	$(AS) -f elf64 $< -o $@

# Ensure Build/ directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ------------------------------------
# Disk Image Creation
# ------------------------------------
diskimg: $(TARGET)
	@echo "Creating disk image..."
	# Create a 1MB empty disk image
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2048
	# Write bootloader.bin to the first sector
	dd if=bootloader.bin of=$(IMAGE) bs=512 count=1 conv=notrunc
	# Write kernel.bin starting at sector 1
	dd if=kernel.bin of=$(IMAGE) bs=512 seek=1 conv=notrunc
	@echo "Disk image $(IMAGE) created successfully."

# ------------------------------------
# Run with QEMU
# ------------------------------------
run: diskimg
	@echo "Booting with QEMU..."
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE)

# Optional: Run with QEMU and Debugging Features
run-debug: diskimg
	@echo "Booting with QEMU (Debug Mode)..."
	qemu-system-x86_64 \
		-drive format=raw,file=$(IMAGE) \
		-m 512 \
		-serial stdio \
		-monitor stdio \
		-display stdio

# ------------------------------------
# Clean Build Artifacts
# ------------------------------------
clean:
	rm -f *.bin
	rm -f *.img
	rm -f $(BUILD_DIR)/*.o
	rm -f $(IMAGE)

.PHONY: all clean diskimg run run-debug

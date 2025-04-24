# Makefile for My Minimal OS with Shell & Filesystem

ASM         = nasm
CC          = gcc
LD          = ld
GRUB2       = grub2-mkrescue
# QEMU        = qemu-system-i386

QEMU        = qemu-system-x86_64
ASMFLAGS    = -f elf32
CFLAGS      = -m32 -ffreestanding -nostdlib -nostartfiles -O2 -Wall -Wextra

SRC_DIR     = src
BUILD_DIR   = build

CRT0        = $(SRC_DIR)/crt0.asm
CRT0_OBJ    = $(BUILD_DIR)/crt0.o

IDT_FLUSH_OBJ = $(BUILD_DIR)/idt_flush.o
KEYBOARD_ASM_OBJ = $(BUILD_DIR)/keyboard_stub.o
TIMER_ASM_OBJ = $(BUILD_DIR)/timer.o
GDT_OBJ = $(BUILD_DIR)/gdt.o

# Automatically locate all C source files in SRC_DIR and convert to .o object file names
C_SOURCES   := $(wildcard $(SRC_DIR)/*.c)
OBJECTS     := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

KERNEL_BIN  = $(BUILD_DIR)/kernel.bin
ISO_DIR     = iso
ISO         = $(BUILD_DIR)/os.iso
DISK_IMG    = disk.img

.PHONY: all clean run prepare_iso prepare_disk

all: $(ISO)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(CRT0_OBJ): $(CRT0) | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(IDT_FLUSH_OBJ): $(SRC_DIR)/idt_flush.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(KEYBOARD_ASM_OBJ): $(SRC_DIR)/keyboard.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(TIMER_ASM_OBJ): $(SRC_DIR)/timer.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(GDT_OBJ): $(SRC_DIR)/gdt.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(CRT0_OBJ) $(OBJECTS) $(GDT_OBJ) $(IDT_FLUSH_OBJ) $(KEYBOARD_ASM_OBJ) $(TIMER_ASM_OBJ) linker.ld
	$(LD) -m elf_i386 -T linker.ld -o $(KERNEL_BIN) $(CRT0_OBJ) $(OBJECTS) $(GDT_OBJ) $(IDT_FLUSH_OBJ) $(KEYBOARD_ASM_OBJ) $(TIMER_ASM_OBJ)

prepare_iso: $(KERNEL_BIN)
	@echo "Setting up ISO structure..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	@cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg

prepare_disk: $(KERNEL_BIN)
	@echo "Deleting old disk image (if any)..."
	@rm -f $(DISK_IMG)
	@echo "Creating new 32GB disk image..."
	@qemu-img create -f raw $(DISK_IMG) 2G
	@echo "Creating partition table and primary partition..."
	@parted --script $(DISK_IMG) mklabel msdos
	@parted --script $(DISK_IMG) mkpart primary ext2 1MiB 100%
	@parted --script $(DISK_IMG) set 1 boot on
	@echo "Mapping image to loop device..."
	@LOOP_DEV=$$(losetup --show -f $(DISK_IMG)); \
	 echo "Loop device: $$LOOP_DEV"; \
	 echo "Mapping partitions..."; \
	 kpartx -a $$LOOP_DEV; \
	 PART_DEV=/dev/mapper/$$(basename $$LOOP_DEV)p1; \
	 echo "Formatting partition $$PART_DEV as ext2..."; \
	 mkfs.ext2 -q $$PART_DEV; \
	 TMP_MNT=$$(mktemp -d); \
	 echo "Mounting partition at $$TMP_MNT"; \
	 mount $$PART_DEV $$TMP_MNT; \
	 echo "Installing boot files into partition..."; \
	 mkdir -p $$TMP_MNT/boot/grub; \
	 cp $(KERNEL_BIN) $$TMP_MNT/boot/kernel.bin; \
	 cp grub.cfg $$TMP_MNT/boot/grub/grub.cfg; \
	 echo "Installing GRUB..."; \
	 grub2-install --recheck --boot-directory=$$TMP_MNT/boot --target=i386-pc $$LOOP_DEV; \
	 \
	 echo "Installing MBR boot code..."; \
	 dd if=/usr/lib/grub/i386-pc/boot.img of=$(DISK_IMG) conv=notrunc; \
	 \
	 echo "Unmounting partition..."; \
	 umount $$TMP_MNT; \
	 rmdir $$TMP_MNT; \
	 echo "Deleting partition mappings..."; \
	 kpartx -d $$LOOP_DEV; \
	 losetup -d $$LOOP_DEV; \
	 \
	 echo "Writing OS file table to disk image..."; \
	 # Create filetable.bin (320 bytes: 8 entries of 40 bytes each)
	 # Entry format (40 bytes): 32 bytes for name, 4 bytes start_lba (LE), 4 bytes sector_count (LE)
	 # Entry 0: "/boot/kernel.bin" (16 chars, pad with 16 nulls), start_lba = 110 (0x6E), count = 4  
	 # Entry 1: "/cmd/hello.txt" (14 chars, pad with 18 nulls), start_lba = 114 (0x72), count = 1  
	 # The remaining 6 entries will be zeros.
	 echo -ne "/boot/kernel.bin\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" > filetable.bin; \
	 echo -ne "\x6E\x00\x00\x00\x04\x00\x00\x00" >> filetable.bin; \
	 echo -ne "/cmd/hello.txt\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" >> filetable.bin; \
	 echo -ne "\x72\x00\x00\x00\x01\x00\x00\x00" >> filetable.bin; \
	 dd if=/dev/zero bs=1 count=$$((320 - $$(stat -c%s filetable.bin))) >> filetable.bin; \
	 dd if=filetable.bin of=$(DISK_IMG) bs=512 seek=100 conv=notrunc; \
	 rm filetable.bin; \
	 \
	 echo "Copying OS file data to disk image..."; \
	 # Copy kernel.bin data into sector 110
	 dd if=$(KERNEL_BIN) of=$(DISK_IMG) bs=512 seek=110 conv=notrunc; \
	 # Create a file for /cmd/hello.txt and copy it into sector 114
	 echo -ne "Hello from /cmd!\n" > hello.txt; \
	 dd if=hello.txt of=$(DISK_IMG) bs=512 seek=114 conv=notrunc; \
	 rm hello.txt; \
	 \
	 echo "Disk image prepared."

$(ISO): prepare_iso
	$(GRUB2) -o $(ISO) $(ISO_DIR)

$(DISK_IMG): prepare_disk
	@echo "Disk image prepared."

run: all prepare_disk
	@echo "Writing OS.ISO to disk image..."
	dd if=$(ISO) of=$(DISK_IMG) conv=notrunc
	$(QEMU) -hda $(DISK_IMG)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ISO_DIR)/boot
	@rm -f $(DISK_IMG)

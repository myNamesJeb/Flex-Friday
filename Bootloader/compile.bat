
nasm -f bin boot.asm -o boot.bin
dd if=/dev/zero of=floppy.img bs=512 count=2880
dd if=boot.bin of=floppy.img conv=notrunc 
qemu-system-i386 -fda floppy.img  
echo "done"
//i686-elf-gcc -ffreestanding -m32 -nostdlib -nostartfiles -O2 -c testprog.c -o testprog.o
//ld -m elf_i386 -Ttext 0x200000 --oformat binary -e main testprog.o -o testprog.bin
// need to compile to flat bin 
// testprog.c
void main(void) {
    // Do something simple
    volatile int *v = (int*)0xB8000; // VGA text buffer (if running on x86 PC)
    v[0] = 0x2F002F00; // Write '/' chars with color
    while (1) { }
}
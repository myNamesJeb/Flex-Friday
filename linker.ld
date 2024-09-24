ENTRY(kernel_entry)

MEMORY
{
    RAM (rw) : ORIGIN = 0x100000, LENGTH = 256M
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text :
    {
        *(.text*)
    } > RAM

    .data :
    {
        *(.data*)
    } > RAM

    .bss :
    {
        *(.bss*)
    } > RAM
}

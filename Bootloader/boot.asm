format pe64 dll efi
entry main

section '.text' code executable readable

include 'uefi.inc'

main:
    ; initialize UEFI library
    InitializeLib
    jc @f

    ; call uefi function to print to screen
    uefi_call_wrapper ConOut, OutputString, ConOut, _hello

@@: mov eax, EFI_SUCCESS
    retn

section '.data' data readable writeable

_hello                                  du 'Hello World',13,10,0

section '.reloc' fixups data discardable
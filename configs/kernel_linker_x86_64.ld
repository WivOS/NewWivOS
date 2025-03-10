ENTRY(kernel_entry)
SECTIONS
{
    . = 0xffffffff80000000 + 0x01000000;

    .text ALIGN(4K) :
    {
        *(.text*)
        *(.symbols*)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data .data.*)

        KEEP(*(.limine_reqs_start_marker))
        KEEP(*(.limine_reqs))
        KEEP(*(.limine_reqs_end_marker))

        *(.sdata .sdata.*)
        PROVIDE(kernel_symbols_start = .);
        PROVIDE(kernel_symbols_end = .);
    }

    .bss ALIGN(4K) :
    {
        *(.bss*)
    }
}
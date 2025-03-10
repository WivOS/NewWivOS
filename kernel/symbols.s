.section symbols
.global kernel_symbols_start
kernel_symbols_start:

    .extern CPULocals
    .align 8
    .quad CPULocals
    .quad 0x1800
    .asciz "CPULocals"

    .extern DTBNodes
    .align 8
    .quad DTBNodes
    .quad 0x8
    .asciz "DTBNodes"

    .extern KernelPageMap
    .align 8
    .quad KernelPageMap
    .quad 0x8
    .asciz "KernelPageMap"

    .extern MEM_PHYS_OFFSET
    .align 8
    .quad MEM_PHYS_OFFSET
    .quad 0x8
    .asciz "MEM_PHYS_OFFSET"

    .extern VmmKernelBase
    .align 8
    .quad VmmKernelBase
    .quad 0x8
    .asciz "VmmKernelBase"

    .extern VmmKernelSize
    .align 8
    .quad VmmKernelSize
    .quad 0x8
    .asciz "VmmKernelSize"

    .extern VmmPhysicalKernelBase
    .align 8
    .quad VmmPhysicalKernelBase
    .quad 0x8
    .asciz "VmmPhysicalKernelBase"

    .extern __global_pointer$
    .align 8
    .quad __global_pointer$
    .quad 0x0
    .asciz "__global_pointer$"

    .extern _vsnprintf_internal
    .align 8
    .quad _vsnprintf_internal
    .quad 0x786
    .asciz "_vsnprintf_internal"

    .extern arch_debug_printf_function
    .align 8
    .quad arch_debug_printf_function
    .quad 0x36
    .asciz "arch_debug_printf_function"

    .extern arch_disable_interrupts
    .align 8
    .quad arch_disable_interrupts
    .quad 0x1a
    .asciz "arch_disable_interrupts"

    .extern arch_enable_interrupts
    .align 8
    .quad arch_enable_interrupts
    .quad 0x1c
    .asciz "arch_enable_interrupts"

    .extern arch_init
    .align 8
    .quad arch_init
    .quad 0x12
    .asciz "arch_init"

    .extern arch_interrupt_complete
    .align 8
    .quad arch_interrupt_complete
    .quad 0x20
    .asciz "arch_interrupt_complete"

    .extern arch_interrupt_disable
    .align 8
    .quad arch_interrupt_disable
    .quad 0x3e
    .asciz "arch_interrupt_disable"

    .extern arch_interrupt_enable
    .align 8
    .quad arch_interrupt_enable
    .quad 0x3a
    .asciz "arch_interrupt_enable"

    .extern arch_interrupt_init
    .align 8
    .quad arch_interrupt_init
    .quad 0x8a
    .asciz "arch_interrupt_init"

    .extern arch_interrupt_max
    .align 8
    .quad arch_interrupt_max
    .quad 0x1a
    .asciz "arch_interrupt_max"

    .extern arch_interrupt_min
    .align 8
    .quad arch_interrupt_min
    .quad 0x12
    .asciz "arch_interrupt_min"

    .extern arch_post_vmm
    .align 8
    .quad arch_post_vmm
    .quad 0x32
    .asciz "arch_post_vmm"

    .extern arch_vmm_preinit
    .align 8
    .quad arch_vmm_preinit
    .quad 0x42
    .asciz "arch_vmm_preinit"

    .extern be32
    .align 8
    .quad be32
    .quad 0x2e
    .asciz "be32"

    .extern be64
    .align 8
    .quad be64
    .quad 0x52
    .asciz "be64"

    .extern dtb_find_node
    .align 8
    .quad dtb_find_node
    .quad 0x1d6
    .asciz "dtb_find_node"

    .extern dtb_init
    .align 8
    .quad dtb_init
    .quad 0x218
    .asciz "dtb_init"

    .extern dtb_prop_read_address_range
    .align 8
    .quad dtb_prop_read_address_range
    .quad 0xe0
    .asciz "dtb_prop_read_address_range"

    .extern dtb_prop_read_string
    .align 8
    .quad dtb_prop_read_string
    .quad 0x9e
    .asciz "dtb_prop_read_string"

    .extern dtb_prop_read_stringlist
    .align 8
    .quad dtb_prop_read_stringlist
    .quad 0xd0
    .asciz "dtb_prop_read_stringlist"

    .extern dtb_prop_read_u32
    .align 8
    .quad dtb_prop_read_u32
    .quad 0xd4
    .asciz "dtb_prop_read_u32"

    .extern dtb_prop_read_u64
    .align 8
    .quad dtb_prop_read_u64
    .quad 0xd4
    .asciz "dtb_prop_read_u64"

    .extern gentree_create
    .align 8
    .quad gentree_create
    .quad 0x5c
    .asciz "gentree_create"

    .extern gentree_get_parent
    .align 8
    .quad gentree_get_parent
    .quad 0x86
    .asciz "gentree_get_parent"

    .extern gentree_insert
    .align 8
    .quad gentree_insert
    .quad 0x13a
    .asciz "gentree_insert"

    .extern gentree_node_create
    .align 8
    .quad gentree_node_create
    .quad 0x82
    .asciz "gentree_node_create"

    .extern gentree_remove
    .align 8
    .quad gentree_remove
    .quad 0xdc
    .asciz "gentree_remove"

    .extern get_cpu_struct
    .align 8
    .quad get_cpu_struct
    .quad 0x0
    .asciz "get_cpu_struct"

    .extern hashmap_create
    .align 8
    .quad hashmap_create
    .quad 0x78
    .asciz "hashmap_create"

    .extern hashmap_delete
    .align 8
    .quad hashmap_delete
    .quad 0x106
    .asciz "hashmap_delete"

    .extern hashmap_get
    .align 8
    .quad hashmap_get
    .quad 0x15c
    .asciz "hashmap_get"

    .extern hashmap_has
    .align 8
    .quad hashmap_has
    .quad 0x128
    .asciz "hashmap_has"

    .extern hashmap_remove
    .align 8
    .quad hashmap_remove
    .quad 0x29a
    .asciz "hashmap_remove"

    .extern hashmap_set
    .align 8
    .quad hashmap_set
    .quad 0x35a
    .asciz "hashmap_set"

    .extern hashmap_string_hash
    .align 8
    .quad hashmap_string_hash
    .quad 0x38
    .asciz "hashmap_string_hash"

    .extern hashmap_to_list
    .align 8
    .quad hashmap_to_list
    .quad 0x214
    .asciz "hashmap_to_list"

    .extern kcalloc
    .align 8
    .quad kcalloc
    .quad 0xea
    .asciz "kcalloc"

    .extern kernel_entry
    .align 8
    .quad kernel_entry
    .quad 0x12
    .asciz "kernel_entry"

    .extern kfree
    .align 8
    .quad kfree
    .quad 0x68
    .asciz "kfree"

    .extern kmain
    .align 8
    .quad kmain
    .quad 0x64
    .asciz "kmain"

    .extern kmalloc
    .align 8
    .quad kmalloc
    .quad 0xb8
    .asciz "kmalloc"

    .extern krealloc
    .align 8
    .quad krealloc
    .quad 0x300
    .asciz "krealloc"

    .extern lfind
    .align 8
    .quad lfind
    .quad 0x16
    .asciz "lfind"

    .extern limine_get_hhdm
    .align 8
    .quad limine_get_hhdm
    .quad 0x1a
    .asciz "limine_get_hhdm"

    .extern limine_get_kernel_info
    .align 8
    .quad limine_get_kernel_info
    .quad 0x18
    .asciz "limine_get_kernel_info"

    .extern limine_get_memmap
    .align 8
    .quad limine_get_memmap
    .quad 0x18
    .asciz "limine_get_memmap"

    .extern limine_get_mp
    .align 8
    .quad limine_get_mp
    .quad 0x18
    .asciz "limine_get_mp"

    .extern list_contains
    .align 8
    .quad list_contains
    .quad 0x70
    .asciz "list_contains"

    .extern list_create
    .align 8
    .quad list_create
    .quad 0x64
    .asciz "list_create"

    .extern list_destroy
    .align 8
    .quad list_destroy
    .quad 0x34
    .asciz "list_destroy"

    .extern list_destroy_node
    .align 8
    .quad list_destroy_node
    .quad 0x16
    .asciz "list_destroy_node"

    .extern list_get_back
    .align 8
    .quad list_get_back
    .quad 0x5a
    .asciz "list_get_back"

    .extern list_get_front
    .align 8
    .quad list_get_front
    .quad 0x5a
    .asciz "list_get_front"

    .extern list_get_indexed
    .align 8
    .quad list_get_indexed
    .quad 0x66
    .asciz "list_get_indexed"

    .extern list_pop
    .align 8
    .quad list_pop
    .quad 0x76
    .asciz "list_pop"

    .extern list_push_back
    .align 8
    .quad list_push_back
    .quad 0x90
    .asciz "list_push_back"

    .extern list_push_before
    .align 8
    .quad list_push_before
    .quad 0x86
    .asciz "list_push_before"

    .extern list_push_front
    .align 8
    .quad list_push_front
    .quad 0x82
    .asciz "list_push_front"

    .extern list_remove
    .align 8
    .quad list_remove
    .quad 0x138
    .asciz "list_remove"

    .extern list_remove_back
    .align 8
    .quad list_remove_back
    .quad 0x80
    .asciz "list_remove_back"

    .extern list_remove_front
    .align 8
    .quad list_remove_front
    .quad 0x80
    .asciz "list_remove_front"

    .extern list_remove_indexed
    .align 8
    .quad list_remove_indexed
    .quad 0x74
    .asciz "list_remove_indexed"

    .extern locked_dec
    .align 8
    .quad locked_dec
    .quad 0x18
    .asciz "locked_dec"

    .extern locked_inc
    .align 8
    .quad locked_inc
    .quad 0x18
    .asciz "locked_inc"

    .extern locked_read
    .align 8
    .quad locked_read
    .quad 0x14
    .asciz "locked_read"

    .extern locked_write
    .align 8
    .quad locked_write
    .quad 0x22
    .asciz "locked_write"

    .extern memcmp
    .align 8
    .quad memcmp
    .quad 0x3e
    .asciz "memcmp"

    .extern memcpy
    .align 8
    .quad memcpy
    .quad 0x2a
    .asciz "memcpy"

    .extern memcpy64
    .align 8
    .quad memcpy64
    .quad 0x26
    .asciz "memcpy64"

    .extern memmove
    .align 8
    .quad memmove
    .quad 0x4c
    .asciz "memmove"

    .extern memset
    .align 8
    .quad memset
    .quad 0x20
    .asciz "memset"

    .extern plic_claim
    .align 8
    .quad plic_claim
    .quad 0x20
    .asciz "plic_claim"

    .extern plic_init
    .align 8
    .quad plic_init
    .quad 0x116
    .asciz "plic_init"

    .extern pmm_alloc
    .align 8
    .quad pmm_alloc
    .quad 0x40
    .asciz "pmm_alloc"

    .extern pmm_alloc_advanced
    .align 8
    .quad pmm_alloc_advanced
    .quad 0xf2
    .asciz "pmm_alloc_advanced"

    .extern pmm_free
    .align 8
    .quad pmm_free
    .quad 0xa2
    .asciz "pmm_free"

    .extern pmm_init
    .align 8
    .quad pmm_init
    .quad 0x2d2
    .asciz "pmm_init"

    .extern printf
    .align 8
    .quad printf
    .quad 0x9e
    .asciz "printf"

    .extern printf_write_char_str
    .align 8
    .quad printf_write_char_str
    .quad 0x2c
    .asciz "printf_write_char_str"

    .extern riscv64_get_boot_dtb
    .align 8
    .quad riscv64_get_boot_dtb
    .quad 0x34
    .asciz "riscv64_get_boot_dtb"

    .extern riscv_clear_soft_interrupt
    .align 8
    .quad riscv_clear_soft_interrupt
    .quad 0x0
    .asciz "riscv_clear_soft_interrupt"

    .extern riscv_cpu_init
    .align 8
    .quad riscv_cpu_init
    .quad 0x1b2
    .asciz "riscv_cpu_init"

    .extern riscv_handle_trap
    .align 8
    .quad riscv_handle_trap
    .quad 0x1e2
    .asciz "riscv_handle_trap"

    .extern riscv_isr
    .align 8
    .quad riscv_isr
    .quad 0x0
    .asciz "riscv_isr"

    .extern riscv_sbicall
    .align 8
    .quad riscv_sbicall
    .quad 0x0
    .asciz "riscv_sbicall"

    .extern set_cpu_struct
    .align 8
    .quad set_cpu_struct
    .quad 0x0
    .asciz "set_cpu_struct"

    .extern spinlock_lock_debug
    .align 8
    .quad spinlock_lock_debug
    .quad 0x6c
    .asciz "spinlock_lock_debug"

    .extern spinlock_lock_normal
    .align 8
    .quad spinlock_lock_normal
    .quad 0x18
    .asciz "spinlock_lock_normal"

    .extern spinlock_try_lock_debug
    .align 8
    .quad spinlock_try_lock_debug
    .quad 0x20
    .asciz "spinlock_try_lock_debug"

    .extern spinlock_try_lock_normal
    .align 8
    .quad spinlock_try_lock_normal
    .quad 0x16
    .asciz "spinlock_try_lock_normal"

    .extern spinlock_unlock
    .align 8
    .quad spinlock_unlock
    .quad 0x18
    .asciz "spinlock_unlock"

    .extern sprintf
    .align 8
    .quad sprintf
    .quad 0x9a
    .asciz "sprintf"

    .extern strchr
    .align 8
    .quad strchr
    .quad 0xe6
    .asciz "strchr"

    .extern strchrnul
    .align 8
    .quad strchrnul
    .quad 0xce
    .asciz "strchrnul"

    .extern strcmp
    .align 8
    .quad strcmp
    .quad 0x36
    .asciz "strcmp"

    .extern strcpy
    .align 8
    .quad strcpy
    .quad 0x40
    .asciz "strcpy"

    .extern strcspn
    .align 8
    .quad strcspn
    .quad 0x142
    .asciz "strcspn"

    .extern strdup
    .align 8
    .quad strdup
    .quad 0x60
    .asciz "strdup"

    .extern strlen
    .align 8
    .quad strlen
    .quad 0x28
    .asciz "strlen"

    .extern strncmp
    .align 8
    .quad strncmp
    .quad 0x44
    .asciz "strncmp"

    .extern strpbrk
    .align 8
    .quad strpbrk
    .quad 0x2c
    .asciz "strpbrk"

    .extern strsep
    .align 8
    .quad strsep
    .quad 0x40
    .asciz "strsep"

    .extern strspn
    .align 8
    .quad strspn
    .quad 0x9c
    .asciz "strspn"

    .extern strstr
    .align 8
    .quad strstr
    .quad 0x70
    .asciz "strstr"

    .extern strtok_r
    .align 8
    .quad strtok_r
    .quad 0xf0
    .asciz "strtok_r"

    .extern vmm_init
    .align 8
    .quad vmm_init
    .quad 0x110
    .asciz "vmm_init"

    .extern vmm_map
    .align 8
    .quad vmm_map
    .quad 0x18
    .asciz "vmm_map"

    .extern vmm_map_huge
    .align 8
    .quad vmm_map_huge
    .quad 0x18
    .asciz "vmm_map_huge"

    .extern vmm_set_pt
    .align 8
    .quad vmm_set_pt
    .quad 0x2a
    .asciz "vmm_set_pt"

    .extern vmm_setup_pt
    .align 8
    .quad vmm_setup_pt
    .quad 0x8c
    .asciz "vmm_setup_pt"

    .extern vmm_unmap
    .align 8
    .quad vmm_unmap
    .quad 0x142
    .asciz "vmm_unmap"
.global kernel_symbols_end
kernel_symbols_end: ;

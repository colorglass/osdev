    .code16
    .text
start:
    mov %dl, boot_dirver

    mov %cs, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov $0x7c00, %sp

    mov $20, %cx
    mov $boot_msg, %ax
    call print_msg
    
load_kernel:
    mov $0x42, %ah
    mov boot_dirver, %dl
    mov $packet, %si
    int $0x13
    jc fail

gather_info:
    /* empty now */

enable_A20:
    in $0x92, %al
    test $0x2, %al
    jnz after
    or $0x2, %al
    and $0xfe, %al
    out %al, $0x92
after:
    jmp enter_protect


fail:
    mov $17, %cx
    mov $fail_msg, %ax
    call print_msg
die:    
    hlt
    jmp die

print_msg:
    push %ax
    push %cx
    mov $0x03, %ah
    xor %bh, %bh
    int $0x10
    jc die

    pop %cx
    pop %ax
    mov $0x7, %bx
    mov %ax, %bp
    mov $0x1301, %ax
    int $0x10
    jc die
    ret

enter_protect:
    cli
    lgdt gdt_48
    mov $0x0001, %ax
    lmsw %ax
    jmp $0x8, $enter_kernel

    .code32
enter_kernel:
    jmp 0x10000

boot_dirver:
    .byte 0x0

boot_msg:
    .ascii "Loading system ...\r\n"

fail_msg:
    .ascii "Booting failed!\r\n"

packet:
packet.size:
    .byte 0x10
packet.reserved:
    .byte 0x0
packet.nr_block:
    .word 0x0080                    /* kernel size: 64kb */
packet.destination:
    .offset:  .word 0x0000
    .segment: .word 0x1000          /* kernel position: 0x10000 */
packet.start:
    .word 0x0001, 0x0, 0x0, 0x0     /* read from the second block */

gdt_start:
    .word 0x0, 0x0, 0x0, 0x0

    /* code seg discriptor */
    .word 0xffff
    .word 0x0000
    .word 0x9a00
    .word 0x00cf

    /* data seg discriptor */
    .word 0xffff
    .word 0x0000
    .word 0x9200
    .word 0x00cf
gdt_end:

gdt_48:
    .word gdt_end - gdt_start - 1
    .long gdt_start


    .org 510
    .word 0xaa55
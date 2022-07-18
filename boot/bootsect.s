    .code16
    .text
/* first move bootsector to 0x90000 for the kernel move */
start:
	mov	$0x07c0, %ax
	mov	%ax, %ds

    mov %dl, boot_dirver            /* save boot dirver number */

	mov	$0x9000, %ax
	mov	%ax, %es
	mov	$256, %cx
	xor %si, %si
	xor %di, %di
	rep
	movsw

    mov %ax, %cs
    jmp $go
go:
    /* now we are in the 0x90000
    mov %ax, %cs
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov $0xffff, %sp

    mov $20, %cx
    mov $msg_boot, %ax
    call print_msg
    
/* ======================================================
 *  load kernel using bios int 0x13, ah = 0x42
 *  kernel size: 128kb, kernel position: 0x10000
 *  read twice, 64kb per operation in size (64K boundary --> 80h sectors)
 * ====================================================== */
load_kernel:
    mov $0x42, %ah
    mov boot_dirver, %dl
    mov $packet, %si
    int $0x13
    jc fail

    addw $0x1000, packet.destination.segment
    addw $0x0080, packet.start
    int $0x13
    jc fail

    
/* ======================================================
 *  detect memory map using bios int 0x15, ax = 0xe820
 *  put the number of discriptors in 0x8000 (4bytes in size)
 *  followed with the array of descriptors
 * ====================================================== */
detect_mem:
    movl $0, 0x8000
    xor %ebx, %ebx
    mov $0x8000+4, %di
do_detect:
    mov $0x0000e820, %eax
    mov $0x534d4150, %edx
    mov $20, %ecx
    int $0x15
    jc fail
    add $20, %di
    incl 0x8000
    cmp $0, %ebx
    jnz do_detect

/* for fully using the space, moving kernel to 0x0 when no bios intr needed */
move_kernel:
    cli
    mov $0, %ax
    mov %ax, %es
    mov $0x1000, %ax
    mov %ax, %ds
    mov $0x8000, %cx
    xor %si, %si
    xor %di, %di
    rep
    movsw
    mov %ax, %es
    add $0x1000, %ax
    mov %ax, %ds
    mov $0x8000, %cx
    xor %si, %si
    xor %di, %di
    rep
    movsw


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
    mov $msg_fail, %ax
    call print_msg
die:    
    hlt
    jmp die

/* =====================================================
 *   real mode print, use bios int 0x10, ax = 0x1301
 *   %ax: string pointer
 *   %cx: character numbers
 * ===================================================== */
print_msg:
    push %ax
    push %cx

    /* read cursor position (ah = 0x03) */
    mov $0x03, %ah
    xor %bh, %bh
    int $0x10
    jc die

    pop %cx
    pop %ax

    mov $0x7, %bx           /* character attribution */
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
    jmp $0x8, $enter_kernel         /* clear pipe-line */

    .code32
enter_kernel:
    jmp 0

boot_dirver:
    .byte 0x0

msg_boot:
    .ascii "Loading system ...\r\n"

msg_fail:
    .ascii "Booting failed!\r\n"

packet:
packet.size:
    .byte 0x10
packet.reserved:
    .byte 0x0
packet.nr_block:
    .word 0x0080                    
packet.destination:
packet.destination.offset:  
    .word 0x0000
packet.destination.segment: 
    .word 0x1000
packet.start:
    .word 0x0001, 0x0, 0x0, 0x0

gdt_start:
    .word 0x0, 0x0, 0x0, 0x0

    /* code seg discriptor, 0 ~ 4GB*/
    .word 0xffff
    .word 0x0000
    .word 0x9a00
    .word 0x00cf

    /* data seg discriptor, 0 ~ 4GB */
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
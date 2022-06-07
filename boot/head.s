    .text
    .globl _gdt,_start
_start:
    movl $0x10, %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $stack_top, %esp
    mov %esp, %ebp

setup_pg:
.clear_mem:
    mov $1024*5, %ecx
    xor %eax, %eax
    mov $0, %edi
    cld;rep;stosl
.set_dir:
    movl $0x21007, 0x20000
    movl $0x22007, 0x20004
    movl $0x23007, 0x20008
    movl $0x24007, 0x2000c
.set_pg:
    mov $0x24ffc, %edi
    mov $0xfff007,  %eax    /* 16MB */
    std
1:  stosl
    sub $0x1000, %eax
    jge 1b 
    mov $0x20000, %eax
    mov %eax, %cr3
    mov %cr0, %eax
    orl $0x80000000, %eax
    mov %eax, %cr0

reload_gdt:
    lgdt gdt_descr
    call kernel_main

die:
    hlt
    jmp die

int_msg:    .asciz "Unknow interrupt\n"

_gdt:	
    .quad 0x0000000000000000
	.quad 0x00cf9a0000000fff
	.quad 0x00cf920000000fff
	.fill 253,8,0	

gdt_descr:
    .word 256*8-1
    .long _gdt

    .bss
    .align 16
stack_bottom:
    .skip 4096    /* 4kib */
stack_top:

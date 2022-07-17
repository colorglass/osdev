    .text
    .globl _start
_start:
    movl $0x10, %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    mov $stack_top, %esp
    mov %esp, %ebp
    call kernel_main

die:
    cli
    hlt
    jmp die

    .bss
    .align 16
stack_bottom:
    .skip 8192    /* 8kib */
stack_top:

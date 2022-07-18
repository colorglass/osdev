    .text
    .globl _start
_start:
    movl $0x10, %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

move_kernel:
    mov $0x10000, %esi
    mov $0x0, %edi
    mov $0x8000, %ecx
    rep;movsw
    mov $next_part, %eax
    jmp *%eax
next_part:
    mov $0x20000, %esi
    mov $0x10000, %edi
    mov $0x8000, %ecx
    rep;movsw

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

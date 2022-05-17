#ifndef _IO_H_
#define _IO_H_

#include <stdint.h>

#define outb(port, value) \
    asm("outb %%al, %%dx" ::"a"(value), "d"(port));

#define inb(port) ({              \
    uint8_t _v;                   \
    asm volatile("inb %%dx, %%al" \
                 : "=a"(_v)       \
                 : "d"(port));    \
    _v;                           \
})

#define io_wait asm("jmp 1f\n1:\tjmp 1f\n1:");

#define outw(port, value) \
    asm("outw %%ax, %%dx" ::"a"(value), "d"(port));

#define inw(port) ({              \
    uint16_t _v;                  \
    asm volatile("inw %%dx, %%ax" \
                 : "=a"(_v)       \
                 : "d"(port));    \
    _v;                           \
})

#endif
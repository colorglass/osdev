#include "interrupt.h"
#include "keyboard.h"
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xa0
#define PIC2_DATA 0xa1
#define PIC_EOI 0x20

#define IDT_SIZE 256

typedef struct
{
    uint16_t offset_lowbits;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_highbits;
} __attribute__((packed)) IDT_entry;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDT_register;

static IDT_entry IDT[IDT_SIZE];
static IDT_register IDT_reg;

void ignore_handler()
{
    asm volatile("iret");
}

void PIC_remap(int offset1, int offset2)
{
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQ)
{
    uint16_t port;
    uint8_t value;
    uint8_t IRQline = IRQ - 0x20;

    if (IRQline < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);
}

void IRQ_clear_mask(unsigned char IRQ)
{
    uint16_t port;
    uint8_t value;
    uint8_t IRQline = IRQ - 0x20;

    if (IRQline < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}

void PIC_init(void)
{
    PIC_remap(0x20, 0x28);
    IRQ_MASK_ALL();
}

void set_IDT_entry(int n, uint32_t handler)
{
    IDT[n].offset_lowbits = handler & 0xffff;
    IDT[n].selector = 0x08;
    IDT[n].zero = 0;
    IDT[n].type_attr = 0x8e;
    IDT[n].offset_highbits = (handler >> 16) & 0xffff;
}

void set_IDT(void)
{
    IDT_reg.base = (uint32_t)&IDT;
    IDT_reg.limit = IDT_SIZE * sizeof(IDT_entry) - 1;
    asm volatile("lidt (%0)\nsti\n" ::"r"(&IDT_reg));
}

void IDT_init(void)
{
    PIC_init();
    for (int i = 0; i < 32; i++)
    {
        set_IDT_entry(i, (uint32_t)ignore_handler);
    }
    set_IDT_entry(IRQ1, (uint32_t)keyboard_handler);
    set_IDT();
    IRQ_clear_mask(IRQ1);
}
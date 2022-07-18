#include "cpu/interrupt.h"
#include "device/terminal.h"
#include "sys/io.h"
#include "lib/string.h"

typedef struct
{
    uint16_t offset_lowbits;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_highbits;
} __attribute__((packed)) idt_entry;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register;

static idt_entry idt[IDT_SIZE];
static idt_register idt_reg;

char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

void set_idt_entry(int n, uint32_t handler)
{
    idt[n].offset_lowbits = handler & 0xffff;
    idt[n].selector = 0x08;
    idt[n].zero = 0;
    idt[n].type_attr = 0x8e;
    idt[n].offset_highbits = (handler >> 16) & 0xffff;
}

void set_idt(void)
{
    idt_reg.base = (uint32_t)&idt;
    idt_reg.limit = IDT_SIZE * sizeof(idt_entry) - 1;
    asm volatile("lidt (%0)\nsti\n" ::"r"(&idt_reg));
}

void isr_install(void)
{
    set_idt_entry(0, (uint32_t)isr0);
    set_idt_entry(1, (uint32_t)isr1);
    set_idt_entry(2, (uint32_t)isr2);
    set_idt_entry(3, (uint32_t)isr3);
    set_idt_entry(4, (uint32_t)isr4);
    set_idt_entry(5, (uint32_t)isr5);
    set_idt_entry(6, (uint32_t)isr6);
    set_idt_entry(7, (uint32_t)isr7);
    set_idt_entry(8, (uint32_t)isr8);
    set_idt_entry(9, (uint32_t)isr9);
    set_idt_entry(10, (uint32_t)isr10);
    set_idt_entry(11, (uint32_t)isr11);
    set_idt_entry(12, (uint32_t)isr12);
    set_idt_entry(13, (uint32_t)isr13);
    set_idt_entry(14, (uint32_t)isr14);
    set_idt_entry(15, (uint32_t)isr15);
    set_idt_entry(16, (uint32_t)isr16);
    set_idt_entry(17, (uint32_t)isr17);
    set_idt_entry(18, (uint32_t)isr18);
    set_idt_entry(19, (uint32_t)isr19);
    set_idt_entry(20, (uint32_t)isr20);
    set_idt_entry(21, (uint32_t)isr21);
    set_idt_entry(22, (uint32_t)isr22);
    set_idt_entry(23, (uint32_t)isr23);
    set_idt_entry(24, (uint32_t)isr24);
    set_idt_entry(25, (uint32_t)isr25);
    set_idt_entry(26, (uint32_t)isr26);
    set_idt_entry(27, (uint32_t)isr27);
    set_idt_entry(28, (uint32_t)isr28);
    set_idt_entry(29, (uint32_t)isr29);
    set_idt_entry(30, (uint32_t)isr30);
    set_idt_entry(31, (uint32_t)isr31);

    set_idt_entry(32, (uint32_t)irq0);
    set_idt_entry(33, (uint32_t)irq1);
    set_idt_entry(34, (uint32_t)irq2);
    set_idt_entry(35, (uint32_t)irq3);
    set_idt_entry(36, (uint32_t)irq4);
    set_idt_entry(37, (uint32_t)irq5);
    set_idt_entry(38, (uint32_t)irq6);
    set_idt_entry(39, (uint32_t)irq7);
    set_idt_entry(40, (uint32_t)irq8);
    set_idt_entry(41, (uint32_t)irq9);
    set_idt_entry(42, (uint32_t)irq10);
    set_idt_entry(43, (uint32_t)irq11);
    set_idt_entry(44, (uint32_t)irq12);
    set_idt_entry(45, (uint32_t)irq13);
    set_idt_entry(46, (uint32_t)irq14);
    set_idt_entry(47, (uint32_t)irq15);
}

isr_t interrupt_handlers[256];

void isr_handler(registers_t *r)
{
    terminal_writestring("received interrupt: ");
    char s[3];
    int_to_ascii(r->int_no, s);
    terminal_writestring(s);
    terminal_writestring("\n");
    terminal_writestring(exception_messages[r->int_no]);
    terminal_writestring("\n");
}

void irq_handler(registers_t *r)
{
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r->int_no >= 40)
        outb(0xA0, 0x20); /* slave */
    outb(0x20, 0x20);     /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r->int_no] != 0)
    {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}

void register_interrupt_handler(uint8_t n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

void interrupt_init()
{
    PIC_init();
    isr_install();
    set_idt();
}
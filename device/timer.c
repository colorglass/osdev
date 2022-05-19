#include "device/timer.h"
#include "sys/io.h"
#include "cpu/interrupt.h"
#include "device/terminal.h"

int tick = 999;

void timer_callback(registers_t *regs)
{
    if (tick-- < 0)
    {
        tick += 1000;
        terminal_writestring("per 1s\n");
    }
    (void *)regs;
}

void timer_init(uint32_t freq)
{
    register_interrupt_handler(IRQ0,timer_callback);

    uint16_t divider = 1193182 / freq;
    outb(0x43,0x36);
    outb(0x40,(uint8_t)(divider & 0xff));
    outb(0x40,(uint8_t)((divider & 0xff00) >> 8));

    IRQ_clear_mask(IRQ0);
}
#if !defined(_INTRRUPT_H_)
#define _INTRRUPT_H_

#include <stdint.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

#define IRQ_MASK_ALL()     \
    outb(PIC1_DATA, 0xff); \
    outb(PIC2_DATA, 0xff);

void PIC_init(void);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(unsigned char IRQ);
void IRQ_clear_mask(unsigned char IRQ);

void IDT_init(void);

#endif // _INTRRUPT_H_

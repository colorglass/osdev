#include <stdint.h>
#include "cpu/gdt.h"

typedef struct
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    unsigned limit_high : 4;
    unsigned flags : 4;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_register;

static gdt_entry gdt[GDT_SIZE];
static gdt_register gdt_reg;

void set_gdt_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[index].base_low = (uint16_t)(base & 0xffff);
    gdt[index].base_middle = (uint8_t)((base >> 16) & 0xff);
    gdt[index].base_high = (uint8_t)((base >> 24) & 0xff);
    gdt[index].limit_low = (uint16_t)(limit & 0xffff);
    gdt[index].limit_high = (uint8_t)((limit >> 16) & 0xff);
    gdt[index].access = access;
    gdt[index].flags = flags;
}

void set_gdt()
{
    gdt_reg.base = (uint32_t)&gdt;
    gdt_reg.limit = sizeof(gdt_entry) * GDT_SIZE - 1;
    asm volatile("lgdt (%0)" ::"r"(&gdt_reg));
}

void gdt_init()
{
    set_gdt_entry(0, 0, 0, 0, 0);
    set_gdt_entry(1, 0, 0xfffff, AC_RW | AC_EX | AC_DPL_KERN | AC_PR | AC_RE, GDT_GR | GDT_SZ);
    set_gdt_entry(2, 0, 0xfffff, AC_RW | AC_DPL_KERN | AC_PR | AC_RE, GDT_GR | GDT_SZ);

    set_gdt();
}
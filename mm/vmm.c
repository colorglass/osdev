#include <mm.h>
#include "lib/string.h"

#define LOW_MEMORY 0x100000
#define CR0_PG 0x80000000

#define PAGE_SIZE 0x1000
#define PDE_RECUSIVE_INDEX 0x3ff
#define KERNEL_HIGH_SPACE 0xc0000000

#define PAGE_DIR_ADDR(vaddr) (uint32_t *)(((vaddr) >> 20) & 0xfffff000)
#define PAGE_TABLE_ADDR(vaddr) (uint32_t *)(((vaddr) >> 10) & 0xfffff000)
#define PDE_INDEX(vaddr) (((vaddr) >> 22) & 0x3ff)
#define PTE_INDEX(vaddr) (((vaddr) >> 12) & 0x3ff)

uint32_t *pg_dir;

void vmm_switch_pgd(uint32_t pde)
{
    __asm__ volatile("mov %0, %%cr3"
                     :
                     : "r"(pde));
}

static inline void vmm_reflush(uint32_t va)
{
    __asm__ volatile("invlpg (%0)" ::"a"(va));
}

static inline void vmm_enable()
{
    uint32_t cr0;

    __asm__ volatile("mov %%cr0, %0"
                     : "=r"(cr0));
    cr0 |= CR0_PG;
    __asm__ volatile("mov %0, %%cr0"
                     :
                     : "r"(cr0));
}

uint32_t vmm_map(uint32_t vaddr, uint32_t paddr, uint32_t flags)
{
    uint32_t *page_dir = PAGE_DIR_ADDR(vaddr);
    uint32_t *page_table = PAGE_TABLE_ADDR(vaddr);
    uint32_t temp;
    if (!page_dir[PDE_INDEX(vaddr)])
    {
        if (!(temp = p_malloc()))
        {
            return 0;
        }
        page_dir[PDE_INDEX(vaddr)] = temp | flags;
    }
    page_table[PTE_INDEX(vaddr)] = paddr | flags;
    return vaddr;
}

uint32_t vmm_unmap(uint32_t vaddr)
{
    uint32_t *page_dir = PAGE_DIR_ADDR(vaddr);
    uint32_t *page_table = PAGE_TABLE_ADDR(vaddr);
    if (!page_dir[PDE_INDEX(vaddr)])
    {
        return 0;
    }
    p_free(page_table[PTE_INDEX(vaddr)]);
    page_table[PTE_INDEX(vaddr)] = 0;
    vmm_reflush(vaddr);
    return vaddr;
}

void page_fault_handle();

void vmm_init()
{
    pg_dir = (uint32_t *)p_malloc();
    uint32_t *page_table = (uint32_t *)p_malloc();
    for (uint32_t addr = 0; addr < LOW_MEMORY; addr += PAGE_SIZE)
    {
        page_table[PTE_INDEX(addr)] = addr | 0x7;
    }
    pg_dir[0] = (uint32_t)page_table | 0x7;
    pg_dir[PDE_INDEX(KERNEL_HIGH_SPACE)] = pg_dir[0];
    pg_dir[PDE_RECUSIVE_INDEX] = (uint32_t)pg_dir | 0x7;
    vmm_switch_pgd((uint32_t)pg_dir);
    vmm_enable();
}
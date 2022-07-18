#include <mm.h>
#include "lib/string.h"

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRYS (PAGE_SIZE >> 2)

static uint32_t *kernel_pg_dir;

int set_page_entry(uint32_t *pg_dir, uint32_t v_addr, uint32_t p_addr, uint8_t flags)
{
    uint32_t tmp, *page_table;
    page_table = pg_dir + (v_addr >> 22);
    if (!(*page_table & 1))
    {
        if (!(tmp = p_malloc()))
        {
            return 0;
        }
        *page_table = tmp | flags;
    }
    page_table = *page_table & 0xfffff000;
    page_table[(v_addr >> 12) & 0x3ff] = p_addr | flags;
    return 1;
}

void vmm_init()
{
    uint32_t pmm_map_end_addr = PMM_MAP_POS + pmm_map.size * 4;
    kernel_pg_dir = pmm_map_end_addr % PAGE_SIZE ? (pmm_map_end_addr + PAGE_SIZE) & 0xfffff000 : pmm_map_end_addr;
    memset(kernel_pg_dir, 0, PAGE_SIZE * 3);
}
/*
    use bitmap to manage physical memory, one page frame is only mapped to one page.
 */

#include "mm.h"
#include "bitmap.h"

#define MEM_NR 0x1000000
#define LOW_MB 0x100000

static bitmap mem_map[PAGE_INDEX(MEM_NR - LOW_MB) / 8] = {FREE};
static int next_pos;

void mem_init()
{
    next_pos = PAGE_INDEX(LOW_MB);
    bitmap_sets(mem_map, 0, PAGE_INDEX(LOW_MB) - 1);
}

/*
    physical memory manager
 */
uint32_t get_free_frame(void)
{
    int pos;
    if (!(pos = bitmap_scan(&mem_map, next_pos, PAGE_INDEX(MEM_NR) - 1, FREE)))
    {
        if (!(pos = bitmap_scan(&mem_map, PAGE_INDEX(LOW_MB), next_pos - 1, FREE)))
            return 0;
    }
    bitmap_set(&mem_map, pos);
    next_pos = pos + 1;
    return pos << 12;
}

void free_frame(uint32_t addr)
{
    if (addr < LOW_MB)
        return;
    if (addr >= MEM_NR)
        return;
    if (bitmap_test(&mem_map, addr >> 12))
        bitmap_clear(&mem_map, addr >> 12);
}

/*
    vitrual memory manager
 */

void set_page_entry(uint32_t v_addr)
{

}

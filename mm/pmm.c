/**
 * @file pmm.c 
 * @author colorglass (colorglass4@outlook.com)
 * @brief do the simple physical memory managent.
 * @todo more test, mutex 
 * @version 0.1
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "mm.h"

#include "lib/string.h"

#define MM_TYPE_AVAILABLE 0x1
#define MM_TYPE_RESERVED 0x2
#define MM_TYPE_ACPI_RECLAIM 0x3
#define MM_TYPE_ACPI_NVS 0x4

#define LOW_MEMORY_END 0x100000
#define PAGE_FRAME(addr) ((addr) >> 12)
#define PAGE_FRAMES(addr) PAGE_FRAME(addr)

#define MAP_INDEX(bit) ((bit) / 32)
#define MAP_OFFSET(bit) ((bit) % 32)
#define MAP_BIT(addr) PAGE_FRAME((addr)-LOW_MEMORY_END)

#define FRAME_ADDR(frame) ((frame) << 12)
#define BLOCK_ADDR(block) (FRAME_ADDR(block) + LOW_MEMORY_END)

#define FREE 0
#define USE 1

typedef struct
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
} __attribute__((packed)) mm_graphy_entry_t;

typedef struct
{
    uint32_t *entry_size;
    mm_graphy_entry_t *mm_list;
} mm_grapht_t;

void pmm_map_set(uint32_t bit)
{
    pmm_map.map[MAP_INDEX(bit)] |= (1 << MAP_OFFSET(bit));
}

void pmm_map_set_range(uint32_t from, uint32_t to)
{
    if (MAP_INDEX(to) == MAP_INDEX(from))
    {
        pmm_map.map[MAP_INDEX(from)] |= (0xffffffff << MAP_OFFSET(from)) & (0xffffffff >> (32 - MAP_OFFSET(to)));
    }
    else if (MAP_INDEX(to) > MAP_INDEX(from))
    {
        pmm_map.map[MAP_INDEX(from)] |= 0xffffffff << MAP_OFFSET(from);
        for (int i = 1; i < MAP_INDEX(to) - MAP_INDEX(from); i++)
        {
            pmm_map.map[MAP_INDEX(from) + i] = 0xffffffff;
        }
        pmm_map.map[MAP_INDEX(to)] |= 0xffffffff >> (32 - MAP_OFFSET(to));
    }
}

void pmm_map_unset(uint32_t bit)
{
    pmm_map.map[MAP_INDEX(bit)] &= ~(1 << MAP_OFFSET(bit));
}

void pmm_map_unset_range(uint32_t from, uint32_t to)
{
    if (MAP_INDEX(to) == MAP_INDEX(from))
    {
        pmm_map.map[MAP_INDEX(from)] &= ~((0xffffffff << MAP_OFFSET(from)) & (0xffffffff >> (32 - MAP_OFFSET(to))));
    }
    else if (MAP_INDEX(to) > MAP_INDEX(from))
    {
        pmm_map.map[MAP_INDEX(from)] &= ~(0xffffffff << MAP_OFFSET(from));
        for (int i = 1; i < MAP_INDEX(to) - MAP_INDEX(from); i++)
        {
            pmm_map.map[MAP_INDEX(from) + i] = 0;
        }
        pmm_map.map[MAP_INDEX(to)] &= ~(0xffffffff >> (32 - MAP_OFFSET(to)));
    }
}

uint8_t pmm_map_test(uint32_t bit)
{
    return pmm_map.map[MAP_INDEX(bit)] & (1 << MAP_OFFSET(bit));
}

uint32_t pmm_map_first_free()
{
    for (int i = 0; i < pmm_map.size; i++)
    {
        if (pmm_map.map[i] != 0xffffffff)
        {
            for (int j = 0; j < 32; j++)
            {
                int bit = 1 << j;
                if (!(pmm_map.map[i] & bit))
                {
                    return i * 32 + j;
                }
            }
        }
    }

    return 0;
}

uint32_t p_malloc()
{
    uint32_t p_addr = 0;
    if (pmm_map.free_block > 0)
    {
        p_addr = BLOCK_ADDR(pmm_map_first_free());
        pmm_map_set(MAP_BIT(p_addr));
        pmm_map.free_block--;
    }
    return p_addr;
}

void p_free(uint32_t p_addr)
{
    if (pmm_map_test(MAP_BIT(p_addr)) == USE)
    {
        pmm_map_unset(MAP_BIT(p_addr));
        pmm_map.free_block++;
    }
}

void pmm_init()
{
    uint32_t memory_end = 0;

    pmm_map.map = (uint32_t *)(PMM_MAP_POS);
    memset(pmm_map.map, 0xff, PMM_MAP_RESERVE_LENGTH);
    pmm_map.free_block = 0;

    mm_grapht_t mm_graphy = {
        .entry_size = (uint32_t *)0x80000,
        .mm_list = (mm_graphy_entry_t *)0x80004,
    };
    for (int i = 0; i < *mm_graphy.entry_size; i++)
    {

        if (mm_graphy.mm_list[i].base < LOW_MEMORY_END)
        {
            continue;
        }

        if (mm_graphy.mm_list[i].type == MM_TYPE_AVAILABLE)
        {
            memory_end = mm_graphy.mm_list[i].base + mm_graphy.mm_list[i].length;
            pmm_map_unset_range(MAP_BIT(mm_graphy.mm_list[i].base), MAP_BIT(memory_end));
            pmm_map.free_block += PAGE_FRAMES((uint32_t)mm_graphy.mm_list[i].length);
        }
    }

    pmm_map.size = MAP_INDEX(PAGE_FRAMES(memory_end)) + 1;
}
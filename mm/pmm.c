/**
 * @file pmm.c
 * @author colorglass (colorglass4@outlook.com)
 * @brief do the simple physical memory managent.
 * @todo more test, mutex ,faster search
 * @version 0.1
 * @date 2022-07-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "mm.h"

#include "lib/string.h"

#define PMM_MAP_POS 0x20000
#define PMM_MAP_RESERVE_LENGTH 0x20000

#define MM_TYPE_AVAILABLE 0x1
#define MM_TYPE_RESERVED 0x2
#define MM_TYPE_ACPI_RECLAIM 0x3
#define MM_TYPE_ACPI_NVS 0x4

#define FRAME_NUMBER(frame) ((frame) >> 12)
#define NUMBER_FRAME(frame) ((frame) << 12)
#define FRAME_NR(frame) FRAME_NUMBER(frame)

#define MAP_INDEX(bit) ((bit) / 32)
#define MAP_OFFSET(bit) ((bit) % 32)
#define MAP_BIT(addr) FRAME_NUMBER(addr)

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
    uint32_t *entry_nr;
    mm_graphy_entry_t *mm_list;
} mm_grapht_t;

typedef struct
{
    uint32_t *map;
    uint32_t map_nr;
    uint32_t free_frame;
} pmm_map_t;

static pmm_map_t pmm_map;

/* sikp the kernel reserved space */
static uint32_t recent_req_map_index = MAP_INDEX(MAP_BIT(PMM_MAP_POS));

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
    for (int i = recent_req_map_index; i < pmm_map.map_nr; i++)
    {
        if (pmm_map.map[i] != 0xffffffff)
        {
            for (int j = 0; j < 32; j++)
            {
                int bit = 1 << j;
                if (!(pmm_map.map[i] & bit))
                {
                    recent_req_map_index = i;
                    return i * 32 + j;
                }
            }
        }
    }
    for (int i = MAP_INDEX(PMM_MAP_POS); i < recent_req_map_index; i++)
    {
        if (pmm_map.map[i] != 0xffffffff)
        {
            for (int j = 0; j < 32; j++)
            {
                int bit = 1 << j;
                if (!(pmm_map.map[i] & bit))
                {
                    recent_req_map_index = i;
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
    if (pmm_map.free_frame > 0)
    {
        p_addr = NUMBER_FRAME(pmm_map_first_free());
        pmm_map_set(MAP_BIT(p_addr));
        pmm_map.free_frame--;
    }
    return p_addr;
}

void p_free(uint32_t p_addr)
{
    if (pmm_map_test(MAP_BIT(p_addr)) == USE)
    {
        pmm_map_unset(MAP_BIT(p_addr));
        pmm_map.free_frame++;
    }
}

void pmm_init()
{
    uint32_t memory_end = 0;

    pmm_map.map = (uint32_t *)(PMM_MAP_POS);
    memset(pmm_map.map, 0xff, PMM_MAP_RESERVE_LENGTH);
    pmm_map.free_frame = 0;

    mm_grapht_t mm_graphy = {
        .entry_nr = (uint32_t *)0x80000,
        .mm_list = (mm_graphy_entry_t *)0x80004,
    };
    for (int i = 0; i < *mm_graphy.entry_nr; i++)
    {
        if (mm_graphy.mm_list[i].type == MM_TYPE_AVAILABLE)
        {
            memory_end = mm_graphy.mm_list[i].base + mm_graphy.mm_list[i].length;
            pmm_map_unset_range(MAP_BIT(mm_graphy.mm_list[i].base), MAP_BIT(memory_end));
            pmm_map.free_frame += FRAME_NR((uint32_t)mm_graphy.mm_list[i].length);
        }
    }

    pmm_map.map_nr = MAP_INDEX(FRAME_NUMBER(memory_end)) + 1;
    pmm_map_set_range(MAP_BIT(0), MAP_BIT(PMM_MAP_POS + pmm_map.map_nr * 32) + 1);
}
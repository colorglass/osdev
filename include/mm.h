#ifndef __MM_H__
#define __MM_H__

#include <stdint.h>

#define PMM_MAP_POS 0x20000
#define PMM_MAP_RESERVE_LENGTH 0x20000

typedef struct
{
    uint32_t *map;
    uint32_t size;
    uint32_t free_block;
} pmm_map_t;

static pmm_map_t pmm_map;

void pmm_init();
uint32_t p_malloc();
void p_free(uint32_t p_addr);

#endif
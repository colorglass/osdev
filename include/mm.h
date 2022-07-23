#ifndef __MM_H__
#define __MM_H__

#include <stdint.h>

void pmm_init();
uint32_t p_malloc();
void p_free(uint32_t p_addr);

void vmm_init();

#endif
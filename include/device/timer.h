#if !defined(_TIME_H)
#define _TIME_H

#include <stdint.h>

extern int tick;

void timer_init(uint32_t freq);

#endif // _TIME_H

#include <stdint.h>

typedef uint8_t bitmap;

uint8_t bitmap_test(bitmap *map, int bit_pos);
void bitmap_set(bitmap *map, int bit_pos);
void bitmap_sets(bitmap *map, int start, int end);
void bitmap_clear(bitmap *map, int bit_pos);
void bitmap_clears(bitmap *map, int start, int end);
int bitmap_scan(bitmap *map, int start, int end, uint8_t bit);
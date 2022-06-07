#include "bitmap.h"

#define INDEX(i) ((i) / 8)
#define COUNT(i) ((i) % 8)

/*
    test for bit in pos
 */
uint8_t bitmap_test(bitmap *map, int pos)
{
    if (!map)
        return;

    if (map[INDEX(pos)] && 1 << COUNT(pos))
        return 1;
    else
        return 0;
}

void bitmap_set(bitmap *map, int pos)
{
    if (!map)
        return;

    map[INDEX(pos)] |= 1 << COUNT(pos);
}

/*
    set bits in range from start to end (included)
 */
void bitmap_sets(bitmap *map, int start, int end)
{
    if (!map)
        return;

    int len;
    uint8_t padding = 0;

    int start_index = INDEX(start);
    int end_index = INDEX(end);

    if (start_index == end_index)
    {
        len = end - start + 1;
        while (len--)
        {
            padding <<= 1;
            padding |= 0x1;
        }

        map[start_index] |= padding << COUNT(start);
    }
    else if (start_index < end_index)
    {
        len = 8 - COUNT(start);
        while (len--)
        {
            padding <<= 1;
            padding |= 0x1;
        }

        map[start_index++] |= padding << COUNT(start);

        len = COUNT(end) + 1;
        padding = 0;
        while (len--)
        {
            padding <<= 1;
            padding |= 0x1;
        }

        map[end_index] |= padding;

        while (start_index < end_index)
        {
            map[start_index++] = 0xff;
        }
    }
}

void bitmap_clear(bitmap *map, int pos)
{
    if (!map)
        return;

    map[INDEX(pos)] &= ~(1 << COUNT(pos));
}

/*
    clear bits in range from start to end (included)
 */
void bitmap_clears(bitmap *map, int start, int end)
{
    if (!map)
        return;

    int len;
    uint8_t padding = 0;

    int start_index = INDEX(start);
    int end_index = INDEX(end);

    if (start_index == end_index)
    {
        len = end - start + 1;
        while (len--)
        {
            padding <<= 1;
            padding |= 0x1;
        }

        map[start_index] &= ~(padding << COUNT(start));
    }
    else if (start_index < end_index)
    {
        len = 8 - COUNT(start);
        while (len--)
        {
            padding <<= 1;
            padding |= 0x1;
        }

        map[start_index++] &= ~(padding << COUNT(start));

        len = COUNT(end) + 1;
        padding = 0;
        while (len--)
        {
            padding <<= 1;
            padding |= 0x1;
        }

        map[end_index] &= ~padding;

        while (start_index < end_index)
        {
            map[start_index++] = 0;
        }
    }
}

/* scan for the first bit from start to end (included), return position */
int bitmap_scan(bitmap *map, int start, int end, uint8_t bit)
{
    int i = INDEX(start);
    int j = COUNT(start);

    if (!map)
        return;

    while (bit ^ ((map[i] & (1 << j)) >> j))
    {
        if (++j == 8)
        {
            j = 0;
            i++;
        }
        if (i * 8 + j > end)
        {
            return 0;
        }
    }
    return i * 8 + j;
}
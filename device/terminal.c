#include "string.h"
#include "device/terminal.h"
#include "sys/io.h"

#define index(col, row) ((col) + (row)*VGA_WIDTH)

static uint8_t terminal_row;
static uint8_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, uint8_t col, uint8_t row)
{
    terminal_buffer[index(col, row)] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
    if (c == '\n')
    {
        terminal_column = 0;
        if (++terminal_row >= VGA_HEIGHT)
        {
            terminal_backword(1);
            terminal_row--;
        }
    }
    else
    {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column >= VGA_WIDTH)
        {
            terminal_column = 0;
            if (++terminal_row >= VGA_HEIGHT)
            {
                terminal_backword(1);
                terminal_row--;
            }
        }
    }
}

void terminal_write(const char *data, int size)
{
    for (int i = 0; i < size; i++)
    {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char *data)
{
    int size = 0;
    for (; data[size] != '\0'; size++)
        ;
    terminal_write(data, size);
    set_cursor(terminal_column, terminal_row);
}

void terminal_initial()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t *)VGA_TEXT_BUFFER;

    clear_all();
    set_cursor(terminal_column, terminal_row);
}

void set_cursor(uint8_t col, uint8_t row)
{
    uint16_t pos = row * VGA_WIDTH + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_backspace()
{
    if (terminal_column == 0)
    {
        if (terminal_row != 0)
        {
            terminal_column = VGA_WIDTH - 1;
            terminal_row--;
        }
    }
    else
    {
        terminal_column--;
    }
    terminal_putentryat(0x08, terminal_color, terminal_column, terminal_row);
    set_cursor(terminal_column, terminal_row);
}

void terminal_backword(uint8_t step)
{
    terminal_backword_line(step, 0, VGA_HEIGHT - 1);
}

void terminal_backword_line(uint8_t step, uint8_t start, uint8_t end)
{
    if (start > end)
    {
        return;
    }

    if (step > end)
    {
        clear_area(start, end);
        return;
    }
    else if (step > start)
    {
        start = step;
    }

    memcpy(&terminal_buffer[(start - step) * VGA_WIDTH],
           &terminal_buffer[start * VGA_WIDTH],
           (end - start + 1) * VGA_WIDTH);
}

void clear_area(uint8_t start, uint8_t end)
{
    for (int i = start * VGA_WIDTH; i < (end + 1) * VGA_WIDTH; i++)
    {
        terminal_buffer[i] = vga_entry(' ', terminal_color);
    }
}

void clear_all()
{
    clear_area(0, VGA_HEIGHT - 1);
    set_cursor(0, 0);
}

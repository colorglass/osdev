#include "string.h"
#include "sys/io.h"
#include "device/terminal.h"

#define VGA_TEXT_BUFFER 0xB8000
#define CRT_ADDR_PORT 0x3D4
#define CRT_DATA_PORT 0x3D5
#define CURSOR_POS_HIGH_INDEX 0x0E
#define CURSOR_POS_LOW_INDEX 0x0F

static uint8_t terminal_row;
static uint8_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

static uint8_t end_col[25] = {0};

static inline uint8_t
vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

static inline uint16_t get_pos(uint8_t col, uint8_t row)
{
    return col + row * VGA_WIDTH;
}

static inline uint8_t get_column(uint16_t pos)
{
    return pos % VGA_WIDTH;
}

static inline uint8_t get_row(uint16_t pos)
{
    return pos / VGA_WIDTH;
}

static void bs()
{
    if (terminal_column == 0)
    {
        if (terminal_row != 0)
        {
            terminal_column = end_col[--terminal_row];
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
    }
    else
    {
        terminal_column--;
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    }
}

static void lf()
{
    end_col[terminal_row] = terminal_column;
    if (++terminal_row >= VGA_HEIGHT)
    {
        terminal_scroll_up();
        terminal_row = VGA_HEIGHT - 1;
    }
}

static inline void cr()
{
    terminal_column = 0;
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, uint8_t col, uint8_t row)
{
    uint16_t index = get_pos(col, row);
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
    if (c == '\n')
    {
        lf();
        cr();
    }
    else if (c == '\b')
    {
        bs();
    }
    else if (c == '\t')
    {
        terminal_writestring("    ");
    }
    else if (c == '\r')
    {
        cr();
    }
    else if (c >= 0x20 && c <= 0x7e)
    {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column >= VGA_WIDTH)
        {
            terminal_column = 0;
            end_col[terminal_row] = VGA_WIDTH - 1;
            if (++terminal_row >= VGA_HEIGHT)
            {
                terminal_scroll_up();
                terminal_row = VGA_HEIGHT - 1;
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
    set_cursor(get_pos(terminal_column, terminal_row));
}

void terminal_writestring(const char *data)
{
    int size = strlen(data);
    terminal_write(data, size);
}

void terminal_initial()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t *)VGA_TEXT_BUFFER;

    terminal_clear();
    set_cursor(0);
}

void set_cursor(uint16_t pos)
{
    outb(CRT_ADDR_PORT, CURSOR_POS_LOW_INDEX);
    outb(CRT_DATA_PORT, (uint8_t)(pos & 0xFF));
    outb(CRT_ADDR_PORT, CURSOR_POS_HIGH_INDEX);
    outb(CRT_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));
}

uint16_t get_cursor()
{
    uint16_t pos = 0;
    outb(CRT_ADDR_PORT, CURSOR_POS_LOW_INDEX);
    pos |= inb(CRT_DATA_PORT);
    outb(CRT_ADDR_PORT, CURSOR_POS_HIGH_INDEX);
    pos |= (uint16_t)inb(CRT_DATA_PORT) << 8;
    return pos;
}

void terminal_scroll_up()
{
    terminal_scroll_up_line(1, 0, VGA_HEIGHT - 1);
    for (int i = 0; i < VGA_HEIGHT - 1; i++)
    {
        end_col[i] = end_col[i + 1];
    }
}

void scroll_down()
{
}

void terminal_scroll_up_line(uint8_t step, uint8_t start, uint8_t end)
{
    if (start > end)
    {
        return;
    }

    if (step > end)
    {
        terminal_clear_area(start, end);
        return;
    }
    else if (step > start)
    {
        start = step;
    }

    memcpy(&terminal_buffer[(start - step) * VGA_WIDTH],
           &terminal_buffer[start * VGA_WIDTH],
           (end - start + 1) * VGA_WIDTH * 2);

    terminal_clear_area(end - step + 1, end);
}

void terminal_clear_area(uint8_t start, uint8_t end)
{
    for (int i = start * VGA_WIDTH; i < (end + 1) * VGA_WIDTH; i++)
    {
        terminal_buffer[i] = vga_entry(' ', terminal_color);
    }
}

void terminal_clear()
{
    terminal_clear_area(0, VGA_HEIGHT - 1);
}

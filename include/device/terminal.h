#ifndef _TERMINAL_H_
#define _TERMIANL_H_

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

void terminal_initial();
void terminal_putentryat(char c, uint8_t color, uint8_t col, uint8_t row);
void terminal_putchar(char c);
void terminal_write(const char *data, int size);
void terminal_writestring(const char *data);
void terminal_clear();
void terminal_clear_area(uint8_t start, uint8_t end);
void terminal_scroll_up();
void terminal_scroll_up_line(uint8_t step, uint8_t start, uint8_t end);
void terminal_setcolor(uint8_t color);
void set_cursor(uint16_t pos);
uint16_t get_cursor();

#endif
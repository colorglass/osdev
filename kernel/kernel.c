#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <terminal.h>

void kernel_main(void)
{
	terminal_initial();

	terminal_writestring("Hello, kernel World!\nHello, my world!");

	while (1)
		;
}
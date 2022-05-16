#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "terminal.h"
#include "interrupt.h"

void kernel_main(void)
{
	terminal_initial();

	terminal_writestring("Hello, kernel World!\nHello, my world!\n");

	IDT_init();
	while (1)
		;
}
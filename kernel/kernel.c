#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cpu/interrupt.h"
#include "device/terminal.h"
#include "device/keyboard.h"

void kernel_main(void)
{
	terminal_initial();

	terminal_writestring("Hello, kernel World!\nHello, my world!\n");

	interrupt_init();

	keyboard_init();

	while (1)
		;
}
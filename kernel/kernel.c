#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cpu/interrupt.h"
#include "cpu/gdt.h"
#include "device/timer.h"
#include "device/terminal.h"
#include "device/keyboard.h"
#include "string.h"

void kernel_main(void)
{
	terminal_initial();
	gdt_init();
	interrupt_init();
	keyboard_init();
	timer_init(1000);

	terminal_writestring("Hello, kernel World!\nHello, my world!\n");

	while (1)
		;
}

#include <stdint.h>

#include "sys/io.h"
#include "cpu/interrupt.h"
#include "device/terminal.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

#define SC_MAX 57

const char *sc_name[] = {"ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                         "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                         "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                         "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                         "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                         "/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = {'?', '?', '1', '2', '3', '4', '5', '6',
                         '7', '8', '9', '0', '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
                         'U', 'I', 'O', 'P', '[', ']', '\n', '?', 'A', 'S', 'D', 'F', 'G',
                         'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
                         'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void keyboard_callback(registers_t *regs)
{
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = inb(0x60);

    if (scancode > SC_MAX)
        return;
    else
    {
        char letter = sc_ascii[(int)scancode];
        terminal_write(&letter, 1);
    }
}

void keyboard_init()
{
    register_interrupt_handler(IRQ1, keyboard_callback);
    IRQ_clear_mask(IRQ1);
}
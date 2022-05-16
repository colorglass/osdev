    .globl ignore_handler, keyboard_handler_wrapper
    .text
ignore_handler:
    iret

keyboard_handler_wrapper:
    call keyboard_handler
    iret

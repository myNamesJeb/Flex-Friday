#ifndef KEYBOARD_H
#define KEYBOARD_H

// The C keyboard interrupt handler (called by the ISR stub)
void keyboard_handler(void);

// Blocking function to get a character from the keyboard buffer.
char keyboard_getc(void);

#endif

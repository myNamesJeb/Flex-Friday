#include "drivers/keyboard.h"
#include "fs/io.h"

#define KB_BUFFER_SIZE 256

volatile char kb_buffer[KB_BUFFER_SIZE];
volatile int kb_head = 0;  // write index
volatile int kb_tail = 0;  // read index

// Externally defined by your VGA output code:
extern void putc(char c);
extern volatile char *VIDEO_MEMORY;
extern int cursor_pos;  // if used by your putc()/printf routines

// Simple US QWERTY mapping (extend as needed)
static char scancode_map[128] = {
    0, 0, '1','2','3','4','5','6',
    '7','8','9','0','-','=', '\b', '\t',
    'q','w','e','r','t','y','u','i',
    'o','p','[',']','\n', 0, 'a','s',
    'd','f','g','h','j','k','l',';',
    '\'', '`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0
};

static int shift_pressed = 0;

static char scancode_to_char(unsigned char scancode) {
    // (For now ignore shift; you can extend by adding a separate map)
    return scancode_map[scancode];
}

static void kb_enqueue(char c) {
    int next = (kb_head + 1) % KB_BUFFER_SIZE;
    if(next != kb_tail) {  // if the buffer is not full
        kb_buffer[kb_head] = c;
        kb_head = next;
    }
}

// This is called by your keyboard assembly ISR stub.
void keyboard_handler(void) {
    unsigned char scancode = inb(0x60);
    // Only process key press events (scancode < 0x80)
    if(scancode < 0x80) {
        char key = scancode_to_char(scancode);
        kb_enqueue(key);
    }
    else {
        // On key release events, update shift state if needed.
        unsigned char code = scancode & 0x7F;
        if(code == 0x2A || code == 0x36)
            shift_pressed = 0;
    }
}

// Called by shell.c: blocks until a keystroke is available.
char keyboard_getc(void) {
    while(kb_head == kb_tail) {
        // Optionally halt CPU briefly to reduce busy-waiting.
    }
    char c = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    return c;
}

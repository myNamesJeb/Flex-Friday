#include "screen.h"
#include "io.h"  // Add this include for outb()

#define VIDEO_MEMORY ((unsigned short*)0xb8000)
#define MAX_ROWS 25
#define MAX_COLS 80

static int cursor_row = 0;
static int cursor_col = 0;

// Replace update_cursor with an implementation that updates the VGA cursor.
static void update_cursor() {
    unsigned short pos = cursor_row * MAX_COLS + cursor_col;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void scroll_screen(void) {
    unsigned short blank = 0x20 | (0x07 << 8); // space with attribute
    if(cursor_row < MAX_ROWS)
        return;
    for (int row = 1; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            VIDEO_MEMORY[(row - 1) * MAX_COLS + col] = VIDEO_MEMORY[row * MAX_COLS + col];
        }
    }
    for (int col = 0; col < MAX_COLS; col++) {
        VIDEO_MEMORY[(MAX_ROWS - 1) * MAX_COLS + col] = blank;
    }
    // DEBUG: Write a marker at the start of the last row
    VIDEO_MEMORY[(MAX_ROWS - 1) * MAX_COLS + 0] = 'S' | (0x07 << 8);

    cursor_row = MAX_ROWS - 1;
    update_cursor();
}

void put_character(char c) {
    unsigned short blank = 0x20 | (0x07 << 8);
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else {
        VIDEO_MEMORY[cursor_row * MAX_COLS + cursor_col] = c | (0x07 << 8);
        cursor_col++;
        if(cursor_col >= MAX_COLS) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if(cursor_row >= MAX_ROWS) {
        scroll_screen();
    }
    update_cursor();
}
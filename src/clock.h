/* src/clock.h */
#ifndef CLOCK_H
#define CLOCK_H

typedef struct {
    int hour;
    int minute;
    int second;
} Time;

Time get_rtc_time();
void print_time(); // Convenience function

#endif

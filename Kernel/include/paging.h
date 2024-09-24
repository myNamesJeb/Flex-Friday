// paging.h
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// Function to initialize paging
void paging_init();

// External function to load paging structures
void paging_load(uint64_t);

#endif

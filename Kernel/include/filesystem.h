// filesystem.h
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// Function to create a file
int fs_create_file(const char* name, const char* content);

// Function to read a file's content
int fs_read_file(const char* name, char* buffer);

// Function to initialize the filesystem
void fs_init();

#endif

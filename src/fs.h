#ifndef FS_H
#define FS_H

#define MAX_NAME_LEN   64
#define MAX_FILE_SIZE  4096
#define MAX_NODES      128
#define MAX_CHILDREN   16

typedef enum {
    FS_NODE_FILE,
    FS_NODE_DIR
} FSNodeType;

typedef struct FSNode {
    char name[MAX_NAME_LEN];
    FSNodeType type;
    char data[MAX_FILE_SIZE]; // used only for files
    int size;
    struct FSNode* parent;
    struct FSNode* children[MAX_CHILDREN];
    int child_count;
} FSNode;

extern FSNode* fs_root; // defined in fs.c

// Basic FS operations
void fs_init();
FSNode* fs_find(const char *path);
int fs_create_file(const char *path);
int fs_create_dir(const char *path);
void fs_ls(const char *path);
void fs_cat(const char *path);
int fs_write(const char *path, const char *content);

// Utilities for path resolution and printing the path
FSNode* fs_resolve(FSNode *start, const char *path);
void fs_get_path(FSNode *node, char *out, int out_size);

#endif

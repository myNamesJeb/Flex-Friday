#include "fs/fs.h"
#include "lib/stdio.h"
#include "lib/string.h"

static FSNode fs_nodes[MAX_NODES];
static int fs_node_count = 0;

FSNode* fs_root = 0;  // Global root node

// Allocate a new node from our static array.
static FSNode* allocate_node(const char* name, FSNodeType type, FSNode* parent) {
    if (fs_node_count >= MAX_NODES)
        return 0;
    FSNode* node = &fs_nodes[fs_node_count++];
    strcpy(node->name, name);
    node->type = type;
    node->size = 0;
    node->child_count = 0;
    node->parent = parent;
    return node;
}

// Initialize the filesystem: create root and default directories.
void fs_init() {
    fs_node_count = 0;
    fs_root = allocate_node("/", FS_NODE_DIR, 0);
    if (!fs_root) return;
    fs_create_dir("/boot");
    fs_create_dir("/etc");
    fs_create_dir("/home");
    fs_create_dir("/bin");

    // Preload a sample file in /boot (your kernel, etc.)
    fs_create_file("/boot/kernel.bin");
    fs_write("/boot/kernel.bin", "This is the OS code (simulated).");
}

// Find a node given an absolute path (starting with '/').
FSNode* fs_find(const char *path) {
    if (!path || path[0] != '/')
        return 0;
    if (strcmp(path, "/") == 0)
        return fs_root;
    FSNode* current = fs_root;
    char temp[MAX_NAME_LEN];
    int i = 1, j = 0;
    while (path[i]) {
        if (path[i] == '/') {
            if (j == 0) { i++; continue; }
            temp[j] = '\0';
            int found = 0;
            for (int k = 0; k < current->child_count; k++) {
                if (strcmp(current->children[k]->name, temp) == 0) {
                    current = current->children[k];
                    found = 1;
                    break;
                }
            }
            if (!found) return 0;
            j = 0;
            i++;
        } else {
            if(j < MAX_NAME_LEN - 1)
                temp[j++] = path[i++];
            else
                i++;
        }
    }
    if (j > 0) {
        temp[j] = '\0';
        int found = 0;
        for (int k = 0; k < current->child_count; k++) {
            if (strcmp(current->children[k]->name, temp) == 0) {
                current = current->children[k];
                found = 1;
                break;
            }
        }
        if (!found) return 0;
    }
    return current;
}

int fs_create_file(const char *path) {
    if (!path || path[0] != '/')
        return -1;
    char parent_path[MAX_NAME_LEN];
    char file_name[MAX_NAME_LEN];
    int len = strlen(path);
    int last_slash = -1;
    for (int i = 0; i < len; i++) {
        if (path[i] == '/')
            last_slash = i;
    }
    if (last_slash < 0)
        return -1;
    if (last_slash == 0)
        strcpy(parent_path, "/");
    else {
        strncpy(parent_path, path, last_slash);
        parent_path[last_slash] = '\0';
    }
    strcpy(file_name, path + last_slash + 1);
    FSNode* parent = fs_find(parent_path);
    if (!parent || parent->type != FS_NODE_DIR)
        return -1;
    // Check if file already exists.
    for (int i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, file_name) == 0)
            return -1;
    }
    FSNode* file = allocate_node(file_name, FS_NODE_FILE, parent);
    if (!file)
        return -1;
    parent->children[parent->child_count++] = file;
    return 0;
}

int fs_create_dir(const char *path) {
    if (!path || path[0] != '/')
        return -1;
    char parent_path[MAX_NAME_LEN];
    char dir_name[MAX_NAME_LEN];
    int len = strlen(path);
    int last_slash = -1;
    for (int i = 0; i < len; i++) {
        if (path[i] == '/')
            last_slash = i;
    }
    if (last_slash < 0)
        return -1;
    if (last_slash == 0)
        strcpy(parent_path, "/");
    else {
        strncpy(parent_path, path, last_slash);
        parent_path[last_slash] = '\0';
    }
    strcpy(dir_name, path + last_slash + 1);
    FSNode* parent = fs_find(parent_path);
    if (!parent || parent->type != FS_NODE_DIR)
        return -1;
    // Check if directory already exists.
    for (int i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, dir_name) == 0)
            return -1;
    }
    FSNode* dir = allocate_node(dir_name, FS_NODE_DIR, parent);
    if (!dir)
        return -1;
    parent->children[parent->child_count++] = dir;
    return 0;
}

void fs_ls(const char *path) {
    FSNode* node = fs_find(path);
    if (!node) {
        printf("Directory not found: %s\n", path);
        return;
    }
    if (node->type != FS_NODE_DIR) {
        printf("%s is not a directory\n", path);
        return;
    }
    for (int i = 0; i < node->child_count; i++) {
        printf("%s ", node->children[i]->name);
    }
    printf("\n");
}

void fs_cat(const char *path) {
    FSNode* node = fs_find(path);
    if (!node) {
        printf("File not found: %s\n", path);
        return;
    }
    if (node->type != FS_NODE_FILE) {
        printf("%s is not a file\n", path);
        return;
    }
    printf("%s\n", node->data);
}

int fs_write(const char *path, const char *content) {
    FSNode* node = fs_find(path);
    if (!node || node->type != FS_NODE_FILE)
        return -1;
    strncpy(node->data, content, MAX_FILE_SIZE);
    node->data[MAX_FILE_SIZE - 1] = '\0';
    node->size = strlen(node->data);
    return 0;
}

int fs_read(const char *path, char *buffer, int max_size) {
    FSNode* node = fs_find(path);
    if (!node || node->type != FS_NODE_FILE)
        return -1;
    int to_copy = node->size < max_size ? node->size : max_size - 1;
    strncpy(buffer, node->data, to_copy);
    buffer[to_copy] = '\0';
    return to_copy;
}

int fs_rm(const char *path) {
    FSNode *node = fs_find(path);
    if (!node || node == fs_root) return -1;
    // Remove node from parent's children
    // (Assume you have a parent pointer and a children list)
    // ...implement removal logic...
    return 0;
}

int fs_mv(const char *src, const char *dst) {
    FSNode *node = fs_find(src);
    if (!node) return -1;
    // Remove from old parent, add to new parent with new name
    // ...implement move/rename logic...
    return 0;
}

// void fs_get_path(FSNode *node, char *buffer, int max_size) {
//     if (!node || !buffer) return;
//     char temp[MAX_NAME_LEN];
//     int len = 0;
//     while (node) {
//         snprintf(temp, sizeof(temp), "/%s", node->name);
//         int temp_len = strlen(temp);
//         if (len + temp_len < max_size) {
//             memmove(buffer + temp_len, buffer, len);
//             memcpy(buffer, temp, temp_len);
//             len += temp_len;
//         }
//         node = node->parent;
//     }
//     buffer[len] = '\0';
// }

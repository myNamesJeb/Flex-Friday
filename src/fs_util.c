#include "fs.h"
#include "string.h"
#include "stdio.h"
#include "stddef.h"
// Helper: Split a path into segments (up to 16 segments).
static int split_path(const char *path, char segments[][MAX_NAME_LEN], int max_segments) {
    int seg_count = 0;
    int i = 0, j = 0;
    while (path[i] != '\0') {
        if (path[i] == '/') {
            if (j > 0) {
                segments[seg_count][j] = '\0';
                seg_count++;
                j = 0;
                if (seg_count >= max_segments)
                    break;
            }
        } else {
            if (j < MAX_NAME_LEN - 1)
                segments[seg_count][j++] = path[i];
        }
        i++;
    }
    if (j > 0 && seg_count < max_segments) {
        segments[seg_count][j] = '\0';
        seg_count++;
    }
    return seg_count;
}

// Resolve a path relative to 'start'. If path is absolute, start from fs_root.
FSNode* fs_resolve(FSNode *start, const char *path) {
    if (!path || !fs_root)
        return NULL;
    FSNode *current = (path[0] == '/') ? fs_root : start;
    char segments[16][MAX_NAME_LEN];
    int seg_count = split_path(path, segments, 16);
    for (int i = 0; i < seg_count; i++) {
        if (strcmp(segments[i], ".") == 0) {
            continue;
        } else if (strcmp(segments[i], "..") == 0) {
            if (current != fs_root && current->parent)
                current = current->parent;
        } else {
            if (current->type != FS_NODE_DIR)
                return NULL;
            FSNode *found = NULL;
            for (int j = 0; j < current->child_count; j++) {
                if (strcmp(current->children[j]->name, segments[i]) == 0) {
                    found = current->children[j];
                    break;
                }
            }
            if (!found)
                return NULL;
            current = found;
        }
    }
    return current;
}

// Construct the absolute path of a node into 'out'.
void fs_get_path(FSNode *node, char *out, int out_size) {
    if (!node) {
        if (out_size > 0)
            out[0] = '\0';
        return;
    }
    // Base case: if node is root, return "/"
    if (node == fs_root) {
        strncpy(out, "/", out_size);
        out[out_size - 1] = '\0';
        return;
    }
    // Recursively get parent's path.
    char parent_path[256];
    fs_get_path(node->parent, parent_path, sizeof(parent_path));
    if (strcmp(parent_path, "/") != 0) {
        strncat(parent_path, "/", sizeof(parent_path) - strlen(parent_path) - 1);
    }
    strncat(parent_path, node->name, sizeof(parent_path) - strlen(parent_path) - 1);
    strncpy(out, parent_path, out_size);
    out[out_size - 1] = '\0';
}

#include "omnisave.h"

void ensure_dir(const char* path) {
    char temp[MAX_PATH_LEN];
    char* p = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if (temp[len - 1] == '\\' || temp[len - 1] == '/') temp[len - 1] = 0;
    for (p = temp + 1; *p; p++) {
        if (*p == '\\' || *p == '/') {
            char sep = *p;
            *p = 0;
            p_create_directory(temp);
            *p = sep;
        }
    }
    p_create_directory(temp);
}

int sync_folders(const char* src, const char* dst) {
    char search_path[MAX_PATH_LEN];
    PFileInfo info;
    PHandle hFind = NULL;

    log_info("Syncing: %s -> %s", src, dst);
    ensure_dir(dst);

    snprintf(search_path, MAX_PATH_LEN, "%s\\*", src);
    hFind = p_find_first(search_path, &info);

    if (hFind == NULL) {
        log_error("Could not open source directory: %s", src);
        return 0;
    }

    int all_success = 1;

    do {
        // Skip current dir, parent dir, AND macOS hidden AppleDouble files
        if (strcmp(info.name, ".") == 0 || 
            strcmp(info.name, "..") == 0 ||
            strncmp(info.name, "._", 2) == 0) { 
            continue;
        }

        char src_file[MAX_PATH_LEN];
        char dst_file[MAX_PATH_LEN];
        snprintf(src_file, MAX_PATH_LEN, "%s\\%s", src, info.name);
        snprintf(dst_file, MAX_PATH_LEN, "%s\\%s", dst, info.name);

        if (info.is_dir) {
            if (!sync_folders(src_file, dst_file)) {
                all_success = 0;
            }
        } else {
            int should_copy = 1;
            unsigned long long src_time = info.last_write_time;
            unsigned long long dst_time = p_get_file_time(dst_file);

            if (dst_time > 0) {
                if (src_time <= dst_time) {
                    should_copy = 0; 
                }
            }

            if (should_copy) {
                log_info("Copying: %s", info.name);
                if (!p_copy_file(src_file, dst_file)) {
                    log_error("Failed to copy %s: %lu", info.name, p_get_last_error());
                    all_success = 0;
                }
            }
        }
    } while (p_find_next(hFind, &info));

    p_find_close(hFind);
    return all_success;
}

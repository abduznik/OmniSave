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

void cleanup_temp_files(const char* dst) {
    char search_path[MAX_PATH_LEN];
    PFileInfo info;
    PHandle hFind = NULL;

    snprintf(search_path, MAX_PATH_LEN, "%s\\*", dst);
    hFind = p_find_first(search_path, &info);

    if (hFind != NULL) {
        do {
            if (strstr(info.name, ".omnitmp")) {
                char tmp_path[MAX_PATH_LEN];
                snprintf(tmp_path, MAX_PATH_LEN, "%s\\%s", dst, info.name);
                log_info("Cleaning up stale temp file: %s", info.name);
                p_delete_file(tmp_path);
            }
        } while (p_find_next(hFind, &info));
        p_find_close(hFind);
    }
}

int sync_folders(const char* src, const char* dst, int propagate_deletes) {
    char search_path[MAX_PATH_LEN];
    PFileInfo info;
    PHandle hFind = NULL;

    log_info("Syncing: %s -> %s (Propagate Deletes: %d)", src, dst, propagate_deletes);
    ensure_dir(dst);
    cleanup_temp_files(dst);

    // --- PASS 1: Copy/Update ---
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
            if (!sync_folders(src_file, dst_file, propagate_deletes)) {
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
                char tmp_file[MAX_PATH_LEN];
                snprintf(tmp_file, MAX_PATH_LEN, "%s.omnitmp", dst_file);

                log_info("Copying (Atomic): %s", info.name);
                if (p_copy_file(src_file, tmp_file)) {
                    if (!p_move_file_atomic(tmp_file, dst_file)) {
                        log_error("Failed to commit atomic write for %s: %lu", info.name, p_get_last_error());
                        p_delete_file(tmp_file);
                        all_success = 0;
                    }
                } else {
                    log_error("Failed to copy %s to temp: %lu", info.name, p_get_last_error());
                    p_delete_file(tmp_file);
                    all_success = 0;
                }
            }
        }
    } while (p_find_next(hFind, &info));

    p_find_close(hFind);

    // --- PASS 2: Deletion Propagation ---
    if (propagate_deletes) {
        snprintf(search_path, MAX_PATH_LEN, "%s\\*", dst);
        hFind = p_find_first(search_path, &info);
        if (hFind != NULL) {
            do {
                if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) continue;

                char src_check[MAX_PATH_LEN];
                char dst_check[MAX_PATH_LEN];
                snprintf(src_check, MAX_PATH_LEN, "%s\\%s", src, info.name);
                snprintf(dst_check, MAX_PATH_LEN, "%s\\%s", dst, info.name);

                int exists_in_src = info.is_dir ? p_directory_exists(src_check) : p_file_exists(src_check);

                if (!exists_in_src) {
                    log_info("Deleting (Stale): %s", info.name);
                    if (info.is_dir) {
                        p_delete_directory_recursive(dst_check);
                    } else {
                        p_delete_file(dst_check);
                    }
                }
            } while (p_find_next(hFind, &info));
            p_find_close(hFind);
        }
    }

    return all_success;
}

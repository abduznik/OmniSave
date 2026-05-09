#include "omnisave.h"
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

// Helper to ensure a directory exists
static void create_dir_if_not_exists(const char *path) {
    char tmp[MAX_PATH];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == '/' || tmp[len - 1] == '\\')
        tmp[len - 1] = 0;
    
    for (p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char sep = *p;
            *p = 0;
            MKDIR(tmp);
            *p = sep;
        }
    }
    MKDIR(tmp);
}

// Copy a single file
static bool copy_file(const char *src, const char *dest) {
    FILE *source = fopen(src, "rb");
    if (!source) {
        printf("[OmniSave] Error opening source file '%s'.\n", src);
        return false;
    }

    FILE *destination = fopen(dest, "wb");
    if (!destination) {
        printf("[OmniSave] Error opening destination file '%s'.\n", dest);
        fclose(source);
        return false;
    }

    char buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes_read, destination);
    }

    fclose(source);
    fclose(destination);
    return true;
}

time_t get_latest_mtime(const char *dir_path) {
    DIR *d = opendir(dir_path);
    if (!d) return 0;

    struct dirent *dir;
    time_t latest_time = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }

        char path[MAX_PATH];
        snprintf(path, sizeof(path), "%s/%s", dir_path, dir->d_name);

        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                time_t sub_time = get_latest_mtime(path);
                if (sub_time > latest_time) latest_time = sub_time;
            } else {
                if (st.st_mtime > latest_time) latest_time = st.st_mtime;
            }
        }
    }
    closedir(d);
    return latest_time;
}

bool copy_dir_recursive(const char *src, const char *dest) {
    DIR *d = opendir(src);
    if (!d) {
        printf("[OmniSave] Notice: Source directory '%s' not found. Skipping sync.\n", src);
        return true; // Return true as it's not a fatal error, just a fresh state.
    }

    create_dir_if_not_exists(dest);

    struct dirent *dir;
    bool success = true;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }

        char src_path[MAX_PATH];
        char dest_path[MAX_PATH];
        snprintf(src_path, sizeof(src_path), "%s/%s", src, dir->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, dir->d_name);

        struct stat st;
        if (stat(src_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                if (!copy_dir_recursive(src_path, dest_path)) {
                    success = false;
                }
            } else {
                if (!copy_file(src_path, dest_path)) {
                    success = false;
                }
            }
        }
    }
    closedir(d);
    return success;
}

bool create_backup(const char *dir_path) {
    struct stat st;
    if (stat(dir_path, &st) == -1) {
        // Source doesn't exist, nothing to backup
        return true;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);
    
    // Remove trailing slash if present for cleaner backup name
    char clean_path[MAX_PATH];
    strncpy(clean_path, dir_path, sizeof(clean_path) - 1);
    size_t len = strlen(clean_path);
    if (len > 0 && clean_path[len - 1] == '/') {
        clean_path[len - 1] = '\0';
    }

    char backup_path[MAX_PATH];
    snprintf(backup_path, sizeof(backup_path), "%s_Backup_%s", clean_path, timestamp);

    printf("[OmniSave] Creating backup to: %s\n", backup_path);
    
    // Using copy instead of rename so we have a full mirror backup
    return copy_dir_recursive(dir_path, backup_path);
}

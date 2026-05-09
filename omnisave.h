#ifndef OMNISAVE_H
#define OMNISAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#include <windows.h>
#endif

#include <time.h>
#include <stdbool.h>
#include <errno.h>

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif
#define MAX_CMD 2048

#define MAX_SYNC_PATHS 10

// Sync path pair
typedef struct {
    char host_path[MAX_PATH];
    char portable_path[MAX_PATH];
} SyncPair;

// Configuration structure
typedef struct {
    char target_game_exe[MAX_PATH];
    char launch_command[MAX_CMD];
    SyncPair sync_paths[MAX_SYNC_PATHS];
    int sync_path_count;
} OmniConfig;

// config_parser.c
bool parse_config(const char *filename, OmniConfig *config);

// path_utils.c
void expand_tilde(const char *path, char *expanded_path, size_t max_len);
void resolve_dynamic_path(const char *input_path, char *resolved_path, size_t max_len);

// sync_engine.c
time_t get_latest_mtime(const char *dir_path);
bool copy_dir_recursive(const char *src, const char *dest);
bool create_backup(const char *dir_path);

// process_manager.c
bool launch_and_wait(const char *command);

#endif // OMNISAVE_H

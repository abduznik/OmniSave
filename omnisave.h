#ifndef OMNISAVE_H
#define OMNISAVE_H

#include "platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CONFIG_FILE "omnisave.ini"
#define LOG_FILE "omnisave.log"
#define OMNISAVE_VERSION "0.2.0"

typedef struct {
    char launch_command[MAX_PATH_LEN];
    char launch_args[MAX_PATH_LEN];
    char local_path[MAX_PATH_LEN];
    char remote_path[MAX_PATH_LEN];
} Config;

void log_info(const char* format, ...);
void log_error(const char* format, ...);
int load_config(Config* cfg, const char* ini_path, const char* base_dir);
void expand_path(char* path, const char* base_dir);
int sync_folders(const char* src, const char* dst, int propagate_deletes);

int launch_game(const char* command, const char* args, const char* base_dir); 
int is_process_running(const char* process_name);

int acquire_lock();
void release_lock();

#endif // OMNISAVE_H

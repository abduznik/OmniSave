#ifndef OMNISAVE_H
#define OMNISAVE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <shlobj.h> 

#define MAX_PATH_LEN 1024
#define CONFIG_FILE "omnisave.ini"
#define LOG_FILE "omnisave.log"

typedef struct {
    char launch_command[MAX_PATH_LEN];
    char launch_args[MAX_PATH_LEN];
    char local_path[MAX_PATH_LEN];
    char remote_path[MAX_PATH_LEN];
} Config;

void log_info(const char* format, ...);
void log_error(const char* format, ...);
int load_config(Config* cfg, const char* ini_path, const char* base_dir);
int sync_folders(const char* src, const char* dst);

int launch_game(const char* command, const char* args, const char* base_dir); 

int acquire_lock();
void release_lock();

#endif // OMNISAVE_H

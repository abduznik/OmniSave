#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include <stdarg.h>

#define MAX_PATH_LEN 1024
#define MAX_CMDLINE_LEN 4096

typedef void* PHandle;

typedef struct {
    char name[MAX_PATH_LEN];
    int is_dir;
    unsigned long long last_write_time;
} PFileInfo;

// Filesystem
int p_create_directory(const char* path);
PHandle p_find_first(const char* path, PFileInfo* info);
int p_find_next(PHandle handle, PFileInfo* info);
void p_find_close(PHandle handle);
int p_copy_file(const char* src, const char* dst);
int p_move_file_atomic(const char* src, const char* dst);
int p_delete_file(const char* path);
int p_delete_directory_recursive(const char* path);
int p_file_exists(const char* path);
int p_directory_exists(const char* path);
unsigned long long p_get_file_time(const char* path);

// Process
int p_is_process_running(const char* process_name);
int p_launch_process(const char* command, const char* args, const char* work_dir);
void p_sleep(int ms);

// Lock / Mutex
PHandle p_create_mutex(const char* name);
int p_is_already_running();
void p_release_mutex(PHandle handle);
void p_close_handle(PHandle handle);

// System / Config
int p_get_config_string(const char* section, const char* key, const char* default_val, char* out, int size, const char* file);
int p_get_user_profile_dir(char* out, int size);
void p_get_module_path(char* out, int size);
int p_set_cwd(const char* path);
void p_show_error(const char* title, const char* msg);

// Error handling
unsigned long p_get_last_error();

#endif // PLATFORM_H

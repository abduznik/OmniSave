#include "platform.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MOCK_FILES 100
#define MAX_MOCK_PROCESSES 10

typedef struct {
    char path[MAX_PATH_LEN];
    int is_dir;
    unsigned long long time;
} MockFile;

typedef struct {
    char name[MAX_PATH_LEN];
    int is_running;
} MockProcess;

static MockFile mock_files[MAX_MOCK_FILES];
static int mock_file_count = 0;

static MockProcess mock_processes[MAX_MOCK_PROCESSES];
static int mock_process_count = 0;

static int last_error = 0;
static char mock_profile_dir[MAX_PATH_LEN] = "/Users/mockuser";
static char mock_module_path[MAX_PATH_LEN] = "/mock/OmniSave.exe";
static char last_launched_cmd[MAX_CMDLINE_LEN] = {0};

// --- Mock Control Interface ---

void mock_reset() {
    mock_file_count = 0;
    mock_process_count = 0;
    last_error = 0;
    memset(last_launched_cmd, 0, MAX_CMDLINE_LEN);
}

void mock_fs_add_file(const char* path, int is_dir, unsigned long long time) {
    if (mock_file_count < MAX_MOCK_FILES) {
        strncpy(mock_files[mock_file_count].path, path, MAX_PATH_LEN);
        mock_files[mock_file_count].is_dir = is_dir;
        mock_files[mock_file_count].time = time;
        mock_file_count++;
    }
}

void mock_process_set_running(const char* name, int is_running) {
    for (int i = 0; i < mock_process_count; i++) {
        if (strcmp(mock_processes[i].name, name) == 0) {
            mock_processes[i].is_running = is_running;
            return;
        }
    }
    if (mock_process_count < MAX_MOCK_PROCESSES) {
        strncpy(mock_processes[mock_process_count].name, name, MAX_PATH_LEN);
        mock_processes[mock_process_count].is_running = is_running;
        mock_process_count++;
    }
}

void mock_set_last_error(int err) {
    last_error = err;
}

// --- Platform Implementation ---

int p_create_directory(const char* path) {
    return 1; // Always succeed in mock
}

typedef struct {
    char search_pattern[MAX_PATH_LEN];
    int current_index;
} MockFindHandle;

PHandle p_find_first(const char* path, PFileInfo* info) {
    // Remove '*' from end of path if present
    char base_path[MAX_PATH_LEN];
    strncpy(base_path, path, MAX_PATH_LEN);
    char* star = strrchr(base_path, '*');
    if (star) *star = '\0';
    if (base_path[strlen(base_path)-1] == '\\') base_path[strlen(base_path)-1] = '\0';

    for (int i = 0; i < mock_file_count; i++) {
        // Simple mock matching: check if file is in the "directory"
        // This is a very basic simulation
        if (strstr(mock_files[i].path, base_path) == mock_files[i].path) {
             // Extract just the filename
             const char* filename = mock_files[i].path + strlen(base_path);
             if (*filename == '\\' || *filename == '/') filename++;
             
             // If there are more slashes, it's in a sub-sub dir, skip for now unless we are recursive
             if (strchr(filename, '\\') == NULL && strchr(filename, '/') == NULL) {
                 strncpy(info->name, filename, MAX_PATH_LEN);
                 info->is_dir = mock_files[i].is_dir;
                 info->last_write_time = mock_files[i].time;
                 
                 MockFindHandle* h = malloc(sizeof(MockFindHandle));
                 strncpy(h->search_pattern, base_path, MAX_PATH_LEN);
                 h->current_index = i + 1;
                 return (PHandle)h;
             }
        }
    }
    return NULL;
}

int p_find_next(PHandle handle, PFileInfo* info) {
    MockFindHandle* h = (MockFindHandle*)handle;
    for (int i = h->current_index; i < mock_file_count; i++) {
        if (strstr(mock_files[i].path, h->search_pattern) == mock_files[i].path) {
             const char* filename = mock_files[i].path + strlen(h->search_pattern);
             if (*filename == '\\' || *filename == '/') filename++;
             
             if (strchr(filename, '\\') == NULL && strchr(filename, '/') == NULL) {
                 strncpy(info->name, filename, MAX_PATH_LEN);
                 info->is_dir = mock_files[i].is_dir;
                 info->last_write_time = mock_files[i].time;
                 h->current_index = i + 1;
                 return 1;
             }
        }
    }
    return 0;
}

void p_find_close(PHandle handle) {
    if (handle) free(handle);
}

int p_copy_file(const char* src, const char* dst) {
    if (last_error != 0) return 0; // Simulate failure
    // In mock, we don't actually move bits, but we can add the dst to mock_fs
    // However, the test expects p_copy_file to be followed by p_move_file_atomic
    // So let's just simulate the "temp file" creation if it ends in .omnitmp
    if (strstr(dst, ".omnitmp")) {
        mock_fs_add_file(dst, 0, 1000); // Dummy time
    }
    return 1;
}

int p_move_file_atomic(const char* src, const char* dst) {
    if (last_error != 0) return 0;
    // Remove src, add/update dst
    p_delete_file(src);
    mock_fs_add_file(dst, 0, 1000);
    return 1;
}

int p_delete_file(const char* path) {
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].path, path) == 0) {
            // Shift remaining
            for (int j = i; j < mock_file_count - 1; j++) {
                mock_files[j] = mock_files[j+1];
            }
            mock_file_count--;
            return 1;
        }
    }
    return 0;
}

int p_delete_directory_recursive(const char* path) {
    int deleted_count = 0;
    for (int i = 0; i < mock_file_count; ) {
        if (strstr(mock_files[i].path, path) == mock_files[i].path) {
            // Delete this entry
            for (int j = i; j < mock_file_count - 1; j++) {
                mock_files[j] = mock_files[j+1];
            }
            mock_file_count--;
            deleted_count++;
            // Don't increment i
        } else {
            i++;
        }
    }
    return deleted_count > 0;
}

int p_file_exists(const char* path) {
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].path, path) == 0 && !mock_files[i].is_dir) return 1;
    }
    return 0;
}

int p_directory_exists(const char* path) {
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].path, path) == 0 && mock_files[i].is_dir) return 1;
    }
    return 0;
}

unsigned long long p_get_file_time(const char* path) {
    for (int i = 0; i < mock_file_count; i++) {
        if (strcmp(mock_files[i].path, path) == 0) return mock_files[i].time;
    }
    return 0;
}

int p_is_process_running(const char* process_name) {
    for (int i = 0; i < mock_process_count; i++) {
        if (strcmp(mock_processes[i].name, process_name) == 0) return mock_processes[i].is_running;
    }
    return 0;
}

int p_launch_process(const char* command, const char* args, const char* work_dir) {
    if (args && strlen(args) > 0) {
        snprintf(last_launched_cmd, MAX_CMDLINE_LEN, "\"%s\" %s", command, args);
    } else {
        snprintf(last_launched_cmd, MAX_CMDLINE_LEN, "\"%s\"", command);
    }
    return 1;
}

void p_get_last_launched_command(char* out) {
    strncpy(out, last_launched_cmd, MAX_CMDLINE_LEN);
}

void p_sleep(int ms) {
    // No-op in mock
}

PHandle p_create_mutex(const char* name) {
    if (last_error == 183) return (PHandle)1; // ERROR_ALREADY_EXISTS
    return (PHandle)1;
}

int p_is_already_running() {
    return last_error == 183;
}

void p_release_mutex(PHandle handle) {}
void p_close_handle(PHandle handle) {}

int p_get_config_string(const char* section, const char* key, const char* default_val, char* out, int size, const char* file) {
    // Basic mock for ini reading
    if (strcmp(key, "Local_Path") == 0) strncpy(out, "./local", size);
    else if (strcmp(key, "Remote_Path") == 0) strncpy(out, "./remote", size);
    else strncpy(out, default_val, size);
    return (int)strlen(out);
}

int p_get_user_profile_dir(char* out, int size) {
    strncpy(out, mock_profile_dir, size);
    return 1;
}

void p_get_module_path(char* out, int size) {
    strncpy(out, mock_module_path, size);
}

int p_set_cwd(const char* path) {
    return 1;
}

void p_show_error(const char* title, const char* msg) {
    printf("MOCK ERROR: [%s] %s\n", title, msg);
}

unsigned long p_get_last_error() {
    return (unsigned long)last_error;
}

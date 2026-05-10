#include "platform.h"
#include <windows.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <stdio.h>

int p_create_directory(const char* path) {
    return CreateDirectoryA(path, NULL) != 0;
}

PHandle p_find_first(const char* path, PFileInfo* info) {
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(path, &fd);
    if (h == INVALID_HANDLE_VALUE) return NULL;

    strncpy(info->name, fd.cFileName, MAX_PATH_LEN);
    info->is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    info->last_write_time = ((unsigned long long)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime;
    
    return (PHandle)h;
}

int p_find_next(PHandle handle, PFileInfo* info) {
    WIN32_FIND_DATAA fd;
    if (FindNextFileA((HANDLE)handle, &fd)) {
        strncpy(info->name, fd.cFileName, MAX_PATH_LEN);
        info->is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        info->last_write_time = ((unsigned long long)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime;
        return 1;
    }
    return 0;
}

void p_find_close(PHandle handle) {
    FindClose((HANDLE)handle);
}

int p_copy_file(const char* src, const char* dst) {
    return CopyFileA(src, dst, FALSE) != 0;
}

int p_move_file_atomic(const char* src, const char* dst) {
    return MoveFileExA(src, dst, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}

int p_delete_file(const char* path) {
    return DeleteFileA(path) != 0;
}

int p_delete_directory_recursive(const char* path) {
    char search_path[MAX_PATH_LEN];
    snprintf(search_path, MAX_PATH_LEN, "%s\\*", path);

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search_path, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
                char sub_path[MAX_PATH_LEN];
                snprintf(sub_path, MAX_PATH_LEN, "%s\\%s", path, fd.cFileName);
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    p_delete_directory_recursive(sub_path);
                } else {
                    DeleteFileA(sub_path);
                }
            }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
    return RemoveDirectoryA(path) != 0;
}

int p_file_exists(const char* path) {
    DWORD dwAttrib = GetFileAttributesA(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int p_directory_exists(const char* path) {
    DWORD dwAttrib = GetFileAttributesA(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

unsigned long long p_get_file_time(const char* path) {
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return 0;
    
    FILETIME ft;
    unsigned long long result = 0;
    if (GetFileTime(h, NULL, NULL, &ft)) {
        result = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    }
    CloseHandle(h);
    return result;
}

int p_is_process_running(const char* process_name) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    int exists = 0;

    if (Process32First(hSnap, &pe)) {
        do {
            if (_stricmp(pe.szExeFile, process_name) == 0) {
                exists = 1;
                break;
            }
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    return exists;
}

int p_launch_process(const char* command, const char* args, const char* work_dir) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmd_line[MAX_PATH_LEN * 2];
    if (args && strlen(args) > 0) {
        snprintf(cmd_line, sizeof(cmd_line), "\"%s\" %s", command, args);
    } else {
        snprintf(cmd_line, sizeof(cmd_line), "\"%s\"", command);
    }

    if (CreateProcessA(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, work_dir, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    return 0;
}

void p_sleep(int ms) {
    Sleep(ms);
}

PHandle p_create_mutex(const char* name) {
    return (PHandle)CreateMutexA(NULL, TRUE, name);
}

int p_is_already_running() {
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

void p_release_mutex(PHandle handle) {
    if (handle) ReleaseMutex((HANDLE)handle);
}

void p_close_handle(PHandle handle) {
    if (handle) CloseHandle((HANDLE)handle);
}

int p_get_config_string(const char* section, const char* key, const char* default_val, char* out, int size, const char* file) {
    return GetPrivateProfileStringA(section, key, default_val, out, size, file);
}

int p_get_user_profile_dir(char* out, int size) {
    return SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, out));
}

void p_get_module_path(char* out, int size) {
    GetModuleFileNameA(NULL, out, size);
}

int p_set_cwd(const char* path) {
    return SetCurrentDirectoryA(path);
}

void p_show_error(const char* title, const char* msg) {
    MessageBoxA(NULL, msg, title, MB_ICONERROR | MB_OK);
}

unsigned long p_get_last_error() {
    return GetLastError();
}

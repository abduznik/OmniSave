#include "omnisave.h"
#include <tlhelp32.h>

// Helper to check if a specific process name is currently running
int is_process_running(const char* process_name) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    int exists = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, process_name) == 0) {
                exists = 1;
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return exists;
}

// <-- SIGNATURE UPDATED
int launch_game(const char* command, const char* args, const char* base_dir) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Construct full path to executable
    char full_command[MAX_PATH_LEN];
    snprintf(full_command, MAX_PATH_LEN, "%s\\%s", base_dir, command);

    // <-- NEW: Format the command line string with quotes and arguments
    char cmd_line[MAX_PATH_LEN * 2];
    if (strlen(args) > 0) {
        snprintf(cmd_line, sizeof(cmd_line), "\"%s\" %s", full_command, args);
    } else {
        snprintf(cmd_line, sizeof(cmd_line), "\"%s\"", full_command);
    }

    log_info("Executing Target: %s", full_command);
    log_info("Arguments: %s", args);
    log_info("Working Directory: %s", base_dir);

    // Pass cmd_line instead of cmd_copy
    if (CreateProcessA(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, base_dir, &si, &pi)) {
        log_info("Process created. Waiting for %s memory presence...", command);
        
        // Give the executable 10 seconds to fully unpack
        Sleep(10000); 

        if (is_process_running(command)) {
            log_info("%s detected in memory. Holding session alive...", command);
            
            while (is_process_running(command)) {
                Sleep(3000); 
            }
            log_info("%s has terminated.", command);
        } else {
            log_error("%s not found after 10 seconds. Did it crash?", command);
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    } else {
        log_error("Failed to execute launcher: %lu", GetLastError());
    }

    return 0;
}

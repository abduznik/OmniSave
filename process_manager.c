#include "omnisave.h"

bool launch_and_wait(const char *command) {
    if (!command || strlen(command) == 0) {
        printf("[OmniSave] Error: No launch command provided.\n");
        return false;
    }

    printf("[OmniSave] Launching game process...\n");
    printf("[OmniSave] Command: %s\n", command);

#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcess needs a mutable string
    char *cmd = strdup(command);

    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("[OmniSave] Error: CreateProcess failed (%ld).\n", GetLastError());
        free(cmd);
        return false;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    printf("[OmniSave] Game process completed with exit code %ld.\n", exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    free(cmd);
#else
    int status = system(command);
    if (status == -1) {
        printf("[OmniSave] Error: Failed to execute system command.\n");
        return false;
    }
    printf("[OmniSave] Game process completed with status %d.\n", status);
#endif

    return true;
}

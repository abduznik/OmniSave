#include "omnisave.h"

static HANDLE hMutex = NULL;

int acquire_lock() {
    // Create a named mutex. The second parameter TRUE means we request initial ownership.
    hMutex = CreateMutexA(NULL, TRUE, "OmniSaveLock");

    if (hMutex == NULL) {
        log_error("Could not create mutex: %lu", GetLastError());
        return 0;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        log_error("OmniSave is already running (Mutex exists).");
        CloseHandle(hMutex);
        hMutex = NULL;
        return 0;
    }

    log_info("Mutex lock acquired.");
    return 1;
}

void release_lock() {
    if (hMutex != NULL) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        hMutex = NULL;
        log_info("Mutex lock released.");
    }
}

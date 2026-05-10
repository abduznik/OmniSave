#include "omnisave.h"

static PHandle hMutex = NULL;

int acquire_lock() {
    hMutex = p_create_mutex("OmniSaveLock");

    if (hMutex == NULL) {
        log_error("Could not create mutex: %lu", p_get_last_error());
        return 0;
    }

    if (p_is_already_running()) {
        log_error("OmniSave is already running (Mutex exists).");
        p_close_handle(hMutex);
        hMutex = NULL;
        return 0;
    }

    log_info("Mutex lock acquired.");
    return 1;
}

void release_lock() {
    if (hMutex != NULL) {
        p_release_mutex(hMutex);
        p_close_handle(hMutex);
        hMutex = NULL;
        log_info("Mutex lock released.");
    }
}

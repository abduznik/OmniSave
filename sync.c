#include "omnisave.h"
#include <shlwapi.h>

void ensure_dir(const char* path) {
    char temp[MAX_PATH_LEN];
    char* p = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if (temp[len - 1] == '\\') temp[len - 1] = 0;
    for (p = temp + 1; *p; p++) {
        if (*p == '\\') {
            *p = 0;
            CreateDirectoryA(temp, NULL);
            *p = '\\';
        }
    }
    CreateDirectoryA(temp, NULL);
}

int sync_folders(const char* src, const char* dst) {
    char search_path[MAX_PATH_LEN];
    WIN32_FIND_DATAA fd;
    HANDLE hFind = NULL;

    log_info("Syncing: %s -> %s", src, dst);
    ensure_dir(dst);

    snprintf(search_path, MAX_PATH_LEN, "%s\\*", src);
    hFind = FindFirstFileA(search_path, &fd);

    if (hFind == INVALID_HANDLE_VALUE) {
        log_error("Could not open source directory: %s", src);
        return 0;
    }

    do {
        // Skip current dir, parent dir, AND macOS hidden AppleDouble files
        if (strcmp(fd.cFileName, ".") == 0 || 
            strcmp(fd.cFileName, "..") == 0 ||
            strncmp(fd.cFileName, "._", 2) == 0) { 
            continue;
        }

        char src_file[MAX_PATH_LEN];
        char dst_file[MAX_PATH_LEN];
        snprintf(src_file, MAX_PATH_LEN, "%s\\%s", src, fd.cFileName);
        snprintf(dst_file, MAX_PATH_LEN, "%s\\%s", dst, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            sync_folders(src_file, dst_file);
        } else {
            HANDLE hDst = CreateFileA(dst_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            int should_copy = 1;

            if (hDst != INVALID_HANDLE_VALUE) {
                FILETIME ftSrc = fd.ftLastWriteTime;
                FILETIME ftDst;
                if (GetFileTime(hDst, NULL, NULL, &ftDst)) {
                    if (CompareFileTime(&ftSrc, &ftDst) <= 0) {
                        should_copy = 0; 
                    }
                }
                CloseHandle(hDst);
            }

            if (should_copy) {
                log_info("Copying: %s", fd.cFileName);
                if (!CopyFileA(src_file, dst_file, FALSE)) {
                    log_error("Failed to copy %s: %lu", fd.cFileName, GetLastError());
                }
            }
        }
    } while (FindNextFileA(hFind, &fd));

    FindClose(hFind);
    return 1;
}

#include "omnisave.h"

#ifndef _WIN32
#include <pwd.h>
#endif

#include <dirent.h>
#include <sys/stat.h>

void expand_tilde(const char *path, char *expanded_path, size_t max_len) {
    if (path == NULL || expanded_path == NULL || max_len == 0) return;

    if (path[0] == '~') {
        const char *home_dir = getenv("HOME");
        
#ifdef _WIN32
        if (home_dir == NULL) {
            home_dir = getenv("USERPROFILE");
        }
#else
        if (home_dir == NULL) {
            struct passwd *pw = getpwuid(getuid());
            if (pw != NULL) {
                home_dir = pw->pw_dir;
            }
        }
#endif

        if (home_dir != NULL) {
            snprintf(expanded_path, max_len, "%s%s", home_dir, path + 1);
        } else {
            strncpy(expanded_path, path, max_len - 1);
            expanded_path[max_len - 1] = '\0';
        }
    } else {
        strncpy(expanded_path, path, max_len - 1);
        expanded_path[max_len - 1] = '\0';
    }
}

void resolve_dynamic_path(const char *input_path, char *resolved_path, size_t max_len) {
    char expanded_tilde[MAX_PATH];
    expand_tilde(input_path, expanded_tilde, sizeof(expanded_tilde));

    char *asterisk = strchr(expanded_tilde, '*');
    if (!asterisk) {
        strncpy(resolved_path, expanded_tilde, max_len - 1);
        resolved_path[max_len - 1] = '\0';
        return;
    }

    // Split path into prefix (before *) and suffix (after *)
    *asterisk = '\0';
    const char *prefix = expanded_tilde;
    const char *suffix = asterisk + 1;

    DIR *d = opendir(prefix);
    if (!d) {
        snprintf(resolved_path, max_len, "%s*%s", prefix, suffix);
        return;
    }

    struct dirent *dir;
    bool found = false;
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;

        char test_path[MAX_PATH];
        snprintf(test_path, sizeof(test_path), "%s%s%s", prefix, dir->d_name, suffix);

        struct stat st;
        if (stat(test_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            strncpy(resolved_path, test_path, max_len - 1);
            resolved_path[max_len - 1] = '\0';
            found = true;
            break;
        }
    }
    closedir(d);

    if (!found) {
        snprintf(resolved_path, max_len, "%s*%s", prefix, suffix);
    }
}

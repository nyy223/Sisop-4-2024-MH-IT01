#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

#define LOG_FILE "/home/nayla/soal2/logs-fuse.log"

static const char *base_path = "/home/nayla/soal2/sensitif";

const char *secret_password = "sisop";

void log_message(const char *status, const char *tag, const char *info) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char timestamp[20];
        strftime(timestamp, 20, "%d/%m/%Y-%H:%M:%S", tm_info);
        fprintf(log, "[%s]::%s::[%s]::[%s]\n", status, timestamp, tag, info);
        fclose(log);
    }
}

void decode_base64(const char *input, char **output) {
    size_t len = strlen(input);
    *output = malloc(len);
    size_t out_len = 0;
    int val = 0, valb = -8;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = input[i];
        if (c >= 'A' && c <= 'Z') c = c - 'A';
        else if (c >= 'a' && c <= 'z') c = c - 'a' + 26;
        else if (c >= '0' && c <= '9') c = c - '0' + 52;
        else if (c == '+') c = 62;
        else if (c == '/') c = 63;
        else if (c == '=') break;
        else continue;
        val = (val << 6) + c;
        valb += 6;
        if (valb >= 0) {
            (*output)[out_len++] = (char)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    (*output)[out_len] = '\0';
}

void decode_rot13(const char *input, char **output) {
    size_t len = strlen(input);
    *output = malloc(len + 1);
    for (int i = 0; i < len; i++) {
        char c = input[i];
        if ('a' <= c && c <= 'z') {
            (*output)[i] = 'a' + ((c - 'a' + 13) % 26);
        } else if ('A' <= c && c <= 'Z') {
            (*output)[i] = 'A' + ((c - 'A' + 13) % 26);
        } else {
            (*output)[i] = c;
        }
    }
    (*output)[len] = '\0';
}

void decode_hex(const char *input, char **output) {
    size_t len = strlen(input) / 2;
    *output = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        sscanf(input + 2 * i, "%2hhx", *output + i);
    }
    (*output)[len] = '\0';
}

void reverse_text(const char *input, char **output) {
    size_t len = strlen(input);
    *output = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        (*output)[i] = input[len - i - 1];
    }
    (*output)[len] = '\0';
}

bool contains_keyword(const char *path, const char *keyword) {
    return strstr(path, keyword) != NULL;
}

bool is_secret_folder(const char *path) {
    return contains_keyword(path, "rahasia");
}

bool handle_secret_folder_access() {
    char user_input[100];
    printf("Enter the password to access the secret folder: ");
    scanf("%s", user_input);
    if (strcmp(user_input, secret_password) != 0) {
        printf("Incorrect password. Access denied.\n");
        return false;
    }
    return true;
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);

    if (is_secret_folder(path) && !handle_secret_folder_access(path)) {
            return -EACCES;
        }

    res = lstat(fpath, stbuf);
    if (res == -1) {
        return -errno;
    }
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);
    DIR *dp;
    struct dirent *de;
    dp = opendir(fpath);
    if (dp == NULL) {
        log_message("FAILED", "opendir", strerror(errno));
        return -errno;
    }

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0)) {
            log_message("FAILED", "filler", "Buffer full or error");
            break;
        }
    }
    closedir(dp);
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);

    if (is_secret_folder(path) && !handle_secret_folder_access(path)) {
            return -EACCES;
    }

    int res = open(fpath, fi->flags);
    if (res == -1) {
        return -errno;
    }
    close(res);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);

    if (is_secret_folder(path) && !handle_secret_folder_access(path)) {
            return -EACCES;
    }

    int fd = open(fpath, O_RDONLY);
    if (fd == -1) {
        return -errno;
    }
    res = pread(fd, buf, size, offset);
    if (res == -1) {
        res = -errno;
    }
    close(fd);

    if (strncmp(path, "/pesan/", 7) == 0) {
        char *decoded = NULL;
        if (strstr(path, "base64") != NULL) {
            decode_base64(buf, &decoded);
        } else if (strstr(path, "rot13") != NULL) {
            decode_rot13(buf, &decoded);
        } else if (strstr(path, "hex") != NULL) {
            decode_hex(buf, &decoded);
        } else if (strstr(path, "rev") != NULL) {
            reverse_text(buf, &decoded);
        }

        if (decoded) {
            strncpy(buf, decoded, size);
            buf[size - 1] = '\0';
            free(decoded);
        }
    }

    log_message("SUCCESS", "readFile", path);
    return res;
}


static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .open = xmp_open,
    .read = xmp_read,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

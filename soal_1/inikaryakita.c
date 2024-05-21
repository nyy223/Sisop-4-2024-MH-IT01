#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char *dirpath = "/home/kokon/PraktikumModul4/Soal1/portofolio";

void add_watermark_to_image(const char *image_path) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "convert %s -gravity South -fill white -pointsize 50 -annotate +0+30 'inikaryakita.id' %s", image_path, image_path);
    system(cmd);
}

void reverse_file_content(const char *file_path) {
    FILE *file = fopen(file_path, "r+");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(file_size + 1);
    if (content == NULL) {
        perror("malloc");
        fclose(file);
        return;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';

    for (long i = 0, j = file_size - 1; i < j; i++, j--) {
        char temp = content[i];
        content[i] = content[j];
        content[j] = temp;
    }

    fseek(file, 0, SEEK_SET);
    fwrite(content, 1, file_size, file);
    fclose(file);
    free(content);
}

static int get_attributes(const char *path, struct stat *stbuf) {
    int res;
    char full_path[1024];

    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("get_attributes: %s\n", full_path); // Debug statement

    res = lstat(full_path, stbuf);
    if (res == -1) return -errno;
    return 0;
}

static int read_directory(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("read_directory: %s\n", full_path); // Debug statement

    DIR *dp;
    struct dirent *de;

    dp = opendir(full_path);
    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0)) break;
    }

    closedir(dp);
    return 0;
}

static int open_file(const char *path, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("open_file: %s\n", full_path); // Debug statement

    int res = open(full_path, fi->flags);
    if (res == -1) return -errno;

    close(res);
    return 0;
}

static int create_file(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("create_file: %s\n", full_path); // Debug statement

    int res = open(full_path, fi->flags, mode);
    if (res == -1) return -errno;

    close(res);
    return 0;
}

static int mknod_file(const char *path, mode_t mode, dev_t rdev) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("mknod_file: %s\n", full_path); // Debug statement

    int res;
    if (S_ISREG(mode)) {
        res = open(full_path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode)) {
        res = mkfifo(full_path, mode);
    } else {
        res = mknod(full_path, mode, rdev);
    }
    if (res == -1)
        return -errno;
    return 0;
}

static int read_file(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("read_file: %s\n", full_path); // Debug statement

    fd = open(full_path, O_RDONLY);
    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;

    close(fd);

    const char *filename = strrchr(path, '/');
    if (filename == NULL) {
        filename = path;
    } else {
        filename++;
    }

    if (strncmp(filename, "test", 4) == 0) {
        reverse_file_content(full_path);
        FILE *file = fopen(full_path, "r");
        if (file != NULL) {
            fread(buf, 1, size, file);
            fclose(file);
        }
    }

    return res;
}

static int rename_file(const char *from, const char *to) {
    char full_path_from[1024];
    char full_path_to[1024];
    snprintf(full_path_from, sizeof(full_path_from), "%s%s", dirpath, from);
    snprintf(full_path_to, sizeof(full_path_to), "%s%s", dirpath, to);

    printf("rename_file: from %s to %s\n", full_path_from, full_path_to); // Debug statement

    int res = rename(full_path_from, full_path_to);
    if (res == -1) return -errno;

    if (strstr(to, "/wm-foto/") != NULL) {
        add_watermark_to_image(full_path_to);
    }

    return 0;
}

static int unlink_file(const char *path) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dirpath, path);

    printf("unlink_file: %s\n", full_path); // Debug statement

    int res = unlink(full_path);
    if (res == -1) return -errno;
    return 0;
}

static void *init_fs(struct fuse_conn_info *conn) {
    DIR *dp;
    struct dirent *de;

    dp = opendir(dirpath);
    if (dp == NULL) return NULL;

    while ((de = readdir(dp)) != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, de->d_name);
        if (de->d_type == DT_REG && strstr(full_path, "/bahaya/") != NULL) {
            reverse_file_content(full_path);
        }
    }

    closedir(dp);
    return NULL;
}

static struct fuse_operations custom_oper = {
    .getattr = get_attributes,
    .readdir = read_directory,
    .open = open_file,
    .create = create_file,
    .mknod = mknod_file,
    .read = read_file,
    .rename = rename_file,
    .unlink = unlink_file,
    .init = init_fs,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &custom_oper, NULL);
}

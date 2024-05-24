#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

static const char *root_dir = "/home/kokon/PraktikumModul4/Soal1/portofolio";

static int custom_getattr(const char *path, struct stat *stbuf) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int result = lstat(full_path, stbuf);
    if (result == -1)
        return -errno;
    return 0;
}

static int custom_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    DIR *dir = opendir(full_path);
    if (dir == NULL)
        return -errno;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = entry->d_ino;
        st.st_mode = entry->d_type << 12;
        if (filler(buf, entry->d_name, &st, 0))
            break;
    }
    closedir(dir);
    return 0;
}

static int custom_mkdir(const char *path, mode_t mode) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int result = mkdir(full_path, mode);
    if (result == -1)
        return -errno;
    return 0;
}

static int custom_rmdir(const char *path) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int result = rmdir(full_path);
    if (result == -1)
        return -errno;
    return 0;
}

static int custom_rename(const char *from, const char *to) {
    char from_path[1000];
    char to_path[1000];
    snprintf(from_path, sizeof(from_path), "%s%s", root_dir, from);
    snprintf(to_path, sizeof(to_path), "%s%s", root_dir, to);

    if (strstr(to, "/wm") != NULL) {
        char command[4096];
        snprintf(command, sizeof(command), "convert -gravity south -font Arial '%s' -fill white -pointsize 50 -annotate +0+0 '%s' '%s'", from_path, "inikaryakita.id", to_path);
        system(command);

        if (unlink(from_path) == -1)
            return -errno;
    } else {
        if (rename(from_path, to_path) == -1)
            return -errno;
    }
    return 0;
}

static int custom_chmod(const char *path, mode_t mode) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    if (chmod(full_path, mode) == -1)
        return -errno;
    return 0;
}

static int custom_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);

    const char *filename = strrchr(path, '/');
    if (filename == NULL) {
        filename = path;
    } else {
        filename++;
    }

    if (strncmp(filename, "test", 4) == 0) {
        FILE *file = fopen(full_path, "r");
        if (file == NULL)
            return -errno;

        size_t len = fread(buf, 1, size, file);
        if (len == -1) {
            fclose(file);
            return -errno;
        }
        buf[len] = '\0';

        for (size_t i = 0; i < len / 2; i++) {
            char temp = buf[i];
            buf[i] = buf[len - i - 1];
            buf[len - i - 1] = temp;
        }

        fclose(file);
        return len;
    } else {
        int fd = open(full_path, O_RDONLY);
        if (fd == -1)
            return -errno;
        int res = pread(fd, buf, size, offset);
        if (res == -1)
            res = -errno;
        close(fd);
        return res;
    }
}

static int custom_unlink(const char *path) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int result = unlink(full_path);
    if (result == -1)
        return -errno;
    return 0;
}

static int custom_open(const char *path, struct fuse_file_info *fi) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int result = open(full_path, fi->flags);
    if (result == -1)
        return -errno;
    close(result);
    return 0;
}

static int custom_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int result = creat(full_path, mode);
    if (result == -1)
        return -errno;
    fi->fh = result;
    return 0;
}

static int custom_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char full_path[1000];
    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
    int fd = open(full_path, O_WRONLY);
    if (fd == -1)
        return -errno;
    int res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    close(fd);
    return res;
}

static struct fuse_operations custom_oper = {
    .getattr    = custom_getattr,
    .readdir    = custom_readdir,
    .mkdir      = custom_mkdir,
    .rmdir      = custom_rmdir,
    .rename     = custom_rename,
    .chmod      = custom_chmod,
    .read       = custom_read,
    .unlink     = custom_unlink,
    .open       = custom_open,
    .create     = custom_create,
    .write      = custom_write,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &custom_oper, NULL);
}

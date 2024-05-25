#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAX_PARTS 100
#define BUFFER_SIZE 10240
#define PART_SIZE 10240

static const char *relics_path = "relics";
static const char *artefak_path = "artefak";

void exec_command(const char *command) {
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else {
        if (waitpid(pid, &status, 0) != pid) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
    }
}

void combine_relics() {
    DIR *dp;
    struct dirent *de;

    dp = opendir(relics_path);
    if (dp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((de = readdir(dp)) != NULL) {
        if (de->d_type == DT_REG && strstr(de->d_name, ".000")) {
            char base_name[256];
            strncpy(base_name, de->d_name, strstr(de->d_name, ".000") - de->d_name);
            base_name[strstr(de->d_name, ".000") - de->d_name] = '\0';

            char combined_file[512];
            snprintf(combined_file, sizeof(combined_file), "%s/%s.png", artefak_path, base_name);

            int output_fd = open(combined_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd == -1) {
                perror("open output file");
                closedir(dp);
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < MAX_PARTS; i++) {
                char part_file[512];
                snprintf(part_file, sizeof(part_file), "%s/%s.%03d", relics_path, base_name, i);

                int input_fd = open(part_file, O_RDONLY);
                if (input_fd == -1) {
                    if (errno != ENOENT) {
                        perror("open input file");
                        close(output_fd);
                        closedir(dp);
                        exit(EXIT_FAILURE);
                    }
                    break;
                }

                char buffer[BUFFER_SIZE];
                ssize_t bytes;
                while ((bytes = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
                    if (write(output_fd, buffer, bytes) != bytes) {
                        perror("write");
                        close(input_fd);
                        close(output_fd);
                        closedir(dp);
                        exit(EXIT_FAILURE);
                    }
                }

                if (bytes == -1) {
                    perror("read");
                    close(input_fd);
                    close(output_fd);
                    closedir(dp);
                    exit(EXIT_FAILURE);
                }

                close(input_fd);
            }

            close(output_fd);
            printf("Combined %s into %s\n", base_name, combined_file);
        }
    }

    closedir(dp);
}

static int custom_getattr(const char *path, struct stat *stbuf) {
    printf("getattr: %s\n", path); // Logging
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s%s", artefak_path, path);
        if (stat(full_path, stbuf) == -1) {
            perror("stat");
            return -errno;
        }
    }
    return 0;
}

static int custom_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    printf("readdir: %s\n", path); // Logging
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    DIR *dp;
    struct dirent *de;

    dp = opendir(artefak_path);
    if (dp == NULL) {
        perror("opendir");
        return -errno;
    }

    while ((de = readdir(dp)) != NULL) {
        printf("readdir: found %s\n", de->d_name); // Logging
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
            if (filler(buf, de->d_name, NULL, 0) != 0) {
                closedir(dp);
                return -ENOMEM;
            }
        }
    }

    closedir(dp);
    return 0;
}

static int custom_open(const char *path, struct fuse_file_info *fi) {
    printf("open: %s\n", path); // Logging
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", artefak_path, path);

    int fd = open(full_path, fi->flags);
    if (fd == -1) {
        perror("open");
        return -errno;
    }

    close(fd);
    return 0;
}

static int custom_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("read: %s\n", path); // Logging
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", artefak_path, path);

    int fd = open(full_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -errno;
    }

    int res = pread(fd, buf, size, offset);
    if (res == -1) {
        perror("pread");
        res = -errno;
    }

    close(fd);
    return res;
}

static int custom_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("write: %s\n", path); // Logging
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", artefak_path, path);

    int fd = open(full_path, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return -errno;
    }

    int res = pwrite(fd, buf, size, offset);
    if (res == -1) {
        perror("pwrite");
        res = -errno;
    }

    close(fd);
    return res;
}

static int custom_unlink(const char *path) {
    printf("unlink: %s\n", path); // Logging
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", artefak_path, path);
    int res = unlink(full_path);
    if (res == -1) {
        perror("unlink");
        return -errno;
    }
    return 0;
}

static struct fuse_operations operations = {
    .getattr = custom_getattr,
    .readdir = custom_readdir,
    .open = custom_open,
    .read = custom_read,
    .write = custom_write,
    .unlink = custom_unlink,
};

int main(int argc, char *argv[]) {
    printf("Starting main function\n"); // Logging
    if (mkdir(artefak_path, 0755) != 0 && errno != EEXIST) {
        perror("mkdir artefak");
        exit(EXIT_FAILURE);
    }

    printf("Downloading discoveries.zip\n"); // Logging
    exec_command("wget -O discoveries.zip \"https://drive.google.com/uc?id=1BJkaBvGaxqiwPWvXRdYNXzxxmIYQ8FKf\"");
    printf("Extracting discoveries.zip\n"); // Logging
    exec_command("unzip -o discoveries.zip");

    printf("Combining relics\n"); // Logging
    combine_relics();

    // Jalankan FUSE dalam foreground untuk debugging yang lebih mudah
    char *fuse_argv[] = { argv[0], "-f", "-o", "nonempty", argv[1], NULL };
    int fuse_argc = 5;

    printf("Starting FUSE main loop\n"); // Logging
    return fuse_main(fuse_argc, fuse_argv, &operations, NULL);
}

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

int main() {

    if (mkdir(artefak_path, 0755) != 0 && errno != EEXIST) {
        perror("mkdir artefak");
        exit(EXIT_FAILURE);
    }

    exec_command("wget -O discoveries.zip \"https://drive.google.com/uc?id=1BJkaBvGaxqiwPWvXRdYNXzxxmIYQ8FKf\"");
    exec_command("unzip -o discoveries.zip");

    combine_relics();

    return 0;
}

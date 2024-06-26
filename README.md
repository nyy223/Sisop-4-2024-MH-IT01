# Laporan Resmi Praktikum Sisop-3-2024-MH-IT01

## Anggota
### Nayla Raissa Azzahra (5027231054)
### Ryan Adya Purwanto (5027231046)
### Rafael Gunawan (5027231019)

## Ketentuan
### Struktur repository seperti berikut : 
      —soal_1:
      	— inikaryakita.c
                                          
      —soal_2:
      	— pastibisa.c
				
      —soal_3:
	        — archeology.c
## Daftar Isi
- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)

## Soal 1
> Rafael (5027231019)
### Soal
#### Adfi merupakan seorang CEO agency creative bernama Ini Karya Kita. Ia sedang melakukan inovasi pada manajemen project photography Ini Karya Kita. Salah satu ide yang dia kembangkan adalah tentang pengelolaan foto project dalam sistem arsip Ini Karya Kita. Dalam membangun sistem ini, Adfi tidak bisa melakukannya sendirian, dia perlu bantuan mahasiswa Departemen Teknologi Informasi angkatan 2023 untuk membahas konsep baru yang akan mengubah project fotografinya lebih menarik untuk dilihat. Adfi telah menyiapkan portofolio hasil project fotonya yang bisa didownload dan diakses di www.inikaryakita.id . Silahkan eksplorasi web Ini Karya Kita dan temukan halaman untuk bisa mendownload projectnya. Setelah kalian download terdapat folder gallery dan bahaya.
#### - Pada folder “gallery”:
####   Membuat folder dengan prefix "wm." Dalam folder ini, setiap gambar yang dipindahkan ke dalamnya akan diberikan watermark bertuliskan inikaryakita.id. 
####   Ex: "mv ikk.jpeg wm-foto/" 
####   Output: 
####   Before: (tidak ada watermark bertuliskan inikaryakita.id)
![Screenshot 2024-05-18 124945](https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/151918510/da514ad6-8dc2-4e08-9b9a-4a8af2ea4df2)
#### After: (terdapat watermark tulisan inikaryakita.id)
![Screenshot 2024-05-18 125004](https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/151918510/93027e0e-3d76-422d-aa87-cdbcd356998d)
#### - Pada folder "bahaya," terdapat file bernama "script.sh." Adfi menyadari pentingnya menjaga keamanan dan integritas data dalam folder ini. 
####   -- Mereka harus mengubah permission pada file "script.sh" agar bisa dijalankan, karena jika dijalankan maka dapat menghapus semua dan isi dari  "gallery"
####   -- Adfi dan timnya juga ingin menambahkan fitur baru dengan membuat file dengan prefix "test" yang ketika disimpan akan mengalami pembalikan (reverse) isi dari file tersebut.  
#### Catatan: Jika ada pertanyaan, bisa menghubungi Adfi via WA aja, nanti dikasih hint

### Penyelesaian
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

### Penjelasan
#### Header dan Path
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
##### Bagian ini mendefinisikan versi FUSE yang akan digunakan dan meng-include header yang diperlukan untuk fungsi-fungsi yang akan digunakan dalam implementasi filesystem. root_dir adalah direktori root dari filesystem yang akan digunakan sebagai basis untuk operasi file dan direktori.

#### Fungsi custom_getattr
    static int custom_getattr(const char *path, struct stat *stbuf) {
        char full_path[1000];
        snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
        int result = lstat(full_path, stbuf);
        if (result == -1)
            return -errno;
        return 0;
    }
##### Fungsi ini digunakan untuk mendapatkan atribut file (metadata) seperti ukuran file, tipe file, dan izin akses. Fungsi lstat mengisi struktur stat dengan informasi tentang file yang diminta.

#### Fungsi custom_readdir
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
##### Fungsi ini digunakan untuk membaca isi direktori. Fungsi ini mengisi buffer dengan daftar nama file dan direktori yang ada dalam direktori yang diminta.

#### Fungsi custom_mkdir
	static int custom_mkdir(const char *path, mode_t mode) {
	    char full_path[1000];
	    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
	    int result = mkdir(full_path, mode);
	    if (result == -1)
	        return -errno;
	    return 0;
	}
##### Fungsi ini digunakan untuk membuat direktori baru. Fungsi mkdir membuat direktori dengan mode akses yang diberikan.

#### Fungsi custom_rmdir
	static int custom_rmdir(const char *path) {
	    char full_path[1000];
	    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
	    int result = rmdir(full_path);
	    if (result == -1)
	        return -errno;
	    return 0;
	}
##### Fungsi ini digunakan untuk menghapus direktori kosong. Fungsi rmdir menghapus direktori yang diminta.

#### Fungsi custom_rename
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
##### Fungsi ini digunakan untuk mengganti nama atau memindahkan file/direktori. Jika path tujuan mengandung "/wm", maka file sumber akan diberi watermark sebelum dipindahkan. Jika tidak, file/direktori hanya akan diganti namanya atau dipindahkan.

#### Fungsi custom_chmod
	static int custom_chmod(const char *path, mode_t mode) {
	    char full_path[1000];
	    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
	    if (chmod(full_path, mode) == -1)
	        return -errno;
	    return 0;
	}
##### Fungsi ini digunakan untuk mengubah izin akses file atau direktori. Fungsi chmod mengubah mode akses dari file yang diminta.

#### Fungsi custom_read
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
##### Fungsi ini digunakan untuk membaca isi file. Jika nama file diawali dengan "test", konten file akan dibaca, dibalik, dan kemudian dikembalikan. Jika tidak, fungsi akan membaca file seperti biasa.

#### Fungsi custom_unlink
	static int custom_unlink(const char *path) {
	    char full_path[1000];
	    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
	    int result = unlink(full_path);
	    if (result == -1)
	        return -errno;
	    return 0;
	}
##### Fungsi ini digunakan untuk menghapus file. Fungsi unlink menghapus file yang diminta.

#### Fungsi custom_open
	static int custom_open(const char *path, struct fuse_file_info *fi) {
	    char full_path[1000];
	    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
	    int result = open(full_path, fi->flags);
	    if (result == -1)
	        return -errno;
	    close(result);
	    return 0;
	}
##### Fungsi ini digunakan untuk membuka file. Fungsi open membuka file dengan mode yang diminta.

#### Fungsi custom_create
	static int custom_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	    char full_path[1000];
	    snprintf(full_path, sizeof(full_path), "%s%s", root_dir, path);
	    int result = creat(full_path, mode);
	    if (result == -1)
	        return -errno;
	    fi->fh = result;
	    return 0;
	}
##### Fungsi ini digunakan untuk membuat file baru. Fungsi creat membuat file dengan mode akses yang diberikan.

#### Fungsi custom_write
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
##### Fungsi ini digunakan untuk menulis data ke file. Fungsi pwrite menulis data ke file pada offset yang ditentukan.

#### Struktur fuse_operations
	    .chmod      = custom_chmod,
	    .read       = custom_read,
	    .unlink     = custom_unlink,
	    .open       = custom_open,
	    .create     = custom_create,
	    .write      = custom_write,
	};
##### Struktur fuse_operations ini menghubungkan fungsi-fungsi yang telah diimplementasikan sebelumnya dengan operasi-operasi yang dapat dilakukan pada filesystem FUSE. Setiap fungsi dihubungkan dengan operasi yang sesuai, seperti getattr untuk mendapatkan atribut file, readdir untuk membaca isi direktori, mkdir untuk membuat direktori baru, dan lain sebagainya.

#### Fungsi main
	int main(int argc, char *argv[]) {
	    umask(0);
	    return fuse_main(argc, argv, &custom_oper, NULL);
	}
##### Fungsi main adalah entry point dari program. Di dalamnya, dilakukan inisialisasi dengan umask(0) untuk memastikan hak akses yang sesuai pada file yang dibuat, dan kemudian program memanggil fuse_main untuk menjalankan filesystem dengan menggunakan operasi yang telah ditentukan dalam struktur fuse_operations.

### Dokumentasi
![image](https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/151918510/cc743f6f-15a2-471e-a28b-368c353b9ed4)


## Soal 2
> Nayla 5027231054

Di soal nomor 2, terdapat folder "sensitif" yang didapatkan dari email inikaryakita. Pada folder "sensitif", terdapat dua folder yang bernama "pesan" dan "rahasia-berkas".
1. Pada folder "pesan" Adfi ingin meningkatkan kemampuan sistemnya dalam mengelola berkas-berkas teks dengan menggunakan fuse.
* Jika sebuah file memiliki prefix "base64," maka sistem akan langsung mendekode isi file tersebut dengan algoritma Base64.
* Jika sebuah file memiliki prefix "rot13," maka isi file tersebut akan langsung di-decode dengan algoritma ROT13.
* Jika sebuah file memiliki prefix "hex," maka isi file tersebut akan langsung di-decode dari representasi heksadesimalnya.
* Jika sebuah file memiliki prefix "rev," maka isi file tersebut akan langsung di-decode dengan cara membalikkan teksnya.
2. Pada folder “rahasia-berkas”, Adfi dan timnya memutuskan untuk menerapkan kebijakan khusus. Mereka ingin memastikan bahwa folder dengan prefix "rahasia" tidak dapat diakses tanpa izin khusus. 
* Jika seseorang ingin mengakses folder dan file pada “rahasia”, mereka harus memasukkan sebuah password terlebih dahulu (password bebas). 
3. Setiap proses yang dilakukan akan tercatat pada logs-fuse.log dengan format : [SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]
* Ex:
  [SUCCESS]::01/11/2023-10:43:43::[moveFile]::[File moved successfully]

### Penyelesaian
```bash
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
```

### Penjelasan
1. Membuat file bernama pastibisa.c
```bash
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
```
Mendefinisikan versi fuse yang digunakan dan beberapa header yang diperlukan untuk fungsi-fungsi dan struktur data yang akan digunakan dalam program.
```bash
#define LOG_FILE "/home/nayla/soal2/logs-fuse.log"
static const char *base_path = "/home/nayla/soal2/sensitif";
const char *secret_password = "sisop";
```
Mendefinisikan path untuk lokasi file log nantinya, path dasar untuk setiap operasi fuse, dan password untuk mengakses folder dengan prefix rahasia.
##### log_message
```bash
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
```
Fungsi untuk mencatat setiap proses yang dilakukan ke dalam sebuah file bernama logs-fuse.log dengan format : [SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]
##### decode_base64
```bash
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
```
Fungsi decode_base64 bertujuan untuk mendekode string yang telah dienkripsi dalam format base64 menjadi string dengan karakter biasa.
* Fungsi menerima input string yang berisi teks dalam format Base64 dan menyediakan pointer ke string output yang akan menampung teks hasil dekode.
* Fungsi melakukan iterasi karakter per karakter pada teks input.
* Setiap karakter dalam teks Base64 dikonversi menjadi nilai numerik yang sesuai dalam rentang 0 hingga 63.
* Nilai-nilai ASCII ini ditambahkan ke string output setiap kali karakter ASCII yang valid telah dibentuk.
* Proses ini diulangi hingga semua karakter dalam teks input terbaca atau karakter '=' (pembatas) ditemukan.
* String output ditutup dengan karakter null-terminator ('\0') untuk menandakan akhir dari string.
##### decode_rot13
```bash
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
```
Fungsi decode_rot13 digunakan untuk melakukan ROT13 (Rotation by 13 Places) pada sebuah string.
* Fungsi menerima input berupa string yang akan di-decode dan pointer ke output string.
* Setiap karakter dalam string input diperiksa melalui loop.
* Jika karakter adalah huruf kecil atau besar, maka digeser sejauh 13 tempat, sedangkan karakter non-alfabet tidak berubah.
* Proses ini diulangi untuk setiap karakter dalam string input hingga selesai.
* Hasil akhir disimpan dalam string output.
##### decode_hex
```bash
void decode_hex(const char *input, char **output) {
    size_t len = strlen(input) / 2;
    *output = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        sscanf(input + 2 * i, "%2hhx", *output + i);
    }
    (*output)[len] = '\0';
}
```
Fungsi decode_hex bertujuan untuk mendekode string heksadesimal menjadi bentuk byte. Ini dilakukan dengan mengonversi setiap pasangan karakter heksadesimal menjadi nilai byte yang sesuai.
* Fungsi ini menerima string input yang berisi nilai heksadesimal.
* Panjang string input dibagi dua karena setiap pasangan karakter heksadesimal mewakili satu byte.
* Setiap pasangan karakter diubah menjadi nilai byte menggunakan sscanf.
* Byte-byte yang dihasilkan kemudian disusun menjadi sebuah string output yang terdekripsi.
##### reverse_text
```bash
void reverse_text(const char *input, char **output) {
    size_t len = strlen(input);
    *output = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        (*output)[i] = input[len - i - 1];
    }
    (*output)[len] = '\0';
}
```
Fungsi reverse_text bertugas untuk membalikkan teks yang diberikan sebagai input. Ini dilakukan dengan cara memindahkan karakter-karakter dari input ke output dalam urutan terbalik.
##### contains_keyword
```bash
bool contains_keyword(const char *path, const char *keyword) {
    return strstr(path, keyword) != NULL;
}
```
Fungsi contains_keyword digunakan untuk memeriksa apakah sebuah string mengandung kata kunci tertentu. Fungsi ini akan mengembalikan true jika string tersebut mengandung kata kunci yang ditentukan, dan false jika tidak. 
##### is_secret_folder
```bash
bool is_secret_folder(const char *path) {
    return contains_keyword(path, "rahasia");
}
```
Fungsi is_secret_folder adalah fungsi singkat yang memeriksa apakah suatu path merupakan folder rahasia. Fungsi ini melakukan pencarian string tertentu, dalam hal ini "rahasia", dalam path yang diberikan. Jika string tersebut ditemukan, fungsi akan mengembalikan nilai true, yang menandakan bahwa path tersebut adalah folder rahasia. Jika tidak ditemukan, maka fungsi akan mengembalikan nilai false. 
##### handle_secret_folder_access
```bash
bool handle_secret_folder_access() {
    // If password already set, prompt user to enter it
    char user_input[100];
    printf("Enter the password to access the secret folder: ");
    scanf("%s", user_input);
    if (strcmp(user_input, secret_password) != 0) {
        printf("Incorrect password. Access denied.\n");
        return false;
    }
    return true;
}
```
Fungsi handle_secret_folder_access bertugas untuk memeriksa apakah pengguna memiliki akses ke folder rahasia dengan meminta pengguna untuk memasukkan kata sandi yang benar. Jika kata sandi yang dimasukkan oleh pengguna sesuai dengan kata sandi yang telah ditentukan (secret_password), maka fungsi ini akan mengembalikan nilai true, yang menunjukkan bahwa pengguna memiliki akses ke folder rahasia. Jika kata sandi yang dimasukkan salah, maka fungsi akan mencetak pesan "Incorrect password. Access denied." dan mengembalikan nilai false, yang menunjukkan bahwa akses ditolak.
##### xmp_getattr
```bash
static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);

    // Check if accessing a secret folder
    if (is_secret_folder(path) && !handle_secret_folder_access(path)) {
            return -EACCES;
        }

    res = lstat(fpath, stbuf);
    if (res == -1) {
        return -errno;
    }
    return 0;
}
```
Fungsi xmp_getattr digunakan untuk mendapatkan atribut dari suatu file atau direktori. 
* Fungsi ini pertama-tama membentuk path lengkap file atau direktori dengan menggabungkan base_path dengan path yang diberikan.
* Jika path menunjukkan ke folder rahasia, maka fungsi akan memeriksa kata sandi untuk mengaksesnya. Jika kata sandi tidak sesuai, akan mengembalikan kode error EACCES yang menandakan akses ditolak.
* Selanjutnya, fungsi menggunakan lstat untuk mendapatkan informasi atribut file yang diidentifikasi oleh fpath. Informasi ini disimpan dalam struktur struct stat *stbuf.
* Jika operasi lstat gagal, fungsi akan mengembalikan kode error yang sesuai dengan nilai errno. Jika operasi berhasil, fungsi akan mengembalikan nilai 0, menandakan bahwa operasi berhasil dilakukan.
##### xmp_readdir
```bash
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
```
Fungsi xmp_readdir digunakan untuk membaca isi dari sebuah direktori.
* Fungsi menerima path dari direktori yang akan dibaca, buffer untuk menyimpan entri direktori, serta pointer ke fungsi filler yang digunakan untuk mengisi buffer dengan entri direktori.
* Path direktori diubah menjadi path fisik di dalam sistem file dengan menggabungkan base_path dengan path yang diberikan.
* Direktori dibuka dengan opendir. Jika gagal, fungsi mengembalikan kode error dan mencatat pesan kesalahan.
* Setiap entri dalam direktori dibaca dengan readdir, dan informasi mengenai setiap entri tersebut dimasukkan ke dalam struktur stat.
* Informasi tersebut kemudian dimasukkan ke dalam buffer menggunakan fungsi filler. Jika buffer penuh atau terjadi kesalahan, fungsi menghentikan proses pembacaan.
* Setelah selesai membaca, direktori ditutup dengan closedir.
* Fungsi mengembalikan 0 jika berhasil dan kode error jika terjadi kesalahan.
##### xmp_open
```bash
static int xmp_open(const char *path, struct fuse_file_info *fi) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);

    // Check if accessing a secret folder
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
```
Fungsi xmp_open digunakan untuk membuka file.
* Fungsi ini membangun path lengkap file yang akan dibuka dengan menggabungkan base_path dan path yang diberikan.
* Jika path yang diberikan mengarah ke folder rahasia dan akses ke folder tersebut memerlukan kata sandi, maka fungsi akan memeriksa kata sandi.
* Jika kata sandi tidak sesuai, fungsi akan mengembalikan -EACCES untuk menandakan akses ditolak.
* Jika kata sandi cocok atau folder tidak rahasia, fungsi akan mencoba membuka file yang dimaksud dengan menggunakan fungsi open.
* Jika berhasil membuka file, fungsi akan langsung menutupnya kembali karena operasi yang diinginkan hanya membuka file untuk memeriksa keberadaannya.
* Fungsi akan mengembalikan 0 jika operasi berjalan dengan sukses atau kode kesalahan yang sesuai jika terjadi kegagalan.
##### xmp_read
```bash
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", base_path, path);

    // Check if accessing a secret folder
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

    // Decode content if needed
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
```
Fungsi xmp_read digunakan untuk membaca isi dari sebuah file.
* Fungsi ini membuka file pada path yang diberikan.
* Jika path menuju ke folder rahasia, maka akan meminta kata sandi sebelum membaca file.
* Setelah membuka file, fungsi ini membaca isi file tersebut.
* Jika jalur file mengandung spesifikasi dekode seperti "base64", "rot13", "hex", atau "rev", maka konten file akan didekode sesuai dengan spesifikasi tersebut.
* Setelah membaca dan mendekode isi file (jika diperlukan), fungsi ini mencatat keberhasilan pembacaan file dalam log dan mengembalikan isi file yang telah dibaca.
```bash
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .open = xmp_open,
    .read = xmp_read,
};
```
Bagian tersebut adalah definisi dari sebuah struct bernama xmp_oper, yang merupakan implementasi dari operasi-operasi yang diperlukan dalam FUSE (Filesystem in Userspace).
```bash
int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```
Fungsi main yang menjalankan FUSE filesystem dengan operasi-operasi yang telah didefinisikan sebelumnya.

2. Compile program pastibisa.c
```bash
$ gcc -Wall `pkg-config fuse --cflags` pastibisa.c -o pastibisa `pkg-config fuse --libs`
```
3. Menjalankan fuse
```bash
$ mkdir [direktori tujuan]
$ ./[output] -f [direktori tujuan]
```

#### Dokumentasi Pengerjaan
<img width="1440" alt="Screenshot 2024-05-25 at 21 21 37" src="https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/80509033/938a3df8-0c1d-44ba-a1e2-6969c2134824">
<img width="1440" alt="Screenshot 2024-05-25 at 21 22 21" src="https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/80509033/0effc262-324f-4e47-b410-491f5fc510e4">
<img width="1440" alt="Screenshot 2024-05-25 at 21 23 17" src="https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/80509033/648d3f1e-7a6d-4b80-9683-9e92ce2be791">

## Soal 3
> Ryan 5027231046

3. Seorang arkeolog menemukan sebuah gua yang didalamnya tersimpan banyak relik dari zaman praaksara, sayangnya semua barang yang ada pada gua tersebut memiliki bentuk yang terpecah belah akibat bencana yang tidak diketahui. Sang arkeolog ingin menemukan cara cepat agar ia bisa menggabungkan relik-relik yang terpecah itu, namun karena setiap pecahan relik itu masih memiliki nilai tersendiri, ia memutuskan untuk membuat sebuah file system yang mana saat ia mengakses file system tersebut ia dapat melihat semua relik dalam keadaan utuh, sementara relik yang asli tidak berubah sama sekali.
Ketentuan :
a. Buatlah sebuah direktori dengan ketentuan seperti pada tree berikut
.
├── [nama_bebas]
├── relics
│   ├── relic_1.png.000
│   ├── relic_1.png.001
│   ├── dst dst…
│   └── relic_9.png.010
└── report

b. Direktori [nama_bebas] adalah direktori FUSE dengan direktori asalnya adalah direktori relics. Ketentuan Direktori [nama_bebas] adalah sebagai berikut :
Ketika dilakukan listing, isi dari direktori [nama_bebas] adalah semua relic dari relics yang telah tergabung.
Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.
Ketika ada file dibuat, maka pada direktori asal (direktori relics) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb.
File yang dipecah akan memiliki nama [namafile].000 dan seterusnya sesuai dengan jumlah pecahannya.
Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

c. Direktori report adalah direktori yang akan dibagikan menggunakan Samba File Server. Setelah kalian berhasil membuat direktori [nama_bebas], jalankan FUSE dan salin semua isi direktori [nama_bebas] pada direktori report.

### Penyelesaian
```
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
    exec_command("unzip -o discoveries.zip -d relics");

    printf("Combining relics\n"); // Logging
    combine_relics();

    // Jalankan FUSE dalam foreground untuk debugging yang lebih mudah
    char *fuse_argv[] = { argv[0], "-f", "-o", "nonempty", argv[1], NULL };
    int fuse_argc = 5;

    printf("Starting FUSE main loop\n"); // Logging
    return fuse_main(fuse_argc, fuse_argv, &operations, NULL);
}
```

### Penjelasan
```
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
```

FUSE_USE_VERSION 30: Mendefinisikan versi FUSE yang akan digunakan.
Header yang disertakan menyediakan berbagai fungsi dan tipe data yang diperlukan untuk operasi file, proses, dan direktori.
MAX_PARTS mendefinisikan jumlah maksimum bagian file.
BUFFER_SIZE dan PART_SIZE mendefinisikan ukuran buffer untuk operasi baca/tulis file.

```
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
```
Fungsi ini menjalankan perintah shell menggunakan fork dan execl.
Jika terjadi kesalahan pada fork atau waitpid, pesan kesalahan akan dicetak dan program keluar.

```
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
```
Fungsi ini menggabungkan file pecahan di relics_path menjadi satu file utuh di artefak_path.
Membuka setiap file pecahan, membacanya, dan menulisnya ke file gabungan.

```
static int custom_getattr(const char *path, struct stat *stbuf) {
    printf("getattr: %s\n", path);
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
```

Fungsi ini mengembalikan atribut file/direktori untuk path yang diberikan.
Menggunakan stat untuk mendapatkan informasi file dari artefak_path.

```
static int custom_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    printf("readdir: %s\n", path);
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
        printf("readdir: found %s\n", de->d_name);
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
```
Fungsi ini mengisi buffer dengan daftar file dan direktori di artefak_path.

```
static int custom_open(const char *path, struct fuse_file_info *fi) {
    printf("open: %s\n", path);
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
```
Fungsi ini membuka file yang ditentukan oleh path.

```
static int custom_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("read: %s\n", path);
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
```
Fungsi ini membaca data dari file yang ditentukan oleh path ke buffer buf.

```
static int custom_unlink(const char *path) {
    printf("unlink: %s\n", path);
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", artefak_path, path);
    int res = unlink(full_path);
    if (res == -1) {
        perror("unlink");
        return -errno;
    }
    return 0;
}
```

Fungsi ini menghapus file yang ditentukan oleh path.

```
static struct fuse_operations operations = {
    .getattr = custom_getattr,
    .readdir = custom_readdir,
    .open = custom_open,
    .read = custom_read,
    .write = custom_write,
    .unlink = custom_unlink,
};
```

Struktur ini mendefinisikan operasi yang didukung oleh sistem file FUSE, menghubungkan fungsi yang telah didefinisikan dengan operasi FUSE yang sesuai.

```
int main(int argc, char *argv[]) {
    printf("Starting main function\n");
    if (mkdir(artefak_path, 0755) != 0 && errno != EEXIST) {
        perror("mkdir artefak");
        exit(EXIT_FAILURE);
    }

    printf("Downloading discoveries.zip\n");
    exec_command("wget -O discoveries.zip \"https://drive.google.com/uc?id=1BJkaBvGaxqiwPWvXRdYNXzxxmIYQ8FKf\"");
    printf("Extracting discoveries.zip\n");
    exec_command("unzip -o discoveries.zip -d relics");

    printf("Combining relics\n");
    combine_relics();

    // Jalankan FUSE dalam foreground untuk debugging yang lebih mudah
    char *fuse_argv[] = { argv[0], "-f", "-o", "nonempty", argv[1], NULL };
    int fuse_argc = 5;

    printf("Starting FUSE main loop\n");
    return fuse_main(fuse_argc, fuse_argv, &operations, NULL);
}
```
Fungsi main adalah titik masuk program.
Membuat direktori artefak jika belum ada.
Mendownload dan mengekstrak file zip berisi relic.
Menggabungkan file pecahan di relics.
Menjalankan loop utama FUSE dengan mountpoint yang ditentukan (artefak).


### Dokumentasi
![image](https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/162680331/05f1c5ac-131d-4e6b-a182-51fbd1a2797d)
![image](https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/162680331/78879364-143d-4118-b6e8-929f9d242ca1)

### Kendala Pengerjaan
looping tidak berjalan dengan lancar sehingga membuat program menjadi stuck.
![image](https://github.com/nyy223/Sisop-4-2024-MH-IT01/assets/162680331/4ada7f19-0da4-491d-80d5-b0831ff651c2)


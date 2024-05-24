# Sisop-4-2024-MH-IT01

# Laporan Resmi Praktikum Sisop-3-2024-MH-IT01


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

#### Kendala yang dialami selama pengerjaan

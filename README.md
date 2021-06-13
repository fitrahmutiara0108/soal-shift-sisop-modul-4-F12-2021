# Soal Shift Modul 4 Sisop 2021 (Kelompok F12)
#### Nama anggota kelompok:
- Farhan Arifandi (05111940000061)
- Fitrah Mutiara (05111940000126)
- M. Iqbal Abdi (05111940000151)

## Soal 1
### Poin (a)
Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Untuk men-encode seluruh folder dan subfolder beserta file yang berada di dalam folder "AtoZ_[Nama]" dengan algoritma atbash cipher, maka akan dibuat fungsi `atbash()`. Atbash Cipher mencerminkan alfabet sesuai urutan (A menjadi Z, B menjadi Y, C menjadi X, dst., dan berlaku sebaliknya sehingga fungsi encode dan decode sama. Referensi tentang Atbash cipher dapat dibaca di `https://www.dcode.fr/atbash-cipher`.
```c
void atbash(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if(str[i] >= 'A' && str[i] <= 'Z') str[i] = 'Z' + 'A' - str[i];
		else if(str[i] >= 'a' && str[i] <= 'z') str[i] = 'z' + 'a' - str[i];
        else if (str[i] == 46) break;
        else continue;
    }
}
```
- Lalu, saat melakukan encoding, akan terjadi perbedaan nama folder dan file antara mount dan FUSE sehingga file menjadi tidak terdeteksi. Maka akan dibuat fungsi `get_original_directory()` dan `get_new_directory()` untuk menjaga agar direktori pada FUSE tetap sama dengan direktori mount-nya.
```c
void get_original_directory(char *input, char *output) {
    char fpath[1024];
    bzero(fpath, sizeof(fpath));
    sprintf(fpath, "%s%s", dirpath, input);
    if (strstr(fpath, "/AtoZ_")) {
        char mid_path[1024];

        strcpy(fpath, input);
        bzero(fpath, sizeof(fpath));

        strcpy(fpath, input);
        char *path = strstr(fpath, "/AtoZ_");
        printf("1 file path AtoZ: %s\n", path);
        char *enc = strtok(path, "/");

        if ((enc = strtok(NULL, "/"))) {
            char name[1024];
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);
            strcpy(name, strstr(fpath, enc));
            atbash(name);

            bzero(mid_path, sizeof(mid_path));
            strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
            bzero(fpath, sizeof(fpath));

            sprintf(output, "%s%s%s", dirpath, mid_path, name);
            printf("1.1 output path AtoZ: %s\n", output);
        }
    }
    else if (strstr(fpath, "/RX_") ) {
        char back_path[1024];

        bzero(fpath, sizeof(fpath));
        strcpy(fpath, input);
        bzero(fpath, sizeof(fpath));
        strcpy(fpath, input);

        char *path = strstr(fpath, "/RX_");
        printf("2 file path RX: %s\n", path);
        char *enc = strtok(path, "/");

        if ((enc = strtok(NULL, "/")) ) {
            char buffer[1024];
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);
            strcpy(buffer, strstr(fpath, enc));

            bzero(back_path, sizeof(back_path));
            strncpy(back_path, fpath, strlen(fpath) - strlen(buffer) - 1);

            bzero(fpath, sizeof(fpath));
            sprintf(fpath, "%s%s", dirpath, back_path);
            printf("2.1 fpath RX: %s\n", fpath);
        }

        if (in(renamed_count, fpath)) {
            char mid_path[1024];

            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);

            char *rec_path = strstr(fpath, "/RX_");
            printf("3 file path RX: %s\n", rec_path);
            char *rec_enc = strtok(rec_path, "/");

            if ((rec_enc = strtok(NULL, "/")) ) {
                char name[1024], decrypted[100];
                strcpy(name, rec_enc);

                bzero(decrypted, sizeof(decrypted));
                vigenere_decrypt(name, decrypted);
                atbash(decrypted);


                bzero(mid_path, sizeof(mid_path));
                strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                bzero(fpath, sizeof(fpath));

                sprintf(output, "%s%s/%s", dirpath, mid_path, decrypted);
                printf("3.1 output path RX: %s\n", output);
            }
        }
        else {
            char mid_path[1024];

            strcpy(fpath, input);
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);

            char *rec_path = strstr(fpath, "/RX_");
            printf("4 file path RX: %s\n", rec_path);
            char *rec_enc = strtok(rec_path, "/");
            printf("4.x file path RX: %s\n", rec_enc);

            if ((rec_enc = strtok(NULL, "/")) ) {
                char name[1024];

                bzero(fpath, sizeof(fpath));
                strcpy(fpath, input);
                strcpy(name, strstr(fpath, rec_enc));
                atbash(name);
                rot13(name);

                bzero(mid_path, sizeof(mid_path));
                strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                bzero(fpath, sizeof(fpath));

                sprintf(output, "%s%s%s", dirpath, mid_path, name);
                printf("4.1 output path RX: %s\n", output);
            }
        }
    }
    else sprintf(output, "%s%s", dirpath, input);
    printf("A1 output path: %s\n", output);
}
```
```c
void get_new_directory(char *input, char *output) {
    char fpath[1024];
    bzero(fpath, sizeof(fpath));
    sprintf(fpath, "%s%s", dirpath, input);
    if (strstr(fpath, "/AtoZ_") ) {
        char mid_path[1024];

        strcpy(fpath, input);
        bzero(fpath, sizeof(fpath));
        strcpy(fpath, input);

        char *path = strstr(fpath, "/AtoZ_");
        printf("5 file path AtoZ: %s\n", path);
        char *enc = strtok(path, "/");

        if ((enc = strtok(NULL, "/")) ) {
            char name[1024];
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);
            strcpy(name, strstr(fpath, enc));

            bzero(mid_path, sizeof(mid_path));
            strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
            bzero(output, sizeof(output));

            sprintf(output, "%s%s%s", dirpath, mid_path, name);
            printf("5.1 output path AtoZ: %s\n", output);
        }

        else {
            bzero(output, sizeof(output));
            sprintf(output, "%s%s", dirpath, input);
            printf("5.2 output path AtoZ: %s\n", output);
        }
    }
    else if (strstr(fpath, "RX_") ) {
        if (in(renamed_count, fpath)) {
            char mid_path[1024];

            strcpy(fpath, input);
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);

            char *path = strstr(fpath, "/RX_");
            printf("6 file path RX: %s\n", path);
            char *enc = strtok(path, "/");

            if ((enc = strtok(NULL, "/")) ) {
                char name[1024];
                bzero(fpath, sizeof(fpath));
                strcpy(fpath, input);
                strcpy(name, strstr(fpath, enc));

                bzero(mid_path, sizeof(mid_path));
                strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                bzero(output, sizeof(output));

                sprintf(output, "%s%s%s", dirpath, mid_path, name);
                printf("6.1 output path RX: %s\n", output);
            }
            else {
                bzero(output, sizeof(output));
                sprintf(output, "%s%s", dirpath, input);
                printf("6.2 output path RX: %s\n", output);
            }
        }
        else {
            char mid_path[1024];

            strcpy(fpath, input);
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, input);

            char *str = strstr(fpath, "/RX_");
            printf("7 file path RX: %s\n", str);
            char *enc = strtok(str, "/");

            if ((enc = strtok(NULL, "/")) ) {
                char name[1024];
                bzero(fpath, sizeof(fpath));
                strcpy(fpath, input);
                strcpy(name, strstr(fpath, enc));

                bzero(mid_path, sizeof(mid_path));
                strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                bzero(output, sizeof(output));

                sprintf(output, "%s%s%s", dirpath, mid_path, name);
                printf("7.1 output path RX: %s\n", output);
            }
            else {
                bzero(output, sizeof(output));
                sprintf(output, "%s%s", dirpath, input);
                printf("7.2 output path RX: %s\n", output);
            }
        }
    }
    else {
        bzero(output, sizeof(output));
        sprintf(output, "%s%s", dirpath, input);
        printf("A2 output path: %s\n", output);
    }
}
```
Fungsi `get_original_directory()` akan mengecek apakah awal direktori memiliki awalan kata `AtoZ_` atau `RX_` dan mendapatkan direktori asli pada direktori mount dari direktori yang sedang diakses. Jika awalannya *AtoZ_* maka akan dilakukan dekripsi dengan metode *Atbash Cipher*, jika awalannya *RX_* akan menggunakan *Atbash Ciper* ditambah metode *ROT13* yang akan digunakan untuk soal nomor 2.

### Poin (b)
Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Poin ini sama saja dengan poin (a), hanya saja direktori yang akan di-encode bukan hasil dari create, melainkan hasil dari rename.
- Direktori yang di-rename dengan awalan `AtoZ` akan di-encode menggunakan metode *Atbash Cipher*.
```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    int res;
    DIR *dp;
    struct dirent *dPtr;

    (void)offset;
    (void)fi;
    char fpath[2000], name[2000], cekrename[128];
    sprintf(fpath, "%s%s", dirpath, path);

    ...

    dp = opendir(fpath);
    if (!dp) return -errno;

    while ((dPtr = readdir(dp)) ) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = dPtr->d_ino;
        st.st_mode = dPtr->d_type << 12;

        char fileName[2000];
        strcpy(fileName, dPtr->d_name);

        if (strstr(path, "/AtoZ_")  && strcmp(".", fileName)  && strcmp("..", fileName) ) {
            printf("12 read file path AtoZ: %s\n", path);
            if (!strstr(fileName, "."))  atbash(fileName);
            else {
                char ext[1024], arr[100][1024], new_name[1024];
                strcpy(ext, strstr(fileName, "."));
                char *get_dot = strtok(fileName, ".");
                int n = 0;
                while (get_dot ) {
                    strcpy(arr[n++], get_dot);
                    get_dot = strtok(NULL, ".");
                }
                strcpy(new_name, arr[n - 2]);
                atbash(new_name);
                bzero(fileName, sizeof(fileName));
                sprintf(fileName, "%s.%s", new_name, arr[n - 1]);
                printf("12.1 file name AtoZ: %s\n", fileName);
            }
        }
        ...
        }
        res = (filler(buf, fileName, &st, 0));
        if (res) break;
    }
    write_info("CD", path);
    closedir(dp);
    return 0;
}
```
Pada fungsi `xmp_readdir()`, akan dicek apakah proses yang dilakukan adalah *create* atau *rename*. Lalu string yang bakal di-encode hanyalah nama file pada subfoldernya saja, tidak sampai ke ekstensi nya.

### Poin (c)
Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

### Poin (d)
Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
- Fungsi `open_log()` akan dijalankan pada saat program fuse dimulai untuk menghasilkan file fuse.log untuk mencatat operasi mkdir atau rename, dan berlokasi di home.
```c
void open_log() {
    if(access("/home/farhan/fuse.log", F_OK)) {
		FILE *fp = fopen("/home/farhan/fuse.log", "w+");
		fclose(fp);
	} 
}
...
int main(int argc, char *argv[]) {
    open_log();
    umask(0);
    renamed_count = 0;
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```
- Ketika mkdir atau rename berhasil dijalankan, dengan syarat perubahan nama dilakukan dari folder yang tidak terenkripsi menjadi folder terenkripsi (nama diawali 'AtoZ_' dan tidak berlaku sebaliknya, karena yang diminta adalah pembuatan folder terenkripsi), append akan dilakukan pada file fuse.log yang telah dibuat.
```c
static int xmp_mkdir(const char *path, mode_t mode) {
    ...

    int res = mkdir(fpath, mode);
    if (res == -1) return -errno;
    else {
        if(strstr(path, "AtoZ_")) {
            FILE *log = fopen("/home/farhan/fuse.log", "a");
            fprintf(log, "MKDIR: %s%s\n", dirpath, path);
            fclose(log);
        }
        ...
    }

    return 0;
}

...

static int xmp_rename(const char *from, const char *to, unsigned int flags){
    ...

    int res = rename(source, dest);
    if (res == -1) return -errno;
    else{
        ...
        if(strstr(to, "AtoZ_") || strstr(to, "RX_")) {
            FILE *log = fopen("/home/farhan/fuse.log", "a");
            fprintf(log, "RENAME: %s -> %s\n", source, dest);
            fclose(log);
        }
    }
    return 0;
}

```
#### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121810067-e9d05180-cc91-11eb-8f35-475f1475958a.png)


### Poin (e)
- Proses encode-decode ini sudah dilakukan dengan metode recursive sampai ke subfolder di dalam direktorinya.
#### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121810159-459ada80-cc92-11eb-8d69-15db6aa21d08.png)


### Kendala dan error selama pengerjaan
- Sebelum revisi, operasi pengubahan (rename, mkdir, delete, dll.) melalui FUSE menyebabkan FUSE crash
- Enkripsi tidak berjalan

## Soal 2
### Poin (a)
Jika sebuah direktori dibuat dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1 dengan algoritma tambahan ROT13 (Atbash + ROT13).
- Program akan mengecek apakah direktori berawalan "RX_". Jika direktori adalah folder maka namanya akan di-encode seluruhnya, sedangkan jika direktori adalah file maka akan diambil terlebih dahulu nama file sebelum '.' terakhir (menandakan selanjutnya adalah ekstensi) untuk kemudian di-encode. 
```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    int res;
    DIR *dp;
    struct dirent *dPtr;

    (void)offset;
    (void)fi;
    char fpath[2000], name[2000], cekrename[128];
    sprintf(fpath, "%s%s", dirpath, path);

    ...

    dp = opendir(fpath);
    if (!dp) return -errno;

    while ((dPtr = readdir(dp)) ) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = dPtr->d_ino;
        st.st_mode = dPtr->d_type << 12;

        char fileName[2000];
        strcpy(fileName, dPtr->d_name);

        ...
        else if (strstr(path, "/RX_") && strcmp(".", fileName) && strcmp("..", fileName)) {
            printf("13 read file path RX: %s\n", path);
            if (in(renamed_count, cekrename)) {
                ...
            }
            else {
                printf("14 file name: %s\n", fileName);
                if (!strstr(fileName, ".")) {
                    atbash(fileName);
                    rot13(fileName);
                    printf("14.1 file name: %s\n", fileName);
                }
                else {
                    char ext[1024], arr[100][1024], new_name[1024];
                    strcpy(ext, strstr(fileName, "."));
                    char *get_dot = strtok(fileName, ".");
                    int n = 0;
                    while (get_dot ) {
                        strcpy(arr[n++], get_dot);
                        get_dot = strtok(NULL, ".");
                    }
                    strcpy(new_name, arr[n - 2]);
                    atbash(new_name);
                    rot13(new_name);

                    bzero(fileName, sizeof(fileName));
                    sprintf(fileName, "%s.%s", new_name, arr[n - 1]);
                    printf("14.2 file name: %s\n", fileName);
                }
            }
        }
        res = (filler(buf, fileName, &st, 0));
        if (res) break;
    }
    write_info("CD", path);
    closedir(dp);
    return 0;
}
```

- Isi direktori di-encode dengan Atbash cipher (sudah dijelaskan di atas) + ROT13. Sama seperti Atbash cipher, ROT13 juga berlaku dua arah sehingga fungsi encode dan decode bisa dijadikan satu.
```c
void rot13(char *str) {
    for (int i = 0; i < strlen(str); i++) { 
        if (str[i] >= 'A' && str[i] <= 'Z') {
            if(str[i]-'A'+1 <= 13) str[i] += 13;
            else str[i] -= 13;
        }
        else if (str[i] >= 'a' && str[i] <= 'z') {
            if(str[i]-'a'+1 <= 13) str[i] += 13;
            else str[i] -= 13;
        }
        else if (str[i] == 46) break;
        else continue;
    }
}
```
#### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121809043-da4f0980-cc8d-11eb-8724-47ec7ccd1ec0.png)


### Poin (b) **(masih belum berfungsi)**
Jika sebuah direktori di-rename dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key “SISOP” (Case-sensitive, Atbash + Vigenere).
- Program menyimpan riwayat direktori yang pernah di-rename dalam array `renamed_dir` dan jumlahnya dalam variabel `renamed_count`. Ketika rename dilakukan, direktori disimpan dan jumlahnya ditambah.
```c
char renamed_dir[100][1024];
int renamed_count;
...
static int xmp_rename(const char *from, const char *to, unsigned int flags){

    char source[1000];
    sprintf(source, "%s%s", dirpath, from);
    if (access(source, F_OK) == -1) {
        memset(source, 0, sizeof(source));
        get_original_directory(from, source);
    }

    char dest[1000];
    sprintf(dest, "%s%s", dirpath, to);
    if (access(dest, F_OK) == -1) {
        memset(dest, 0, sizeof(dest));
        get_new_directory(to, dest);
    }

    int res = rename(source, dest);
    if (res == -1) return -errno;
    else{
        ...
        strcpy(renamed_dir[renamed_count++], source);
        strcpy(renamed_dir[renamed_count++], dest);
        ...
    }
    return 0;
}
```
- Fungsi `in()` akan mengecek apakah direktori yang akan diakses pernah direname atau tidak. Jika ya, maka akan mengembalikan nilai 1, dan jika tidak maka 0.
```c
int in(int len, char *target){
    for (int i = 0; i < len; i++) {
        if (strncmp(renamed_dir[i], target, strlen(target)) == 0) 
            return 1;
    }
    return 0;
}
```
- Pada saat direktori dibuka, fungsi `in()` dipanggil dan jika nilai yang dikembalikan adalah 1 (menandakan direktori sudah direname), maka nama isi direktori akan di-encode dengan Atbash cipher dan Vigenere cipher.
```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    
    int res;
    DIR *dp;
    struct dirent *dPtr;

    (void)offset;
    (void)fi;
    char fpath[2000], name[2000], cekrename[128];
    sprintf(fpath, "%s%s", dirpath, path);

    ...

    dp = opendir(fpath);
    if (!dp) return -errno;

    while ((dPtr = readdir(dp)) ) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = dPtr->d_ino;
        st.st_mode = dPtr->d_type << 12;

        char fileName[2000];
        strcpy(fileName, dPtr->d_name);

        ...

        else if (strstr(path, "/RX_") && strcmp(".", fileName) && strcmp("..", fileName)) {
            printf("13 read file path RX: %s\n", path);
            if (in(renamed_count, cekrename)) {
                if (!strstr(fileName, ".")) {
                    char old_name[100], encrypted[100];
                    bzero(old_name, sizeof(old_name));
                    strcpy(old_name, fileName);
                    atbash(old_name);
                    
                    bzero(encrypted, sizeof(encrypted));
                    vigenere_encrypt(old_name, encrypted);
                    bzero(fileName, sizeof(fileName));
                    strcpy(fileName, encrypted);
                    printf("13.1 file name RX: %s\n", fileName);
                }
                else {
                    char ext[1024], arr[100][1024], new_name[1024];
                    strcpy(ext, strstr(fileName, "."));
                    char *get_dot = strtok(fileName, ".");
                    int n = 0;
                    while (get_dot ) {
                        strcpy(arr[n++], get_dot);
                        get_dot = strtok(NULL, ".");
                    }
                    strcpy(new_name, arr[n - 2]);
                    atbash(new_name);

                    char encrypted[100];
                    bzero(encrypted, sizeof(encrypted));
                    vigenere_encrypt(new_name, encrypted);
                    bzero(fileName, sizeof(fileName));
                    sprintf(fileName, "%s.%s", encrypted, arr[n - 1]);
                    printf("13.2 file name RX: %s\n", fileName);
                }
            }
            ...
        }
        res = (filler(buf, fileName, &st, 0));
        if (res) break;
    }
    write_info("CD", path);
    closedir(dp);
    return 0;
}
```
- Vigenere cipher sendiri meng-encode tiap huruf nama direktori yang dipetakan menuju key "SISOP" yang diulang-ulang sebanyak banyak huruf pada nama direktori (tidak termasuk spasi dan case-sensitive), kemudian huruf digeser dengan ketentuan "A = huruf pada key yang dipetakan pada huruf yang akan di-encode". Vigenere cipher tidak berlaku dua arah sehingga fungsi encode dan decode harus dibuat terpisah.
```c
void vigenere_encrypt(char *input, char *output) {
    char key[] = "SISOP";
	char buf[100], new_key[strlen(input)], result[strlen(input)];
    int i, j, k = 0;
    strcpy(buf, input);

    for (i=0, j=0; i < strlen(input); ++i, ++j) {
        if (buf[i] == 32) {
        	j--;
			continue;
		}
        if (j == (strlen(key))) j = 0;
        new_key[i] = key[j];
    }

    new_key[i] = '\0';

    for (i = 0; i < strlen(input); i++) {
        if ((buf[i] >= 'a' && buf[i] <= 'z') || (buf[i] >= 'A' && buf[i] <= 'Z')) {
            if (buf[i] >= 'A' && buf[i] <= 'Z')
                result[i] = ((buf[i] - 'A' + new_key[i] - 'A') % 26) + 'A';
            else if (buf[i] >= 'a' && buf[i] <= 'z')
                result[i] = ((buf[i] - 'a' + new_key[i] - 'A') % 26) + 'a';
        }
        else if (buf[i] == 46) {
            for (k = i; k < strlen(buf); k++) {
                buf[i] = buf[k];
                result[k] = buf[i];
            }
            break;
        }
        else result[i] = buf[i];
    }

    if (k < i) result[i] = 0;
    strcpy(output, result);
}

void vigenere_decrypt(char *input, char *output) {
    char key[] = "SISOP";
	char buf[100], new_key[strlen(input)], result[strlen(input)];
    int i, j, k = 0;
    strcpy(buf, input);

    for (i=0, j=0; i < strlen(input); ++i, ++j) {
        if (buf[i] == 32) {
        	j--;
			continue;
		}
        if (j == (strlen(key))) j = 0;
        new_key[i] = key[j];
    }

    new_key[i] = '\0';

    for (i = 0; i < strlen(input); i++) {
        if ((buf[i] >= 'a' && buf[i] <= 'z') || (buf[i] >= 'A' && buf[i] <= 'Z')) {
            if (buf[i] >= 'A' && buf[i] <= 'Z')
                result[i] = ((buf[i] - new_key[i] + 26) % 26) + 'A';
            else if (buf[i] >= 'a' && buf[i] <= 'z')
                result[i] = ((buf[i] - new_key[i] - 6) % 26) + 'a';
        }
        else if (buf[i] == 46) {
            for (k = i; k < strlen(buf); k++) {
                buf[i] = buf[k];
                result[k] = buf[i];
            }
            break;
        }
        else result[i] = buf[i];
    }

    if (k < i) result[i] = 0;
    strcpy(output, result);
}
```
#### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121809609-09ff1100-cc90-11eb-8472-a038dfc0e6b4.png)

### Poin (c)
Apabila direktori yang terencode di-rename (Dihilangkan “RX_” nya), maka folder menjadi tidak terencode dan isi direktori tersebut akan terdecode berdasar nama aslinya.
- Pada blok kode di bawah, program akan menentukan apakah nama direktori diawali dengan 'RX_'. Jika ya, maka nama isi direktori akan di-encode dengan Atbash dan ROT13, dan jika tidak, maka ditampilkan sesuai nama pada direktori mount, tanpa encoding apapun.
```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    int res;
    DIR *dp;
    struct dirent *dPtr;

    (void)offset;
    (void)fi;
    char fpath[2000], name[2000], cekrename[128];
    sprintf(fpath, "%s%s", dirpath, path);

    ...

    dp = opendir(fpath);
    if (!dp) return -errno;

    while ((dPtr = readdir(dp)) ) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = dPtr->d_ino;
        st.st_mode = dPtr->d_type << 12;

        char fileName[2000];
        strcpy(fileName, dPtr->d_name);

        ...
        else if (strstr(path, "/RX_") && strcmp(".", fileName) && strcmp("..", fileName)) {
            printf("13 read file path RX: %s\n", path);
            if (in(renamed_count, cekrename)) {
                if (!strstr(fileName, ".")) {
                    char old_name[100], encrypted[100];
                    bzero(old_name, sizeof(old_name));
                    strcpy(old_name, fileName);
                    atbash(old_name);
                    
                    bzero(encrypted, sizeof(encrypted));
                    vigenere_encrypt(old_name, encrypted);
                    bzero(fileName, sizeof(fileName));
                    strcpy(fileName, encrypted);
                    printf("13.1 file name RX: %s\n", fileName);
                }
                else {
                    char ext[1024], arr[100][1024], new_name[1024];
                    strcpy(ext, strstr(fileName, "."));
                    char *get_dot = strtok(fileName, ".");
                    int n = 0;
                    while (get_dot ) {
                        strcpy(arr[n++], get_dot);
                        get_dot = strtok(NULL, ".");
                    }
                    strcpy(new_name, arr[n - 2]);
                    atbash(new_name);

                    char encrypted[100];
                    bzero(encrypted, sizeof(encrypted));
                    vigenere_encrypt(new_name, encrypted);
                    bzero(fileName, sizeof(fileName));
                    sprintf(fileName, "%s.%s", encrypted, arr[n - 1]);
                    printf("13.2 file name RX: %s\n", fileName);
                }
            }
            else {
                printf("14 file name: %s\n", fileName);
                if (!strstr(fileName, ".")) {
                    atbash(fileName);
                    rot13(fileName);
                    printf("14.1 file name: %s\n", fileName);
                }
                else {
                    char ext[1024], arr[100][1024], new_name[1024];
                    strcpy(ext, strstr(fileName, "."));
                    char *get_dot = strtok(fileName, ".");
                    int n = 0;
                    while (get_dot ) {
                        strcpy(arr[n++], get_dot);
                        get_dot = strtok(NULL, ".");
                    }
                    strcpy(new_name, arr[n - 2]);
                    atbash(new_name);
                    rot13(new_name);

                    bzero(fileName, sizeof(fileName));
                    sprintf(fileName, "%s.%s", new_name, arr[n - 1]);
                    printf("14.2 file name: %s\n", fileName);
                }
            }
        }
        res = (filler(buf, fileName, &st, 0));
        if (res) break;
    }
    write_info("CD", path);
    closedir(dp);
    return 0;
}
```
#### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121809561-dcb26300-cc8f-11eb-9f9c-83add32d150f.png)

### Poin (d)
Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat ke sebuah log file beserta methodnya (apakah itu mkdir atau rename).
- Ketika mkdir atau rename berhasil dijalankan, dengan syarat perubahan nama dilakukan dari folder yang tidak terenkripsi menjadi folder terenkripsi (nama diawali 'RX_' dan tidak berlaku sebaliknya, karena yang diminta adalah pembuatan folder terenkripsi), append akan dilakukan pada file fuse.log yang telah dibuat (pada poin 1d).
```c
static int xmp_mkdir(const char *path, mode_t mode) {
    ...

    int res = mkdir(fpath, mode);
    if (res == -1) return -errno;
    else {
        if(strstr(path, "AtoZ_")) {
            FILE *log = fopen("/home/farhan/fuse.log", "a");
            fprintf(log, "MKDIR: %s%s\n", dirpath, path);
            fclose(log);
        }
        ...
    }

    return 0;
}

...

static int xmp_rename(const char *from, const char *to, unsigned int flags){
    ...

    int res = rename(source, dest);
    if (res == -1) return -errno;
    else{
        ...
        if(strstr(to, "AtoZ_") || strstr(to, "RX_")) {
            FILE *log = fopen("/home/farhan/fuse.log", "a");
            fprintf(log, "RENAME: %s -> %s\n", source, dest);
            fclose(log);
        }
    }
    return 0;
}

```
#### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121809379-22226080-cc8f-11eb-8afe-d8808e6291db.png)

### Poin (e)
Pada metode enkripsi ini, file-file pada direktori asli akan menjadi terpecah menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. Sebagai contoh, Suatu_File.txt berukuran 3 kiloBytes pada directory asli akan menjadi 3 file kecil yakni:
`Suatu_File.txt.0000`

`Suatu_File.txt.0001`

`Suatu_File.txt.0002`

Ketika diakses melalui filesystem hanya akan muncul `Suatu_File.txt`.
```
Belum dikerjakan
```

### Kendala dan error selama pengerjaan
- Rename folder dengan awalan 'RX_' melalui FUSE kemudian membuka folder yang direname tersebut menyebabkan FUSE crash, sedangkan jika FUSE di-restart, isi folder dengan awalan nama 'RX_' tetap terenkripsi dan bisa dibuka

![image](https://user-images.githubusercontent.com/70105993/121809620-13887900-cc90-11eb-93ec-61c073b50c73.png)

- Enkripsi menghasilkan karakter non-alfabetik

![image](https://user-images.githubusercontent.com/70105993/121808915-4f6e0f00-cc8d-11eb-98ca-82c18659d8a2.png)


## Soal 3
- Jika sebuah direktori dibuat dengan awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
- Jika sebuah direktori di-rename dengan memberi awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
- Apabila direktori yang terenkripsi di-rename dengan menghapus “A_is_a_” pada bagian awal nama folder maka direktori tersebut menjadi direktori normal.
- Direktori spesial adalah direktori yang mengembalikan enkripsi/encoding pada direktori “AtoZ_” maupun “RX_” namun masing-masing aturan mereka tetap berjalan pada direktori di dalamnya (sifat recursive  “AtoZ_” dan “RX_” tetap berjalan pada subdirektori).
- Pada direktori spesial semua nama file (tidak termasuk ekstensi) pada fuse akan berubah menjadi lowercase insensitive dan diberi ekstensi baru berupa nilai desimal dari biner perbedaan namanya.
```
Belum dikerjakan
```

## Soal 4
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.
- Log system yang akan terbentuk bernama `SinSeiFS.log` pada direktori home pengguna `/home/[user]/SinSeiFS.log)`. Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
- log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
- Untuk log level WARNING, digunakan untuk mencatat syscall `rmdir` dan `unlink`.
- Sisanya, akan dicatat pada level INFO.
- Format untuk logging yaitu:
`[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]`
- Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan

`INFO::28052021-10:00:00:CREATE::/test.txt`
`INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt`

```c
void write_info(char *text, char* path){
    char* level = "INFO";
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char log[2000];
    sprintf(log, "%s::%02d%02d%04d-%02d:%02d:%02d:%s::%s", level, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec, text, path);
	FILE *out = fopen("/home/farhan/SinSeiFS.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
}

void write_warning(char *text, char* path){ //rmdir dan unlink
    char* level = "WARNING";
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char log[2000];
    sprintf(log, "%s::%02d%02d%04d-%02d:%02d:%02d:%s::%s", level, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec, text, path);
	FILE *out = fopen("/home/farhan/SinSeiFS.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
}
```
- `year + 1900` dan `month + 1` untuk menghasilkan tahun dan bulan yang sesuai dengan sekarang 
- `sprintf()` akan mencetak string pada variabel `log` sesuai dengan format log yang sudah ditentukan
- Selanjutnya dengan bantuan `fopen()` akan melakukan appending untuk lognya secara berkala dari string `log` yang sudah disusun tadi.
- Untuk command RENAME, sebelum log dicatat, path nama awal dan akhir terlebih dahulu dimasukkan dalam string dengan format sesuai contoh, lalu string tersebut akan dicetak ke dalam log melalui fungsi `write_info()`
```c
static int xmp_rename(const char *from, const char *to, unsigned int flags){
    ...
    int res = rename(source, dest);
    if (res == -1) return -errno;
    else{
        char write[2048];
        sprintf(write, "%s::%s", from, to);
        write_info("RENAME", write);
        ...
```
### Dokumentasi
![image](https://user-images.githubusercontent.com/70105993/121808510-6f9cce80-cc8b-11eb-98d5-49339e4549be.png)

### Kendala dan Error selama pengerjaan
![Screenshot (1167)](https://user-images.githubusercontent.com/81247727/121806769-6c014b80-cc7b-11eb-8bfd-3cb4e1e57c45.png)
- Terdapat warning message yang muncul

# Soal Shift Modul 4 Sisop 2021 (Kelompok F12)
#### Nama anggota kelompok:
- Farhan Arifandi (05111940000061)
- Fitrah Mutiara (05111940000126)
- M. Iqbal Abdi (05111940000151)

## Soal 1
- Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
- Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
- Nama isi direktori berawalan "AtoZ_" di-encode menggunakan Atbash Cipher, yaitu sistem pengkodean mencerminkan alfabet sesuai urutan (A menjadi Z, B menjadi Y, C menjadi X, dan seterusnya, berlaku sebaliknya).
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

## Soal 2
- Jika sebuah direktori dibuat dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1 dengan algoritma tambahan ROT13 (Atbash + ROT13).
- Jika sebuah direktori di-rename dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key “SISOP” (Case-sensitive, Atbash + Vigenere).
- Apabila direktori yang terencode di-rename (Dihilangkan “RX_” nya), maka folder menjadi tidak terencode dan isi direktori tersebut akan terdecode berdasar nama aslinya.
- Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat ke sebuah log file beserta methodnya (apakah itu mkdir atau rename).
- Pada metode enkripsi ini, file-file pada direktori asli akan menjadi terpecah menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. Sebagai contoh, Suatu_File.txt berukuran 3 kiloBytes pada directory asli akan menjadi 3 file kecil yakni:
`Suatu_File.txt.0000`
`Suatu_File.txt.0001`
`Suatu_File.txt.0002`
```

```

## Soal 3
- Jika sebuah direktori dibuat dengan awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
- Jika sebuah direktori di-rename dengan memberi awalan “A_is_a_”, maka direktori tersebut akan menjadi sebuah direktori spesial.
- Apabila direktori yang terenkripsi di-rename dengan menghapus “A_is_a_” pada bagian awal nama folder maka direktori tersebut menjadi direktori normal.
- Direktori spesial adalah direktori yang mengembalikan enkripsi/encoding pada direktori “AtoZ_” maupun “RX_” namun masing-masing aturan mereka tetap berjalan pada direktori di dalamnya (sifat recursive  “AtoZ_” dan “RX_” tetap berjalan pada subdirektori).
- Pada direktori spesial semua nama file (tidak termasuk ekstensi) pada fuse akan berubah menjadi lowercase insensitive dan diberi ekstensi baru berupa nilai desimal dari binner perbedaan namanya.
```

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

### Kendala dan Error selama pengerjaan
![Screenshot (1167)](https://user-images.githubusercontent.com/81247727/121806769-6c014b80-cc7b-11eb-8bfd-3cb4e1e57c45.png)
- Terdapat warning message yang muncul

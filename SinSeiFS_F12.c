#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/xattr.h>
#include <sys/wait.h>
#include <pthread.h>

static const char *dirpath = "/home/farhan/Downloads";

char renamed_dir[100][1024];
int renamed_count;

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

void open_log() {
    if(access("/home/farhan/fuse.log", F_OK)) {
		FILE *fp = fopen("/home/farhan/fuse.log", "w+");
		fclose(fp);
	} 
}

void atbash(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if(str[i] >= 'A' && str[i] <= 'Z') str[i] = 'Z' + 'A' - str[i];
		else if(str[i] >= 'a' && str[i] <= 'z') str[i] = 'z' + 'a' - str[i];
        else if (str[i] == 46) break;
        else continue;
    }
}

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

int in(int len, char *target){
    for (int i = 0; i < len; i++) {
        if (strncmp(renamed_dir[i], target, strlen(target)) == 0)  return 1;
    }
    return 0;
}

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

static int xmp_unlink(const char *path) {
    char fpath[2000];
    bzero(fpath, sizeof(fpath));
    get_original_directory(path, fpath);

    int res = unlink(fpath);
    write_warning("UNLINK", fpath);
    if (res == -1) return -errno;
    return 0;
}

static int xmp_access(const char *path, int mask) {
    char fpath[2000];
    sprintf(fpath, "%s%s", dirpath, path);
    if (access(fpath, F_OK) == -1)
    {
        memset(fpath, 0, 2000);
        get_original_directory(path, fpath);
    }

    int res = access(fpath, mask);
	if (res == -1) return -errno;
    write_info("ACCESS", fpath);
	return 0;
}

static int xmp_rmdir(const char *path) {
    char fpath[2000];
    get_original_directory(path, fpath);

    int res = rmdir(fpath);
    if (res == -1)  return -errno;
    else write_warning("RMDIR", fpath);
    return 0;
}

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
        char write[2048];
        sprintf(write, "%s::%s", from, to);
        write_info("RENAME", write);
        strcpy(renamed_dir[renamed_count++], source);
        strcpy(renamed_dir[renamed_count++], dest);
        if(strstr(to, "AtoZ_") || strstr(to, "RX_")) {
            FILE *log = fopen("/home/farhan/fuse.log", "a");
            fprintf(log, "RENAME: %s -> %s\n", source, dest);
            fclose(log);
        }
    }
    return 0;
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    char fpath[2000];
    char temp[1024];
    strcpy(temp, path);
    sprintf(fpath, "%s%s", dirpath, path);
    if (access(fpath, F_OK) == -1) {
        bzero(fpath, sizeof(fpath));
        get_original_directory(temp, fpath);
    }

    int res = lstat(fpath, stbuf);
    if (res == -1) return -errno;
	else write_info("LS", path);
    return 0;
}

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

    if (strcmp(path, "/") == 0) {
        bzero(fpath, sizeof(fpath));
        sprintf(fpath, "%s", dirpath);
        printf("A3 file path: %s\n", fpath);
    }
    else {
        if (strstr(fpath, "/AtoZ_") ) {
            char mid_path[1024];

            bzero(fpath, sizeof(fpath));
            strcpy(fpath, path);
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, path);

            char *str = strstr(fpath, "/AtoZ_");
            printf("8 file path AtoZ: %s\n", str);
            char *enc = strtok(str, "/");

            if ((enc = strtok(NULL, "/")) ) {
                char name[1024];

                bzero(fpath, sizeof(fpath));
                strcpy(fpath, path);
                bzero(name, sizeof(name));
                strcpy(name, strstr(fpath, enc));
                atbash(name);

                bzero(mid_path, sizeof(mid_path));
                strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                bzero(fpath, sizeof(fpath));

                sprintf(fpath, "%s%s%s", dirpath, mid_path, name);
                printf("8.1 file path AtoZ: %s\n", fpath);
            }
            else sprintf(fpath, "%s%s", dirpath, path);
            printf("8.2 file path AtoZ: %s\n", fpath);
        }
        else if (strstr(fpath, "/RX_") ) {
            char buffer_path[1024];

            bzero(fpath, sizeof(fpath));
            strcpy(fpath, path);
            bzero(fpath, sizeof(fpath));
            strcpy(fpath, path);

            char *str = strstr(fpath, "/RX_");
            printf("9 file path RX: %s\n", str);
            char *enc = strtok(str, "/");

            if ((enc = strtok(NULL, "/")) ) {
                char buffer_name[1024];

                bzero(fpath, sizeof(fpath));
                strcpy(fpath, path);
                bzero(buffer_name, sizeof(buffer_name));
                strcpy(buffer_name, strstr(fpath, enc));

                atbash(buffer_name);

                char decrypted[100];
                bzero(decrypted, sizeof(decrypted));
                vigenere_decrypt(buffer_name, decrypted);
                atbash(decrypted);

                bzero(buffer_path, sizeof(buffer_path));
                strncpy(buffer_path, fpath, strlen(fpath) - strlen(buffer_name) - 1);
                bzero(fpath, sizeof(fpath));

                sprintf(fpath, "%s%s", dirpath, buffer_path);
                sprintf(cekrename, "%s%s", dirpath, buffer_path);
                printf("9.1 file path RX: %s\n", fpath);
            }

            if (in(renamed_count, fpath)) {
                char mid_path[1024];

                bzero(fpath, sizeof(fpath));
                strcpy(fpath, path);
                bzero(fpath, sizeof(fpath));
                strcpy(fpath, path);

                char *str = strstr(fpath, "/RX_");
                printf("10 file path RX: %s\n", str);
                char *enc = strtok(str, "/");

                if ((enc = strtok(NULL, "/")) ) {
                    char name[1024];

                    bzero(fpath, sizeof(fpath));
                    strcpy(fpath, path);
                    bzero(name, sizeof(name));
                    strcpy(name, strstr(fpath, enc));

                    char decrypted[100];
                    bzero(decrypted, sizeof(decrypted));
                    vigenere_decrypt(name, decrypted);
                    atbash(decrypted);

                    bzero(mid_path, sizeof(mid_path));
                    strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                    bzero(fpath, sizeof(fpath));

                    sprintf(fpath, "%s%s%s", dirpath, mid_path, decrypted);
                    printf("10.1 file path RX: %s\n", fpath);
                }
                else sprintf(fpath, "%s%s", dirpath, path);
                printf("10.2 file path RX: %s\n", fpath);
            }
            else {
                char mid_path[1024];

                bzero(fpath, sizeof(fpath));
                strcpy(fpath, path);
                bzero(fpath, sizeof(fpath));
                strcpy(fpath, path);

                char *str = strstr(fpath, "/RX_");
                printf("11 file path RX: %s\n", str);
                char *enc = strtok(str, "/");

                if ((enc = strtok(NULL, "/")) ) {
                    char name[1024];

                    bzero(fpath, sizeof(fpath));
                    strcpy(fpath, path);
                    bzero(name, sizeof(name));
                    strcpy(name, strstr(fpath, enc));
                    atbash(name);
                    rot13(name);

                    bzero(mid_path, sizeof(mid_path));
                    strncpy(mid_path, fpath, strlen(fpath) - strlen(name));
                    bzero(fpath, sizeof(fpath));

                    sprintf(fpath, "%s%s%s", dirpath, mid_path, name);
                    printf("11.1 file path RX: %s\n", fpath);
                }
                else sprintf(fpath, "%s%s", dirpath, path);
                printf("11.2 file path RX: %s\n", fpath);
            }
        }
        else sprintf(fpath, "%s%s", dirpath, path);
        printf("A4 file path: %s\n", fpath);
    }

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

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
    char fpath[1024];
    get_new_directory(path, fpath);

    int res;
    if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
    else res = mknod(fpath, mode, rdev);
    if (res == -1) return -errno;
    return 0;

    char desc[1024];
    strcpy(desc, fpath);
    write_info("MKNOD", fpath);
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
    char fpath[2000];
    bzero(fpath, sizeof(fpath));
    get_new_directory(path, fpath);

    int res = mkdir(fpath, mode);
    if (res == -1) return -errno;
    else {
        if(strstr(path, "AtoZ_")) {
            FILE *log = fopen("/home/farhan/fuse.log", "a");
            fprintf(log, "MKDIR: %s%s\n", dirpath, path);
            fclose(log);
        }
        write_info("MKDIR", fpath);
    }

    return 0;
}

static int xmp_create(const char *path, mode_t mode,
                      struct fuse_file_info *fi) {
    char fpath[2000];
    get_new_directory(path, fpath);

    int res = open(fpath, fi->flags, mode);
    if (res == -1) return -errno;
    else write_info("CREAT", fpath); 
    fi->fh = res;
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    char fpath[2000];
    get_original_directory(path, fpath);

    int res = open(fpath, fi->flags);

    if (res == -1) return -errno;
    else write_info("OPEN", fpath);

    fi->fh = res;
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[2000], name[2000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (strcmp(path, "/") == 0) sprintf(fpath, "%s", dirpath);
    else get_original_directory(path, fpath);

    int res = 0;
    int fd = 0;

    (void)fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    else write_info("READ", fpath);
    close(fd);
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi) {
    char fpath[2000];
    get_new_directory(path, fpath);

    int fd;
    int res;

    (void)fi;
    if (!fi) fd = open(fpath, O_WRONLY);
    else fd = fi->fh;

    if (fd == -1) return -errno;

    res = pwrite(fd, buf, size, offset);

    if (res == -1) res = -errno;
    else write_info("WRITE", fpath);

    if (!fi) close(fd);
    return res;
}

static struct fuse_operations xmp_oper = {
    .getattr    = xmp_getattr,
    .access     = xmp_access,
    .readdir    = xmp_readdir,
    .mknod      = xmp_mknod,
    .mkdir      = xmp_mkdir,
    .unlink     = xmp_unlink,
    .rmdir      = xmp_rmdir,
    .open		= xmp_open,
    .rename     = xmp_rename,
    .create     = xmp_create,
    .read       = xmp_read,
    .write      = xmp_write,
};

int main(int argc, char *argv[]) {
    open_log();
    umask(0);
    renamed_count = 0;
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
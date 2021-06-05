#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdbool.h>

char dirpath[100] = "/home/farhan/Downloads", ext[100000] = "\0";
int id = 0;

void write_info(char *text, char* path){
    char* level = "INFO";
	char curr_time[30];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
//	strftime(curr_time, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%02d%02d%04d-%02d:%02d:%02d:%s::%s", level, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec, text, path);
	FILE *out = fopen("/home/farhan/SinSeiFS.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
    
}

void write_warning(char *text, char* path){ //rmdir dan unlink
    char* level = "WARNING";
    char curr_time[30];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
//	strftime(curr_time, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%02d%02d%04d-%02d:%02d:%02d:%s::%s", level, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec, text, path);
	FILE *out = fopen("/home/farhan/SinSeiFS.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
}

void open_log() {
    if(access("fuse.log", F_OK)) {
		FILE *fp = fopen("fuse.log", "w+");
		fclose(fp);
	} 
}

void substring(char *s, char *sub, int start, int length) {
   int i = 0;
   while (i < length){
      sub[i] = s[start + i];
      i++;
   }
   sub[i] = '\0';
}

char *atbash(char* str, bool cek) {
	int i, j, k = 0;
	char *ext = strrchr(str, '.');
	if(cek && ext != NULL) k = strlen(ext);
	for(i=0; i<strlen(str)-k; i++) {
		if(str[i] >= 'A' && str[i] <= 'Z') str[i] = 'Z' + 'A' - str[i];
		if(str[i] >= 'a' && str[i] <= 'z') str[i] = 'z' + 'a' - str[i];
	}
	return str;
}

char *get_path_end(char *str) {
	if(!strcmp(str, "/")) return NULL;
	return strrchr(str, '/') + 1;
}

char *get_path(char *str) {
	bool encr;
	int start, id;
	encr = 0; start = 1; 
	id = strchr(str + start, '/') - str - 1;
	char curpos[1024];
	while(id < strlen(str)) {
		strcpy(curpos, "");
		strncpy(curpos, str + start, id - start + 1);
		curpos[id - start + 1] = '\0';
		if(encr) {
			atbash(curpos, 0);
			strncpy(str + start, curpos, id - start + 1);
		}
		if(!encr && strstr(str + start, "AtoZ_") == str + start) encr = 1;
		start = id + 2;
		id = strchr(str + start, '/') - str - 1;
	}
	id = strlen(str); id--;
	strncpy(curpos, str + start, id - start + 1);
	curpos[id - start + 1] = '\0';
	if(encr) {
		atbash(curpos, 1);
		strncpy(str + start, curpos, id - start + 1);
	}
	return str;
}

char *set_path(char *fin, char *str1, const char *str2) {
	strcpy(fin, str1);
	if(!strcmp(str2, "/")) return fin;
	if(str2[0] != '/') {
		fin[strlen(fin) + 1] = '\0';
		fin[strlen(fin)] = '/';
	}
	sprintf(fin, "%s%s", fin, str2);
	return fin;
}

int encrypt_folder(char *str) {
	int ans=0;
	char *fi = strtok(str, "/");
	while(fi) {
		char sub[1024];
		substring(fi, sub, 0, 5);
		if(!strcmp(sub, "AtoZ_")) ans=1;
		fi = strtok(NULL, "/");
	}
	return ans;
}

void recursive_atbash(char *str, int flag) {
	struct dirent *dp;
	DIR *dir = opendir(str);
	
	if(!dir) return;
	
	while((dp = readdir(dir)) != NULL) {
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
        	char path[2000000], name[1000000], newname[1000000];
        	set_path(path, str, dp->d_name);
			strcpy(name, dp->d_name);
			set_path(newname, str, atbash(name, 1));
			if(dp->d_type == DT_REG) rename(path, newname);
			else if(dp->d_type == DT_DIR) {
				rename(path, newname);
				recursive_atbash(newname, flag);
			}
        }
	}
}

void encrypt_atbash(char *str, int flag) {
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	recursive_atbash(str, flag);
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
	int res;
	char fpath[1000];
	set_path(fpath, dirpath, path);
	res = lstat(get_path(fpath), stbuf);
	write_info("LS", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask) {
	int res;
	char fpath[1000];
	set_path(fpath, dirpath, path);
	res = access(get_path(fpath), mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size) {
	int res;
	char fpath[1000];
	set_path(fpath, dirpath, path);
	res = readlink(get_path(fpath), buf, size - 1);
	if (res == -1) return -errno;
	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res = 0;
	
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(get_path(fpath));
	if (dp == NULL) return -errno;
	
	int flag = encrypt_folder(fpath);
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;		
		st.st_mode = de->d_type << 12;
		char nama[1000000];
		strcpy(nama, de->d_name);
		if(flag == 1) {
			if(de->d_type == DT_REG) atbash(nama, 1);
			else if(de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) atbash(nama, 0);
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		} else {
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
	}
	closedir(dp);
    write_info("CD", fpath);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	get_path(fpath);
	int res;
	
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	} else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	if (res == -1) return -errno;
	
    write_info("CREATE", fpath);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	
	int res;

	res = mkdir(get_path(fpath), mode);
	if (res == -1) return -errno;
	
    char cek_substr[1024];
    if(get_path_end(fpath) == 0) return 0;
    char filePath[1000000];
    strcpy(filePath, get_path_end(fpath));
    substring(filePath, cek_substr, 0, 5);
	if(strcmp(cek_substr, "AtoZ_") == 0) encrypt_atbash(fpath, 1);

	FILE *log = fopen("fuse.log", "a");
	fprintf(log, "MKDIR: %s\n", filePath);
	fclose(log);
	write_info("MKDIR", fpath);
	return 0;
}

static int xmp_unlink(const char *path) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = unlink(get_path(fpath));
    write_warning("UNLINK", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = rmdir(get_path(fpath));
    write_warning("RMDIR", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_symlink(const char *from, const char *to) {
	int res;

	res = symlink(from, to);
	if (res == -1) return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to) {    
    char source[1000];
	set_path(source, dirpath, from);
	
    char dest[1000];
	set_path(dest, dirpath, to);

	FILE *logFile = fopen("fuse.log", "a");
	fprintf(logFile, "RENAME: %s -> %s\n", get_path(source), get_path(dest));
	fclose(logFile);
	write_info("RENAME", get_path(source));
	
	int res = rename(get_path(source), get_path(dest));
	
	if (res == -1) return -errno;
	else{
		FILE *logFile = fopen("fuse.log", "a");
		fprintf(logFile, "RENAME: %s -> %s\n", from, to);
		fclose(logFile);
		write_info("RENAME", source);
	}

	char from_atoz[1024], to_atoz[1024];

    if(get_path_end(source) == 0) return 0;
    char filePath[1000000];
    strcpy(filePath, get_path_end(source));
    substring(filePath, from_atoz, 0, 5);

    if(get_path_end(dest) == 0) return 0;
    strcpy(filePath, get_path_end(dest));
    substring(filePath, from_atoz, 0, 5);
	
	if(strcmp(from_atoz, "AtoZ_") && !strcmp(to_atoz, "AtoZ_")) encrypt_atbash(dest, 1);
	else if(!strcmp(from_atoz, "AtoZ_") && strcmp(to_atoz, "AtoZ_")) encrypt_atbash(dest, -1);

	return 0;
}

static int xmp_link(const char *from, const char *to) {
	int res;

	res = link(from, to);
	if (res == -1) return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = chmod(get_path(fpath), mode);
	write_info("CHMOD", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = lchown(get_path(fpath), uid, gid);
	write_info("CHOWN", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_truncate(const char *path, off_t size) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = truncate(get_path(fpath), size);
    write_info("TRUNCATE", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2]) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(get_path(fpath), tv);
    write_info("UTIMENS", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = open(get_path(fpath), fi->flags);
    write_info("OPEN", fpath);
	if (res == -1) return -errno;
	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int fd = 0;
	int res = 0;

	(void) fi;
	fd = open(get_path(fpath), O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;
	close(fd);
	
    write_info("READ", fpath);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int fd;
	int res;

	(void) fi;
	fd = open(get_path(fpath), O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

    write_info("WRITE", fpath);
	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf) {
	char fpath[1000];
	set_path(fpath, dirpath, path);
	int res;

	res = statvfs(get_path(fpath), stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)  {
	char fpath[1000];
	set_path(fpath, dirpath, path);
    (void) fi;

    int res;
    res = creat(get_path(fpath), mode);
    if(res == -1) return -errno;
	
    write_info("CREAT", fpath);
    close(res);
    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi) {
	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi) {
	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};

int main(int argc, char *argv[]) {
	open_log();
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <sys/wait.h>

static const char *trash_path = "/home/nabil/Documents/cek/trash"; //path to trash dir
static const char *dirpath = "/home/nabil/Documents/cek"; // fuse implementation directory
static const char *log_path = "/home/nabil/Documents/cek/trash.log"; //to record the process in the log

void appendLog(const char *command, const char *path, const char *add) {
    FILE *log = fopen(log_path, "a");

    time_t now;
    struct tm *tm_info;
    char timestamp[20];

    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y-%H:%M:%S", tm_info);

    if (log != NULL) {
        if (strcmp(command, "MV") == 0) {
            fprintf(log, "%s MOVED %s TO TRASH.\n", timestamp, path);
        } else if (strcmp(command, "RM") == 0) {
            fprintf(log, "%s PERMANENTLY REMOVED %s.\n", timestamp, path);
        } else if (strcmp(command, "RMDIR") == 0) {
            fprintf(log, "%s PERMANENTLY REMOVED %s.\n", timestamp, path);
        } else if (strcmp(command, "RMF") == 0) {
            fprintf(log, "%s FAILED TO REMOVE %s.\n", timestamp, path);
        } else if (strcmp(command, "MVF") == 0) {
            fprintf(log, "%s FAILED TO MOVE %s.\n", timestamp, path);
        } else if (strcmp(command, "RNF") == 0) {
            fprintf(log, "%s FAILED TO RENAME %s.\n", timestamp, path);
        } else if (strcmp(command, "MKDIRF") == 0) {
            fprintf(log, "%s FAILED TO CREATE DIRECTORY %s.\n", timestamp, path);
        } else if (strcmp(command, "CHOWNF") == 0) {
            fprintf(log, "%s FAILED TO CHANGE OWNER %s.\n", timestamp, path);
        } else if (strcmp(command, "CHMODF") == 0) {
            fprintf(log, "%s FAILED TO CHANGE PERMISSION %s.\n", timestamp, path);
        } else if (strcmp(command, "MKDIR") == 0) {
            fprintf(log, "%s CREATED DIRECTORY %s.\n", timestamp, path);
        } else if (strcmp(command, "CHOWN") == 0) {
            fprintf(log, "%s CHANGED OWNER %s.\n", timestamp, path);
        } else if (strcmp(command, "CHMOD") == 0) {
            fprintf(log, "%s CHANGED PERMISSION %s.\n", timestamp, path);
        } else if (strcmp(command, "RN") == 0) {
            fprintf(log, "%s NAME CHANGED TO %s.\n", timestamp, path);
        } else if (strcmp(command, "RS") == 0) {
            fprintf(log, "%s RESTORED %s FROM TRASH TO %s.\n", timestamp, path, add);
        }
    }
    fclose(log);
}

static int isSubdir(const char *parent, const char *child) {
    size_t parent_len = strlen(parent);
    if (strncmp(parent, child, parent_len) != 0) {
        return 0; 
    }
    if (child[parent_len] == '\0') {
        return 1; 
    }
    if (child[parent_len] == '/') {
        return 1; 
    }
    return 0;
}

static int checkRestricted(const char *path) {
    if (isSubdir(trash_path, path)) {
        return 1; // Access denied
    }
    return 0; //access accepted
}

//declaration of some helper functions
int change_permissions(const char *path, mode_t mode); //change permissions using sudo 
void manageOriginalPath(const char *trash_fpath, char *original_path, const char *src_fpath, int *mode, int read);
int my_move(const char *from, const char *to);
static int move_to_trash(const char *path);

//functionn to get attributes
static int do_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];

    sprintf(fpath, "%s%s", dirpath, path);
    printf("do_getattr: Path = %s\n", fpath);

    res = lstat(fpath, stbuf);

    if (res == -1) {
        printf("do_getattr: Error %d\n", errno);
        return -errno;
    }

    return 0;
}

//functionn to get current position in the directory
static int do_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];

    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    printf("do_readdir: Path = %s\n", fpath);

    DIR *dir;
    struct dirent *ent;
    (void)offset;
    (void)fi;
    int res = 0;

    dir = opendir(fpath);
    if (dir == NULL) {
        printf("do_readdir: Error opening directory %s\n", fpath);
        return -errno;
    }

    while ((ent = readdir(dir)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = ent->d_ino;
        st.st_mode = ent->d_type << 12;

        res = (filler(buf, ent->d_name, &st, 0));
        if (res != 0) {
            printf("do_readdir: Error adding entry for %s\n", ent->d_name);
            break;
        }
    }

    closedir(dir);

    return 0;
}

//function to read data from file
static int do_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    printf("do_read: Path = %s, Size = %zu, Offset = %ld\n", fpath, size, offset);

    int res = 0;
    int fd = 0;
    (void)fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) {
        printf("do_read: Error opening file %s\n", fpath);
        return -errno;
    }

    res = pread(fd, buf, size, offset);

    if (res == -1) {
        printf("do_read: Error reading file %s\n", fpath);
        res = -errno;
    }

    close(fd);

    return res;
}

//function for remove file
static int do_unlink(const char *path) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    printf("do_read: Path = %s\n", fpath);
    if (checkRestricted(fpath)) {
        int res = unlink(fpath);
        if (res == -1) {
            printf("do_unlink: Error %d\n", errno);
            appendLog("RMF", fpath, "");
            return -errno;
        }
        appendLog("RM", fpath, "");
        return 0;
    } else {
        return move_to_trash(path);
    }
}

//function for remove directory
static int do_rmdir(const char *path) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    printf("do_rmdir: Path = %s\n", fpath);

    if (strcmp(path, "/") == 0) {
        printf("do_rmdir: Cannot remove root directory\n");
        return -EINVAL;
    }

    if (strcmp(fpath, trash_path) == 0) {
        printf("do_rmdir: Cannot delete the 'trash' directory\n");
        appendLog("RMDIRF", fpath, "");
        return -EACCES;
    }

    if (isSubdir(trash_path, fpath)) {
        int res = rmdir(fpath);
        if (res == -1) {
            printf("do_rmdir: Error %d\n", errno);
            appendLog("RMDIRF", fpath, "");
            return -errno;
        }
        appendLog("RMDIR", fpath, "");
        return 0;
    }

    return move_to_trash(path);
}

//function for rename or remove file and directory
static int do_rename(const char *from, const char *to){
    char old_fpath[1000], new_fpath[1000];
    sprintf(old_fpath, "%s%s", dirpath, from);
    sprintf(new_fpath, "%s%s", dirpath, to);

    if (checkRestricted(old_fpath) || checkRestricted(new_fpath)) {
        return my_move(from, to);
    }

    int res = rename(old_fpath, new_fpath);
    if (res == -1) {
        appendLog("RNF", from, "");
        return -errno;
    }
    appendLog("RN", to, "");
    return 0;
}

//fuction for make directory
static int do_mkdir(const char *path, mode_t mode) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    printf("do_mkdir: Path = %s\n", fpath);
    
    if (checkRestricted(fpath)) {
        printf("do_mkdir: Access denied for %s\n", fpath);
        appendLog("MKDIRF", fpath, "");
        return -EACCES; 
    }

    if (strcmp(path, "/trash") == 0 || strcmp(path, "/restore") == 0) {
        printf("do_mkdir: Cannot create directory with name 'trash' or 'restore'\n");
        appendLog("MKDIRF", fpath, "");
        return -EACCES;
    }

    int res = mkdir(fpath, mode);
    if (res == -1){
        printf("do_mkdir: Error %d\n", errno);
        appendLog("MKDIRF", fpath, "");
        return -errno;
    }

    appendLog("MKDIR", fpath, "");
    return 0;
}

//function to change owner
static int do_chown(const char *path, uid_t uid, gid_t gid) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    printf("do_chown: Path = %s\n", fpath);
    if (checkRestricted(fpath)) {
        printf("do_chown: Access denied for %s\n", fpath);
        appendLog("CHOWNF", fpath, "");
        return -EACCES; 
    }

    int res = lchown(fpath, uid, gid);
    if (res == -1) {
        printf("do_chown: Error %d\n", errno);
        appendLog("CHOWNF", fpath, "");
        return -errno;
    }

    appendLog("CHOWN", fpath, "");
    return 0;
}

//function to change permissions
static int do_chmod(const char *path, mode_t mode) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    printf("do_chmod: Path = %s\n", fpath);
    if (checkRestricted(fpath)) {
        printf("do_chmod: Access denied for %s\n", fpath);
        appendLog("CHMODF", fpath, "");
        return -EACCES;
    }

    int res = chmod(fpath, mode);
    if (res == -1) {
        printf("do_chmod: Error %d\n", errno);
        appendLog("CHMODF", fpath, "");
        return -errno;
    }

    appendLog("CHMOD", fpath, "");
    return 0;
}

//change permissions using sudo 
int change_permissions(const char *path, mode_t mode) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        char mode_str[10];
        snprintf(mode_str, sizeof(mode_str), "%o", mode);
        execlp("sudo", "sudo", "chmod", mode_str, path, (char *)NULL);

        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return 0;
        } else {
            fprintf(stderr, "Failed to change permissions for %s\n", path);
            return -1;
        }
    }
}

int my_move(const char *from, const char *to) {
    char from_fpath[1000];
    char *from_basename = basename((char *)from);
    if (from_basename == NULL) {
        printf("Error getting basename for %s\n", from);
        return -1;
    }

    snprintf(from_fpath, sizeof(from_fpath), "%s/%s", trash_path, from_basename);
    printf("Memindahkan: %s -> %s\n", from_fpath, to);
   
    if (change_permissions(from_fpath, 0700) == -1) {
        printf("chmod trash error my_move function:\n");
        return -errno;
    }

    struct stat st;
    if (stat(from_fpath, &st) == -1) {
        return -errno;
    }

    char cek[1000];
    strcpy(cek, to);
    printf("to my_move function: %s\n", cek);
    if (strcmp(basename(cek), "restore") == 0) {
        char original_path[1000];
        int original_mode;
        manageOriginalPath(from_fpath, original_path, NULL, &original_mode, 1);

        if (chmod(from_fpath, original_mode) == -1) {
            return -errno;
        }

        int res = rename(from_fpath, original_path);
        appendLog("RS", from_fpath, original_path);
        if (res == -1) {
            return -errno;
        }

        res = rmdir(from_fpath);
        return 0;
    }

    int res = rename(from_fpath, to);
    if (res == -1) {
        printf("Failed to move file %s to %s: %s\n", from_fpath, to, strerror(errno));
        return -errno;
    }
    appendLog("RS", from_fpath, to);

    return 0;
}

void manageOriginalPath(const char *trash_fpath, char *original_path, const char *src_fpath, int *mode, int read) {
    if(read == 0){
        FILE *tes = fopen("/home/nabil/Documents/cek/catat.log", "a");
        if (tes != NULL) {
            struct stat st;
            int ret = lstat(src_fpath, &st);
            if (ret == 0) {
                fprintf(tes, "%s_%s_%o\n", trash_fpath, src_fpath, st.st_mode);
                fclose(tes);
            } else {
                perror("lstat");
                fclose(tes);
                return;
            }
        } else {
            perror("Failed to open catat.log for writing");
        }
    } else {
        FILE *tes = fopen("/home/nabil/Documents/cek/catat.log", "r");
        if (tes != NULL) {
            char line[2000];
            while (fgets(line, sizeof(line), tes)) {
                char temp_trash_fpath[1000], temp_src_fpath[1000];
                int temp_mode;
                sscanf(line, "%[^_]_%[^_]_%o", temp_trash_fpath, temp_src_fpath, &temp_mode);
                if (strcmp(temp_trash_fpath, trash_fpath) == 0) {
                    strcpy(original_path, temp_src_fpath);
                    *mode = temp_mode;
                    printf("manageOriginalPath: Read from story.txt: %s, %o\n", original_path, *mode); // Tambahkan printf untuk debug
                    break;
                }
            }
            fclose(tes);
        }
    }
}

static int move_to_trash(const char *path) {
    char src_fpath[1000], trash_fpath[1000];
    struct stat st;
    sprintf(src_fpath, "%s%s", dirpath, path);

    const char *basename = strrchr(src_fpath, '/');
    if (!basename) {
        printf("move_to_trash: Invalid path %s\n", src_fpath);
        appendLog("MVF", src_fpath, "");
        return -EINVAL;
    }

    sprintf(trash_fpath, "%s%s", trash_path, basename);

    if (checkRestricted(src_fpath)) {
        appendLog("MVF", src_fpath, "");
        return -EACCES;
    }

    if (lstat(src_fpath, &st) == -1) {
        appendLog("MVF", src_fpath, "");
        return -errno;
    }
    
    char dummy_path[1000]; 
    int dummy_mode = 0;    
    manageOriginalPath(trash_fpath, dummy_path, src_fpath, &dummy_mode, 0);

    int res = rename(src_fpath, trash_fpath);
    if (res == -1) {
        appendLog("MVF", src_fpath, "");
        return -errno;
    }

    res = chmod(trash_fpath, 000);
    if (res == -1) {
        return -errno;
    }

    appendLog("MV", src_fpath, "");
    return 0;
}

static int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    int fd = open(fpath, O_WRONLY);
    if (fd == -1) {
        return -errno;
    }

    int res = pwrite(fd, buf, size, offset);
    if (res == -1) {
        res = -errno;
    }

    close(fd);
    return res;
}

static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    int fd = creat(fpath, mode);
    if (fd == -1) {
        return -errno;
    }

    fi->fh = fd;

    return 0;
}

static struct fuse_operations trash_operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .read = do_read,
    .unlink = do_unlink,
    .rmdir = do_rmdir,
    .rename = do_rename,
    .mkdir = do_mkdir,
    .chown = do_chown,
    .chmod = do_chmod,
    .write = do_write,
    .create = do_create,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &trash_operations, NULL);
}

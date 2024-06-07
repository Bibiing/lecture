### Daftar Soal _(Task List)_
- [Task 1 - TrashOps](/task-1/)

## Task 1

### Main Function
```
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &trash_operations, NULL);
}
```

#### Penjelasan
- `umask(0)` singkatan dari "user file creation mode mask". Mengatur umask ke 0 untuk memastikan izin file yang diciptakan oleh program FUSE tidak dibatasi oleh umask dari proses yang menjalankannya.
- `fuse_main` adalah fungsi utama dari library FUSE yang akan menjalankan file system.
- `argc, argv` untuk menangani argumen command-line yang diperlukan FUSE.
- `&trash_operations` pointer ke struct `fuse_operations` yang mendifinisikan fungsi-fungsi operrasi file system yang akan diimplementasikan.
- `NULL` tidak ada data khusus yang diteruskan ke FUSE.

### Struct fuse_operations
```
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
};
```

#### Penjelasan
- `struct fuse_operations` adalah structur dari library FUSE untuk menyimpan pointer ke fungsi-fungsi callback yang akan mengenai berbagai operasi file system.
- `.getattr = do_getattr` Fungsi do_getattr digunakan untuk mendapatkan atribut file atau direktori (seperti izin, ukuran, waktu modifikasi, dll.).
- `.readdir = do_readdir` Fungsi do_readdir digunakan untuk membaca isi dari sebuah direktori.
- `.read = do_read` Fungsi do_read digunakan untuk membaca isi dari sebuah file.
- `.unlink = do_unlink` Fungsi do_unlink digunakan untuk menghapus sebuah file. Jika file yang di hapus berada diluar direktori trash, pindahkan file tersebut ke direktori trash dengan izin 000. jika file yang ingin dihapus berada di direktori trash maka hapus permanen file tersebut.
- `.rmdir = do_rmdir` Fungsi do_rmdir digunakan untuk menghapus sebuah direktori. Jika direktori yang di hapus berada diluar direktori trash, pindahkan direktori tersebut ke direktori trash dengan izin 000. jika direktori yang ingin dihapus berada di direktori trash maka hapus permanen direktori tersebut. Tidak dapat menghapus direktori trash.
- `.rename = do_rename` Fungsi do_rename digunakan untuk mengganti nama atau memindahkan file atau direktori. file dan direktori di trash tidak dapat diubah namanya. untuk mengembalikan file dari trash directori ada dua cara, menggunakan argumen restore atau menggunakan full path.
- `.mkdir = do_mkdir` Fungsi do_mkdir digunakan untuk membuat sebuah direktori baru. Tidak dapat membuat direktori dengan nama trash dan restore.
- `.chown = do_chown` Fungsi do_chown digunakan untuk mengubah kepemilikan dari sebuah file atau direktori, tidak berlaku pada fild dan direktori yg berada pada direktori trash.
- `.chmod = do_chmod` Fungsi do_chmod digunakan untuk mengubah izin (permissions) dari sebuah file atau direktori, tidak berlaku pada fild dan direktori yg berada pada direktori trash.

### Path Declaration
```
static const char *trash_path = "/home/nabil/Documents/cek/trash"
static const char *dirpath = "/home/nabil/Documents/cek"
static const char *log_path = "/home/nabil/Documents/cek/trash.log"
```
#### Penjelasan
- `trash_path` berisi path menuju direktori trash.
- `dirpath` Direktori tempat utama FUSE file system akan beroperasi.
- `log_path` berisi path menuju menuju file trash.log.


### *Helper Function*
```
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
```

#### Penjelasan
- `size_t parent_len = strlen(parent)` menyimpan panjang string parent
- Bandingkan `parent` dan `child` sepanjang `parent_len` karakter. jika tidak sama maka `child` bukan subdirektori dari `parent`.
- Memeriksa apakah karakter setelah `parent_len` di `child` adalah null character jikya iya, maka `child` subdirektori dari `parent`.
- Memeriksa apakah karakter setelah `parent_len` di `child` adalah garis miring character jikya iya, maka `child` subdirektori dari `parent`.
- selain itu `child` bukan subdirektori dari `parent`. 


```
static int checkRestricted(const char *path) {
    if (isSubdir(trash_path, path)) {
        return 1; // Jika path adalah subdirektori dari trash_path, return 1 (akses ditolak)
    }
    return 0; // Jika tidak, return 0 (akses diizinkan)
}
```

#### Penjelasan
- memannggil fungsi isSubdir dengan parameter `trash_path` dan `path` yang akan di bandingkan. jika `path` adalah subdirektori dari `trash_path` maka akses ditolak.


```
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
```

#### Penjelasan
- Jika `read` adalah 0, maka akan membuka file dalam mode append.
- `lstat(src_fpath, &st)` mendapatkan indormasi status dari `src_fpath`.
- `fprintf(tes, "%s_%s_%o\n", trash_fpath, src_fpath, st.st_mode)` mencatat ke `catat.log` dengan pemisah '_' antar informasi. Informasi yang disimpan adalah path file atau direktori yang berada di trash, path sebelum masuk trash directory, dan izin dari file atau direktori.
- Jika `read` adalah 1, maka akan membuka file dalam mode baca.
- `fgets(line, sizeof(line), tes)` membaca setiap baris dari file `catat.log`.
- `sscanf(line, "%[^_]_%[^_]_%o", temp_trash_fpath, temp_src_fpath, &temp_mode)` memisahkan informasi menggunakan `sscanf` dan menyimpan informasinya.
- Bandingkan trash path yang dibaca dari `catat.log` dengan `trash_fpath`. Jika cocok, maka menyalin `temp_src_fpath` ke `original_path` dan `temp_mode` ke `mode`.


```
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
        return -EACCES; // Access denied
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
```

#### Penjelasan
- `sprintf(src_fpath, "%s%s", dirpath, path)` mendapatkan fullpath dengan menggabungkan `dirpath` dan `path`.
- `basename = strrchr(src_fpath, '/')` menggunakan `strrchr` untuk menemukan '/' terakhir dalam `src_fpath` dan mendapatkan nama file.
- jika `basename` tidak ditemukan, berarti path tidak valid.
- `sprintf(trash_fpath, "%s%s", trash_path, basename)` membentuk trash path.
- Cek apakah `src_fpath` berada dalam trash, jika fungsi mengembalikan nilai selain '0' maka akan mencatat log dan return `-EACCES`.
- `lstat(src_fpath, &st)` mendapatkan informasi status file.
- `manageOriginalPath(trash_fpath, dummy_path, src_fpath, &dummy_mode, 0)` mencatat dengan memanggil fungsi `manageOriginalPath`.
- `rename(src_fpath, trash_fpath)` pindahkan file ke dalam direktori trash menggunakan `renamne`.
- `chmod(trash_fpath, 000)` mengubah izinnya menjadi '000'.
- `appendLog("MV", src_fpath, "")` catat log.


```
int my_move(const char *from, const char *to) {
    printf("Memindahkan: %s -> %s\n", from, to);
   
    if (chmod(from, 0700) == -1) {
        return -errno;
    }

    if (chmod(to, 0700) == -1) {
        return -errno;
    }
    
    struct stat st;
    if (stat(from, &st) == -1) {
        return -errno;
    }

    char cek[1000];
    strcpy(cek, to);
    if (strcmp(basename(cek), "restore") == 0) {
        char original_path[1000];
        int original_mode;
        manageOriginalPath(from, original_path, NULL, &original_mode, 1);

        if (chmod(from, original_mode) == -1) {
            return -errno;
        }

        int res = rename(from, original_path);
        appendLog("Restore", original_path, "");
        if (res == -1) {
            return -errno;
        }

        res = rmdir(from);
        return 0;
    }

    if (S_ISDIR(st.st_mode)) {
        if (mkdir(to, st.st_mode) == -1 && errno != EEXIST) {
            return -errno;
        }

        DIR *dir = opendir(from);
        if (!dir) {
            return -errno;
        }

        struct dirent *entry;
        int res = 0;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char from_path[1000];
            char to_path[1000];
            snprintf(from_path, sizeof(from_path), "%s/%s", from, entry->d_name);
            snprintf(to_path, sizeof(to_path), "%s/%s", to, entry->d_name);

            res = my_move(from_path, to_path);
            if (res != 0) {
                break;
            }
        }

        closedir(dir);

        if (res == 0) {
            if (rmdir(from) == -1) {
                return -errno;
            }
        }

        return res;
    } else {
        char from_basename[1000];
        char to_basename[1000];
        strncpy(from_basename, basename((char *)from), 1000);
        strncpy(to_basename, basename((char *)to), 1000);

        if (strcmp(from_basename, to_basename) != 0) {
            appendLog("RNF", from, ""); 
            return -EPERM; 
        }

        int res = rename(from, to);
        appendLog("RN", to, "");
        if (res == -1) {
            appendLog("RNF", from, "");
            return -errno;
        }

        return 0;
    }
}

```

#### Penjelasan
- Fungsi ini dipanggil untuk melakukan restore.
-`chmod(from, 0700)` ubah izin dari `from` dan `to` untuk mengakses file.
-`stat(from, &st)` mendapatkan informasi status file.
-`strcmp(basename(cek), "restore")` bandingkan basename dari 'to' dengan 'restore'. jika true maka akan melakukan restore.
-`manageOriginalPath(from, original_path, NULL, &original_mode, 1)` memanggil fungsi `manageOriginalPath` untuk mendapatkan `original_path` dan `original_mode`.
-`chmod(from, original_mode)` ubah izin seperti semula.
-`rename(from, original_path)` pindahkan ke path semula.
-`rmdir(from)` jika berhasil hapus file yang ditrash.
-`mkdir(to, st.st_mode)` membuat direktori tujuan jika belum ada.
- Setelah itu hanya melakukan move seperti biasa.


```
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
```

#### Penjelasan
- `pid_t pid = fork()` membuat child process.
- `snprintf(mode_str, sizeof(mode_str), "%o", mode)` menulis representasi oktal dari mode.
- `execlp("sudo", "sudo", "chmod", mode_str, path, (char *)NULL)` jalankan perintah `sudo chmod mode_str path`.
- `waitpid(pid, &status, 0)` menunggu child process selesai.

---
### appendLog Function
```
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
```

#### Penjelasan
- `fopen(log_path, "a")` membuka `log_path` dengan mode append.
- `time_t now` digunakan untuk menyimpan waktu saat ini dalam bentuk objek `time_t`.
- `struct tm *tm_info` digunakan untuk menampung informasi waktu yang sudah diuraikan menjadi struktur `tm`(time structure) oleh fungsi `localtime`.
- `char timestamp[20]` untuk menyimpan format waktu.
- `time(&now)` mendapatkan waktu saat ini.
- `tm_info = localtime(&now)` mengonversi menjadi waktu lokal berdasarkan zona waktu yang berlaku.
- `strftime(timestamp, sizeof(timestamp), "%d/%m/%Y-%H:%M:%S", tm_info);` formating waktu.
- Sisanya untuk mencatata log berdasarkan kondisi-kondisinya.

#### Output
![logging](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_log.png)


### getattr function
```
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
```

#### Penjelasan
- fungsi untuk mendapatkan atribut (metadata) dari file atau direktori.
- `sprintf(fpath, "%s%s", dirpath, path)` mencatat fullpath.
- `lstat(fpath, stbuf)` digunakan untuk mendapatkan atribut dari file atau dirtektori dari `fpath`. Hasilnya akan disimpan dalam `stbuf`.

#### Output
![attr](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_atribut.png)

### readdir function
```
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
```

#### Penjelasan
- fungsi untuk membaca isi dari sebuah direktori dan mengisi buffer dengan entri-entri direktori tersebut. 
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
-`dir = opendir(fpath)` 
-`DIR *dir; struct dirent *ent;` variabel untuk menyimpan direktori dan entri entri di dalamnya.
-`dir = opendir(fpath);` membuka direktori untuk di baca
-`(ent = readdir(dir)` membaca entri-entri direktori dan mengisi buffer dengan informasi setiap entri.
- `memset(&st, 0, sizeof(st))` menginisialisasi struktur stat untuk setiap entri.
- `st.st_ino = ent->d_ino;  st.st_mode = ent->d_type << 12;` mengisi informasi inode dan mode dari entri direktori.
- `(filler(buf, ent->d_name, &st, 0))` menambahkan entri ke dalam buffer menggunakan filler function.
- `closedir(dir)` tutup direktori setelah selesai membaca.

#### Output
![readdir](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_readdir.png)

### read function
```
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
```

#### Penjelasan
- Fungsi untuk membaca dengan menggunakan command `cat`.
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
-`open(fpath, O_RDONLY)` buka dengan mode baca saja.
-`pread(fd, buf, size, offset)` membaca data dari file yang sudah dibuka menggunakan `pread`. `buf` adalah buffer tempat data dibaca disimpan, `size` adalah ukuran data yang akan dibaca, dan `offset` adalah posisi dalam file dari mana pembacaan dimulai.

#### Output
![read](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_read.png)

### unlink function
```
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
```

#### Penjelasan
- Fungsi untuk remove file / unlink
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
- Cek apakah fpath berada dalam trash directory dengan memanggil fungsi `checkRestricted`.
- `unlink(fpath)` jika berada dalam direktori trash, maka file akan dihapus permanen.
- Jika file berada diluar panggil fungsi `move_to_trash` untuk memindahkan path ke dalam trash.

#### Output
![unlink](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_unlink.png)

### rmdir function
```
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
```

#### Penjelasan
- Fungsi untuk menghapus direktori
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
- Cek apakah yang akan hapus adalah direktori home, jika true return `-EINVAL` menandakan akses ditolak. 
- Cek apakah akan mencoba menghapus direktori dengan nama `trash` jika true return `-EINVAL` menandakan akses ditolak.
- cek menggunakan fungsi `isSubdir(trash_path, fpath)` apakah `fpath` merupakan subdirektori dari `trash_path`. jika true akan direktori akan di hapus permanen `rmdir(fpath)`.
- jika `fpath` bukan subdirektori dari `trash_path` maka move `fpath` ke trash.

#### Output
![rmdir](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_rmdir.png)

### rename function
```
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
    appendLog("RN", to, "");  // Log berhasil move
    return 0;
}
```

#### Penjelasan
- Fungsi ini dapat mengganti nama dan move file atau direktori.
-`sprintf(old_fpath, "%s%s", dirpath, path)` menyiapkan fullpath asal.
- `sprintf(new_fpath, "%s%s", dirpath, to)` menyiapkan fullpath tujuan.
- Cek apakah salah satu dari path berada di direktori trash dengan menggunakan fungsi `checkRestricted`. Jika iya, maka akan memanggil fungsi `my_move`.
- Jika file atau direktori berada di luar trash dapat melakukan rename dan move seperti biasa `rename(old_fpath, new_fpath)`.

#### Output
![dalam](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_dalam.png)
![luar](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_luar.png)

### mkdir function
```
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
```

#### Penjelasan
- Fungsi untuk membuat direktori
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
- Cek apakah path berada di trash dengan menggunakan fungsi `checkRestricted`. Jika true return `-EINVAL` menandakan akses ditolak.
- Cek apakah mencoba membuat direktori dengan nama 'trash' atau 'restore'. Jika true return `-EINVAL` menandakan akses ditolak.
- `mkdir(fpath, mode)` membuat direktori baru.
- `appendLog("MKDIR", fpath, "")` catat log jika berhasil.

#### Output
![mkdir](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_mkdir.png)

### chown function
```
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
```

#### Penjelasan
- Fungsi untuk mengubah kepemilikan.
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
- Cek apakah path berada di trash dengan menggunakan fungsi `checkRestricted`. Jika true return `-EINVAL` menandakan akses ditolak.
- Gunakan `lchown(fpath, uid, gid)` untuk mengubah kepemilikian.
- `appendLog("CHOWN", fpath, "")` catat log jika berhasil.


### chmod function
```
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

```

#### Penjelasan
- Fungsi untuk mengubah izin dari direktori atau file.
-`sprintf(fpath, "%s%s", dirpath, path)` menyiapkan fullpath.
- Cek apakah path berada di trash dengan menggunakan fungsi `checkRestricted`. Jika true return `-EINVAL` menandakan akses ditolak.
- Gunakan fungsi `chmod(fpath, mode)` untuk mengubah permissions dari `fpath` sesuai dengan mode yang diberikan.
- `appendLog("CHMOD", fpath, "")` catat log jika berhasil.

#### Output
![chmod](https://github.com/sisop-its-s24/praktikum-modul-4-a15/blob/main/output/1_chmod.png)

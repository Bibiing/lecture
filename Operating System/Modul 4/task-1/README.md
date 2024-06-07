# TrashOps

In most Linux operating systems, the rm command will delete the index pointing to the file entry. Hence, it will be difficult to recover the file, or it can be said to have been permanently deleted. Interestingly, you're going to create a filesystem that modifies the file deletion and recovery process. Before that, please mount using the following resource: [link](https://drive.google.com/file/d/1WuFRDFHW3yYJpZkbgkON70P-9gFkicIR/view).

You are free to choose any destination directory.

- a. The code to create the filesystem is in the [trash.c](./trash.c) file. You must use the functions getattr, readdir, read, unlink, rmdir, rename, mkdir, chown, and chmod in the fuse_operations struct. You are free to add other functions in addition to the 9 functions mentioned above.

- b. When using the rm or rmdir commands for files or directories outside the trash directory, those files or directories will be moved to the trash directory and become unreadable, unwritable, and unexecutable by the owner, group, or other users.

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/18bd1c11-3eb0-47cc-9c95-e2f8096282a9)

  Only when the rm or rmdir commands are used for files or directories within the trash directory, those files or directories will be permanently deleted.

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/78c4175a-e064-407f-a644-d326bba02daa)

- c. Files or directories within the trash directory cannot have their permissions or ownership changed, and they cannot be renamed.

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/1e0460cc-b490-4f5a-adb0-476ef78ea578)

- d. To restore a file or directory from the trash directory, you must use the mv command with the format mv [path_to_file_in_trash] [arg]. The first option for arg is with a regular path, so the file or directory will be moved from the trash directory to that path. The second option for arg is restore, so the file or directory will return to its original path before being moved to the trash. Permissions for the restored file or directory must return to their initial state. Especially for using restore as arg, you need to create a correct path if the original path does not exist (deleted/moved)."

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/3a052576-806e-4c7a-bc23-821959e4e1a9)

- e. The trash directory cannot be deleted, moved, or renamed. You also cannot create directories with the names trash or restore.

- f. Write the log to the file trash.log. The format for each log line is YYMMDD-HH:MM:SS DESCRIPTION. The description format will be based on the following actions:

  | Action                                                                                                | Description                                      |
  | ----------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
  | Move a file or directory to the trash directory                                                       | `MOVED [original_path] TO TRASH`                 |
  | Permanently delete files or directories                                                               | `PERMANENTLY REMOVED [trash_path]`               |
  | Recover files or directories from the trash directory                                                 | `RESTORED [trash_path] FROM TRASH TO [new_path]` |
  | Attempt to change permissions, change ownership, or rename files or directories that are in the trash | `FAILED TO [CHMOD/CHOWN/RENAME] [trash_path]`    |
  | Trash directory delete, move, rename experiment                                                       | `FAILED TO [REMOVE/MOVE/RENAME] TRASH DIRECTORY` |
  | Attempt to create a directory with the name 'trash' or 'restore'                                      | `FAILED TO CREATE [TRASH/RESTORE] DIRECTORY`     |

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/bfedad43-1a15-46a1-9c5b-09aad7cb06e1)

Note :

- The trash directory already exists in the resources you will be using.
- Assume there is no case for deleting files or directories with the same name into the trash.
- Using system() function is not allowed.

# TrashOps

Pada kebanyakan sistem operasi Linux, perintah rm akan menghapus index yang mengarah pada entri file. Sehingga akan sulit untuk memulihkan file tersebut, atau dapat dikatakan telah terhapus permanen. Menariknya, anda akan membuat filesystem yang memodifikasi proses penghapusan dan pemulihan file dan direktori. Sebelum itu, silakan gunakan resource berikut sebagai direktori target: [link](https://drive.google.com/file/d/1WuFRDFHW3yYJpZkbgkON70P-9gFkicIR/view).

- a. Kode untuk membuat filesystem diletakkan pada [trash.c](./trash.c). Anda harus menggunakan fungsi getattr, readdir, read, unlink, rmdir, rename, mkdir, chown, dan chmod pada struct fuse_operations. Dibebaskan jika ingin menambahkan fungsi lain selain 9 fungsi diatas.

- b. Ketika menggunakan perintah rm atau rmdir untuk file atau direktori yang berada diluar direktori trash, maka file atau direktori tersebut akan dipindahkan ke direktori trash dan menjadi tidak dapat diread, write, dan execute baik oleh pemilik, grup, maupun user lainnya.

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/18bd1c11-3eb0-47cc-9c95-e2f8096282a9)

  Barulah ketika perintah rm atau rmdir digunakan untuk file atau direktori yang berada di dalam direktori trash, maka file atau direktori tersebut akan dihapus permanen.

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/78c4175a-e064-407f-a644-d326bba02daa)

- c. File atau direktori yang berada pada direktori trash tidak dapat diubah permission dan kepemilikannya, serta tidak dapat direname.

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/1e0460cc-b490-4f5a-adb0-476ef78ea578)

- d. Untuk memulihkan file atau direktori dari direktori trash, anda harus menggunakan perintah mv dengan format mv [path_file_dalam_trash] [arg]. Opsi pertama untuk arg adalah dengan path biasa sehingga file atau direktori akan dipindahkan dari direktori trash ke path tersebut. Opsi kedua untuk arg adalah restore sehingga file atau direktori akan kembali ke path asal sebelum ia dipindah ke trash. Permission untuk file atau direktori yang dipulihkan harus kembali seperti sebelum dimasukkan ke trash. Khusus untuk arg restore anda harus membuat path yang sesuai apabila path asal tidak ada (terhapus/dipindah).

  ![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/3a052576-806e-4c7a-bc23-821959e4e1a9)

- e. Direktori trash tidak dapat dihapus, dipindah, direname. Anda juga tidak dapat membuat direktori dengan nama trash atau restore.

- f. Catatlah log pada file trash.log. Format untuk tiap baris log adalah YYMMDD-HH:MM:SS KETERANGAN. Format keterangan akan berdasarkan aksi sebagai berikut:

| Aksi                                                                                                              | Keterangan                                           |
| ----------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------- |
| Memindahkan file atau direktori ke direktori trash                                                                | `MOVED [path_awal] TO TRASH`                         |
| Menghapus file atau direktori secara permanen                                                                     | `PERMANENTLY REMOVED [path_di_trash]`                |
| Memulihkan file atau direktori dari direktori trash                                                               | `RESTORED [path_di_trash] FROM TRASH TO [path_baru]` |
| Percobaan mengubah permission, mengubah kepemilikan, atau merename file atau direktori yang berada di dalam trash | `FAILED TO [CHMOD/CHOWN/RENAME][path_di_trash]`      |
| Percobaan menghapus, memindah, merename direktori trash                                                           | `FAILED TO [REMOVE/MOVE/RENAME] TRASH DIRECTORY`     |
| Percobaan membuat direktori dengan nama 'trash' atau 'restore'                                                    | `FAILED TO CREATE [TRASH/RESTORE] DIRECTORY`         |

![image](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/assets/54766683/bfedad43-1a15-46a1-9c5b-09aad7cb06e1)

Catatan:

- Direktori trash sudah terdapat pada resource yang akan anda gunakan.
- Anggap saja tidak ada kasus ketika menghapus file atau direktori dengan nama yang sama kedalam trash.
- Dilarang menggunakan fungsi system().

### Daftar Soal _(Task List)_
- [Task 1 - HORSECHAMP](task-1/)

## Task 1

DJumanto adalah seorang pengembang hebat yang telah menciptakan banyak kode keren. Saat ini, ia ingin membuat sebuah proyek baru yang disebut "horsechamp", sebuah program untuk lomba balapan kuda bernama horsechamp.c.

### Fungsi main

```
int main(){
    char *url = "https://docs.google.com/uc?export=download&id=1gJkoTbiHZcJ1M36vqZPTrj3qTQkRMElQ";
    char *namefile = "horses.zip";
    HorseFetcher(url, namefile);

    char *file = getFile();
    if(file != NULL){
        // printf("Random horse data file: %s\n", file);
        HorseRaceHooray(file);

        free(file);
    }

    HorseChampionLeaderboard();

    return 0;
}
```

#### Penjelasan

Dalam main ini digunakan untuk memanggil fungsi - fungsi yang digunakan.

- `*url` merupakan variabel dengan tipe data stirng yang menyimpan url dari google drive yang akan didownload.
- `*namefile` digunakan untuk menyimpan hasil folder zip yang telah didapatkan dari hasil download `*url`.
- `HorseFetcher(url, namefile)` memanggil fungsi **HorseFetcher** dengan url dan namefile sebagai parameternya.
- `*file = getFile()` membuat variabel file yang menyimpan hasil dari fungsi getFile().
- `if(file != NULL) HorseRaceHooray(file) ` selama filenya tidak kosong, memanggil fungsi HorseRaceHooray(file) dengan parameternya adalah nilai yang disimpan dalam variabel file.
- `free(file)` untuk membebaskan file dari memori
- `HorseChampionLeaderboard()` memanggil fungsi HorseChampionLeaderboard()


### Struct

```
struct Horse{
    int position;
    char name[20];
    char owner[20];
    int guess;
    double race_time;
};
```

#### Penjelasan

- `struct Horse` membuat struct dengan nama Horse.
- `position` variabel posisition dari kuda dengan tipe data integer.
- `name` variabel bertipe stirng yang digunakan untuk menyimpan nama kuda.
- `owner` variabel bertipe stirng yang digunakan untuk menyimpan nama pemilik kuda.
- `int guess` variabel yang menyimpan hasil tebakan dari kuda dengan tipe data integer.
- `double race_time` variabel yang menyimpan waktu perlombaan setiap kuda.

### Poin A

Buatlah fungsi HorseFetcher, yang bertugas mengambil data kuda dari internet dan menyimpannya dalam file dengan nama horse_1.txt, horse_2.txt, horse_3.txt, dan seterusnya. Kuda dan pemilik kuda dipisahkan dengan “:”, sebagai contoh “Morioh:DJumanto”, DJumanto adalah pemilik dari kuda Morioh.

```
void HorseFetcher(char* url, char* namefile){
    pid_t IDdownlaod = fork();
    int Dstat;

    if(IDdownlaod == 0){
        char *download_args[] = {"wget", "--no-check-certificate", url, "-q", "-O", namefile, NULL};
        execv("/usr/bin/wget", download_args);
    } else {
        waitpid(IDdownlaod, &Dstat, 0);
    }

    int Zstat;
    pid_t IDunzip = fork();
    if(IDunzip == 0){
        char *unzip_args[] = {"unzip", "-q", namefile, NULL};
        execv("/usr/bin/unzip", unzip_args);
    } else {
        waitpid(IDunzip, &Zstat, 0);
    }
}
```

#### Penjelasan

**_Download_**

- `void HorseFetcher(char* url, char* namefile)` fungsi HorseFetcher mengambil dua parameter: `url` yang merupakan URL dari file yang akan didownload, dan `namefile` merupakan nama file yang akan disimpan.
- `pid_t IDdownlaod` deklarasi variabel IDdownload dengan tipe data pid_t.
- `IDdownlaod = fork()` membuat sebuah child process menggunakan sistem call fork() dan mennyimpan nilai PID dari child process pada variable IDdownlaod.
- `if (IDdownlaod == 0)` jika IDdownload adalah 0, maka program ang sedang berjalan adalah child process.
- `*download_args` digunakan sebagai argumen yang nantinya akan dieksekusi oleh `execv`.
- `execv("/usr/bin/wget", download_args)` digunakann untuk menjalankan program `wget` dengan menggunakan perintah execv berparameter `/usr/bin/wget` sebagai path dari program `wget` dan download_args sebagai argumen yang akan disampaikan pada program wget
- `waitpid(IDdownlaod, &Dstat, 0);` digunakan untuk menunggu child process dengan PID yang sama dengan pid untuk selesai dan menyimpan status keluaran child process pada variabel Dstat.

**_unzip_**

- `pid_t IDunzip` deklarasi variabel IDunzip dengan tipe data pid_t.
- `IDunzip = fork()` membuat sebuah child process menggunakan sistem call fork() dan mennyimpan nilai PID dari child process pada variable IDunzip.
- `if (IDunzip == 0)` jika IDunzip adalah 0, maka program ang sedang berjalan adalah child process.
- `*unzip_args[]` digunakan sebagai argumen yang nantinya akan dieksekusi oleh `execv`.
- `execv("/usr/bin/unzip", unzip_args)` digunakann untuk menjalankan program `unzip` dengan menggunakan perintah execv berparameter `/usr/bin/unzip` sebagai path dari program `unzip` dan unzip_args sebagai argumen yang akan disampaikan pada program unzip
- `waitpid(IDunzip, &Zstat, 0)` digunakan untuk menunggu child process dengan PID yang sama dengan pid untuk selesai dan menyimpan status keluaran child process pada variabel Dstat.

#### Output
![Screenshot](https://github.com/nabil1407/nabil1407.github.io/raw/main/foto/Screenshot%202024-04-27%20214759.png)

### Poin B

Buatlah fungsi HorseRaceHooray, yang bertugas melaksanakan balapan kuda. Parameter yang diguakan adalah salah satu file kuda yang telah kalian fetch sebelumnya. Gunakan thread untuk menjalankan perlombaan tersebut. Setiap kuda akan melakukan perhitungan bilangan prima dengan angka acak antara 1 sampai 4000. Kuda yang menyelesaikan perhitungan lebih awal akan menempati posisi lebih tinggi dari kuda lainnya. Setelah perlombaan selesai, simpan hasil lomba dalam format HorseRace\_(nomor lomba).txt.

**_fungsi getFile_**

```
char* getFile(){
    DIR *dir;
    struct dirent *entry;
    int random = rand() % 3 + 1;
    char *filename = NULL;

    dir = opendir(".");
    if(dir == NULL){
        exit(EXIT_FAILURE);
    }

    while((entry = readdir(dir)) != NULL){
        if(strstr(entry->d_name, "horse_") == entry->d_name){
            random--;
            if(random == 0){
                filename = strdup(entry->d_name);
                break;
            }
        }
    }

    closedir(dir);
    return filename;
}
```

#### Penjelasan

Fungsi ini untuk mendapatkan file random dari file - file yang di unzip sebelumnya. Hasilnya digunakan sebagai parameter dalam fungsi utama `HorseRaceHooray`;

- `DIR *dir` merupakan sebuah tipe data bawaan library `<dirent.h>` digunakan untuk membuka suatu directury dengan bantuan fungsi `opendir()`.
- `struct dirent *entry` merupakan sebuah tipe data bertipe struct yang digunakan untuk menunjuk file yang ada dalam suatu folder tertentu.
- `int random = rand() % 3 + 1` mengambil nilai random dari 1 - 3 dan di masukkan ke variabel random.
- `char *filename = NULL` deklarasi varibel filename dengan null.
- `dir = opendir(".")` membuka direktori "." menandakan di dalam direktori yang sama.
- `if(dir == NULL)` kalau dir = NULL fungsi akan exit.
- `while((entry = readdir(dir)) != NULL)` akan membaca semua entri dalam direktori. `readdir(dir)` digunakan untuk membaca entri berikutnya dalam direktori yang ditunjukkan oleh pointer dir. setiap pemanggilan readdir() akan membaca entri berikutnya dalam direktori.
- `if(strstr(entry->d_name, "horse_") == entry->d_name)` memeriksa apakah substring "horse_" ditemukan di awal nama file. fungsi strstr() digunakan untuk mencari substring "horse_" dalam nama file yang sedang diproses. Pemeriksaan ini memeriksa apakah hasil dari strstr() sama dengan entry->d_name. 

**_fungsi getLastRace_**

```
int getLastRace(){
    int lastRaceNumber = 0;
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(".")) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, "HorseRace_") == entry->d_name) {
                int raceNumber = atoi(entry->d_name + strlen("HorseRace_"));
                if (raceNumber > lastRaceNumber) {
                    lastRaceNumber = raceNumber;
                }
            }
        }
        closedir(dir);
    } else {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }
    return lastRaceNumber;
}
```

#### Penjelasan

Fungsi ini tujuan utamanya adalah mendapatkan nama file perlombaan terakhir. Dari fungsi ini return nya adalah sebuah integer, yang akan digunakan nantinya untuk membuat file `HorseRace_%d.txt` sesuai urutannya.

**_fungsi horseRace_**

```
void *horseRace(void *arg){
    struct Horse *horse = (struct Horse *)arg;
    clock_t start_time = clock();
    int number;
    do {
        number = rand() % 4000 + 1;
    } while (!isPrime(number));
    horse->guess = number;
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    horse->race_time = time_taken * 1000;

    pthread_exit(NULL);
}
```

#### Penjelasan

Tujuan dari fungsi ini adalah untuk melakukan perlombaan dari setiap kuda. Dengan cara menyimpan informasi perlombaan setiap kuda ke race_time, dan isi dari guess adalah nilai prima yang dihitung masing masing kuda.

- `clock_t start_time = clock();` menandakan perhitungan/perlombaan dimulai.
- `clock_t end_time = clock();` menandakan perhitungan/perlombaan selesai.
- `while (!isPrime(number))` melakukan do while loop untuk mencari bilangan prima random dari 1 sampai 4000.
- `horse->guess = number` nilai prima yang berhasil di hitung oleh kuda di simpan di guess.
- `double time_taken = ((double)(end_time - start_time))` menghitung berapa lama kuda menghitung bilangan prima.

***fungsi sortWinners***
```
void sortWinners(struct Horse *horses, int num_horses) {
    for (int i = 0; i < num_horses - 1; i++) {
        for (int j = 0; j < num_horses - i - 1; j++) {
            if (horses[j].race_time > horses[j + 1].race_time) {
                struct Horse temp = horses[j];
                horses[j] = horses[j + 1];
                horses[j + 1] = temp;
            }
        }
    }
}
```

#### Penjelasan
Menggunakan implementasi bubble sort.

**_fungsi isPrime_**

```
int isPrime(int num){
    if(num <= 1) return 0;
    if(num <= 3) return 1;
    if(num % 2 == 0 || num % 3 == 0) return 0;
    for(int i = 5; i * i <= num; i += 6){
        if(num % i == 0 || num % (i + 2) == 0)
            return 0;
    }
    return 1;
}
```

#### Penjelasan
Fungsi isPrime seperti pada umumnya yang akan mengembalikan 0 jika prima dan 1 jika tidak prima.


**_fungsi removeNewline_**

```
char *removeNewline(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
    return str;
}
```

#### Penjelasan

fungsi untuk menghapus newline.

**_fungsi HorseRaceHooray_**

```
void HorseRaceHooray(char *fileHorses){
    int lastRace = getLastRace();
    int num = lastRace + 1;

    FILE *file = fopen(fileHorses, "r");
    if (file == NULL) {
        printf("Error opening file %s.\n", fileHorses);
        return;
    }
    
    //membaca banyak baris
    int num_horses = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        num_horses++;
    }

    rewind(file);

    struct Horse horses[num_horses];

    int i = 0;
    while(fgets(line, sizeof(line), file)){
        sscanf(line, "%[^:]:%[^\n]", horses[i].name, horses[i].owner);
        i++;
    }

    fclose(file);

    pthread_t threads[num_horses];
    for(int i = 0; i < num_horses; i++){
        if(pthread_create(&threads[i], NULL, horseRace, &horses[i]) != 0){
            fprintf(stderr, "Error creating thread %s\n", horses[i].name);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < num_horses; i++) {
        pthread_join(threads[i], NULL);
    }

    sortWinners(horses, num_horses);

    for (i = 0; i < num_horses; i++) {
        horses[i].position = i + 1;
    }


    char outputFileName[50];
    sprintf(outputFileName, "HorseRace_%d.txt", num);
    FILE *outputFile = fopen(outputFileName, "w");
    if(outputFile == NULL){
        printf("Error opening output file %s.\n", outputFileName);
        return;
    }

    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);
    char date[20];
    strftime(date, sizeof(date), "%d/%m/%Y", tm_struct);
    char time[20];
    strftime(time, sizeof(time), "%H:%M:%S", tm_struct);

    fprintf(outputFile, "----Horse Race %d------\n", num);
    fprintf(outputFile, "Date: %s\n", date);
    fprintf(outputFile, "Time: %s\n\n", time);

    for (i = 0; i < num_horses; i++) {
        fprintf(outputFile, "%d. %s %s %d\n", horses[i].position, horses[i].name, removeNewline(horses[i].owner), horses[i].guess);
    }

    fclose(outputFile);

    printf("Results written to %s.\n", outputFileName);    
}
```

#### Penjelasan
- `int lastRace = getLastRace(); int num = lastRace + 1;` Mengambil nomor balapan terakhir yang dilakukan dengan bantuan fungsi `getLastRace()`. Kemudian, nomor balapan berikutnya ditentukan dengan menambahkan 1 ke nomor balapan terakhir.
- `FILE *file = fopen(fileHorses, "r")` Membuka file kuda yang disediakan dalam mode baca. Jika file tidak dapat dibuka, pesan error akan dicetak dan fungsi akan mengembalikan nilai.
- `int num_horses = 0` Variabel ini digunakan untuk menyimpan jumlah kuda dalam balapan. Nilai ini akan dihitung dengan menghitung jumlah baris dalam file.
- `while (fgets(line, sizeof(line), file)) { num_horses++; }` Menghitung jumlah kuda dalam balapan dengan membaca setiap baris dalam file dan meningkatkan nilai `num_horses` setiap kali sebuah baris dibaca.
- `rewind(file)` Mengembalikan posisi file ke awal sehingga file dapat dibaca ulang dari awal.
- `while (fgets(line, sizeof(line), file)) { sscanf(line, "%[^:]:%[^\n]", horses[i].name, horses[i].owner); i++; }` Membaca data dari file dan menyimpannya ke dalam struktur Horse. Setiap baris dalam file dipecah menjadi dua bagian di tandai dengan ':' nama kuda dan pemiliknya, yang kemudian disalin ke dalam horses.
- `fclose(file)` Menutup file setelah selesai membacanya.
- `pthread_t threads[num_horses]; for (int i = 0; i < num_horses; i++) { if (pthread_create(&threads[i], NULL, horseRace, &horses[i]) != 0) { fprintf(stderr, "Error creating thread %s\n", horses[i].name); exit(EXIT_FAILURE); } }`  Membuat thread untuk setiap kuda dalam balapan. Setiap thread akan menjalankan fungsi horseRace untuk melakukan simulasi balapan.
- `for (int i = 0; i < num_horses; i++) { pthread_join(threads[i], NULL); }` Menunggu semua thread selesai dieksekusi sebelum melanjutkan ke langkah berikutnya.
- `sortWinners(horses, num_horses)`  Mengurutkan kuda-kuda berdasarkan waktu balapan mereka menggunakan fungsi `sortWinners`.
- `for (int i = 0; i < num_horses; i++) { horses[i].position = i + 1; }` Memberikan posisi kepada setiap kuda berdasarkan urutan setelah diurutkan.
- `char outputFileName[50]; sprintf(outputFileName, "HorseRace_%d.txt", num); FILE *outputFile = fopen(outputFileName, "w")` Membuat nama file output berdasarkan nomor balapan yang baru. Kemudian, membuka file output dalam mode penulisan
- `time_t now = time(NULL); struct tm *tm_struct = localtime(&now)` Mengambil waktu saat ini dan mengonversinya menjadi struktur waktu lokal menggunakan fungsi `localtime`.
- `fprintf(outputFile, "----Horse Race %d------\n", num); fprintf(outputFile, "Date: %s\n", date); fprintf(outputFile, "Time: %s\n\n", time)` Menulis header informasi tentang balapan ke dalam file output, termasuk nomor balapan, tanggal, dan waktu.
- `for (int i = 0; i < num_horses; i++) { fprintf(outputFile, "%d. %s %s %d\n", horses[i].position, horses[i].name, removeNewline(horses[i].owner), horses[i].guess); }` Menulis hasil balapan untuk setiap kuda ke dalam file output.
- `fclose(outputFile)` Menutup file output setelah selesai menulis.
- `printf("Results written to %s.\n", outputFileName)` Mencetak pesan di terminal bahwa hasil balapan telah ditulis ke dalam file output.

#### Output
![Screenshot](https://github.com/nabil1407/nabil1407.github.io/blob/main/foto/Screenshot%202024-04-27%20215250.png)
![Screenshot](https://github.com/nabil1407/nabil1407.github.io/blob/main/foto/Screenshot%202024-04-27%20220353.png)

### Poin C
Buatlah fungsi HorseChampionLeaderboard, yang bertugas menampilkan pemenang dari pertandingan-pertandingan sebelumnya. Berikan opsi kepada pengguna untuk memilih lomba mana yang ingin dilihat hasilnya.

```
void HorseChampionLeaderboard() {
    DIR *dir;
    struct dirent *entry;
    int num;

    dir = opendir(".");
    if(dir == NULL){
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "HorseRace_") == entry->d_name) {
            printf("%s\n", entry->d_name);
        }
    }
    closedir(dir);

    printf("Choose your Horse Race history:\n");
    scanf("%d", &num);

    char filename[50];
    sprintf(filename, "HorseRace_%d.txt", num);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s.\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Change "----Horse Race" to "----HORSE CHAMPIONSHIP"
        if (strstr(line, "----Horse Race") != NULL) {
            printf("----HORSE CHAMPIONSHIP %d----\n", num);
        } else {
            printf("%s", line);
        }
    }

    fclose(file);
}
```

#### Penjelasan
Fungsi HorseChampionLeaderboard bertujuan untuk menampilkan papan klasemen atau leaderboard balapan kuda yang sudah dilakukan sebelumnya dengan memberikan beberapa pilihan perlombaan yang telah berlangsung bagi pengguna.

- `DIR *dir; struct dirent *entry; int num`Deklarasi variabel *dir ke struktur direktori dan *entry ke struktur dirent untuk menunjuk entri dalam direktori. Variabel num akan digunakan untuk menyimpan nomor balapan yang dipilih oleh pengguna.
- `dir = opendir(".")` Membuka direktori saat ini dengan bantuan fungsi `opendir()`. Jika direktori tidak dapat dibuka, pesan error akan dicetak dan fungsi akan mengembalikan nilai.
- `while ((entry = readdir(dir)) != NULL) { if (strstr(entry->d_name, "HorseRace_") == entry->d_name) { printf("%s\n", entry->d_name); } }` Mencari semua entri dalam direktori saat ini menggunakan while loop. Jika nama file yang ditemukan mengandung substring "HorseRace_", maka nama file tersebut akan dicetak, yang menunjukkan bahwa itu adalah file hasil balapan kuda.
- `closedir(dir)` Menutup direktori setelah selesai membacanya.
- `printf("Choose your Horse Race history:\n"); scanf("%d", &num)` Meminta pengguna untuk memilih nomor balapan yang ingin dilihat leaderboardnya.
- `char filename[50]; sprintf(filename, "HorseRace_%d.txt", num); FILE *file = fopen(filename, "r")` Membuka file hasil balapan yang sesuai dengan nomor balapan yang dipilih oleh pengguna. Jika file tidak dapat dibuka, pesan error akan dicetak.
- `char line[256]; while (fgets(line, sizeof(line), file)) { if (strstr(line, "----Horse Race") != NULL) { printf("----HORSE CHAMPIONSHIP %d----\n", num); } else { printf("%s", line); } }` Membaca isi file dengan menggunakan while loop.Jika baris yang dibaca mengandung substring "----Horse Race", maka baris tersebut akan dicetak dengan tambahan kata "HORSE CHAMPIONSHIP". Jika tidak, baris tersebut akan dicetak tanpa modifikasi.
- `fclose(file)` Menutup file setelah selesai membacanya.

#### Output
![Screenshot](https://github.com/nabil1407/nabil1407.github.io/blob/main/foto/Screenshot%202024-04-27%20220336.png)
![Screenshot](https://github.com/nabil1407/nabil1407.github.io/blob/main/foto/Screenshot%202024-04-27%20220403.png)

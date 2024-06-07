**struct**
```
struct Services{
    char containerName[100];
    char img[100];
    char command[100];
    char volume[100];
};
```

### Poin A
Membuat 2 file, yaitu server.c dan client.c.
- Disaat server.c pertama kali dijalankan, dia akan meminta prompt user untuk mengetikkan berapa banyak client yang akan dilayani (range 1-5).

**_Server.c_**
```
#define PORT 4040
int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int num;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client: "); scanf("%d", &num);
    send(new_socket, &num, sizeof(num), 0);
}
```

#### Penjelasan
Di task ini karena dibebaskan untuk memilih IPC, saya memilih menggunakan socket.
- `#define PORT 4040` nomor port yang akan digunakan.
- `server_fd` untuk menyimpan deskriptor file untuk socket server.
- `new_socket` deskriptor socket baru yang diterima.
- `struct sockaddr_in address` merupakan struct untuk menyimpan alamat jaringan pada protokol IPv4.`address` digunakan untuk menyimpan alamat server.
- `opt = 1` untuk setsockopt mengatur opsi socket
- `addrlen = sizeof(address)` menampung ukuran dari struct alamat server.
- `num` untuk menyimpan inputan pengguna.
- `server_fd = socket(AF_INET, SOCK_STREAM, 0)` membuat socket.
- `setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))` mengatur opsi socket dengan opsi `SO_REUSEADDR` digunakan untuk memungkinkan penggunaan kembali alamat dan port yang sama setelah server dihentikan dan kemudian dijalankan kembali.
- `address.sin_family = AF_INET` tipe alamat yang digunakan adalah IPv4 dengan konstanta `AF_INET`.
- `address.sin_addr.s_addr = INADDR_ANY` Menetapkan alamat IP untuk binding socket. `INADDR_ANY` mengindikasikan server akan listen koneksi yang datang dari semua alamat IP yang dimilikinya.
- `address.sin_port = htons(PORT)` Menetapkan nomor port untuk binding socket. `htons()` digunakan untuk mengkonversi nomor port ke dalam format yang sesuai, variabel `PORT` harus sudah diatur sebelumnya dengan nomor port yang diinginkan.
- `bind(server_fd, (struct sockaddr *)&address, sizeof(address))` memasang socket ke server ke alamat dan port.`bind()` fungsi yang digunakan untuk memasangkan socket dengan alamat dan port. parameter yang diperlukan tiga yaitu socket file descriptor, alamat, dan ukuran alamat.
- `listen(server_fd, 3)` digunakan untuk menerima koneksi masuk dari client. parameter yang digunakan adalah socket file descriptor dan backlog. backlog ini menentukan jumlah maksimum koneksi yang dapat ditangani.
- `new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)` untuk berkuomunikasi dengan client yang terhubung. fungsi dari `accpet()` digunakan untuk menerima koneksi masuk dari client.
- `scanf("%d", &num);` prompt user menggetikkan berapa banyak client yang akan dilayani (range 1-5).
- `send(new_socket, &num, sizeof(num), 0)` mengirimkan data ke client yang terhubung melalui socket `new_socket`.

- Disaat client.c pertama kali dijalankan, dia akan meminta prompt user untuk mengetikkan username. Username ini kemudian dikirim ke server, untuk di check apakah sudah ada sebelumnya atau tidak. Jika sudah ada, maka akan meminta prompt lagi untuk mengetikkan username sampai diterima oleh server.

***_client.c_***
```
#define PORT 4040

int main(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    int num;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    recv(sock, &num, sizeof(num), 0);
    char username[num][100];

    char msg[200];
    for (int i = 0; i < num; i++) {
        printf("Name: "); scanf("%s", username[i]);
        send(sock, username[i], strlen(username[i]), 0);

        recv(sock, msg, sizeof(msg), 0);
        printf("%s", msg);
        if(strstr(msg, "already exists")) {
            i--;
        }
    }
}
```

#### Penjelasan
- `#define PORT 4040` nomor port yang akan digunakan.
- ``sock` untuk menyimpan deskriptor file untuk socket server.`
- `sock = socket(AF_INET, SOCK_STREAM, 0`
- `struct sockaddr_in serv_addr` merupakan struct untuk menyimpan alamat jaringan pada protokol IPv4.`serv_addr` digunakan untuk menyimpan alamat.
- `serv_addr.sin_family = AF_INET` tipe alamat yang digunakan adalah IPv4 dengan konstanta `AF_INET`.
- `serv_addr.sin_port = htons(PORT)` Menetapkan nomor port untuk binding socket. `htons()` digunakan untuk mengkonversi nomor port ke dalam format yang sesuai, variabel `PORT` harus sudah diatur sebelumnya dengan nomor port yang diinginkan.
- `connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))` menghubungkan socket ke server.
- `recv(sock, &num, sizeof(num), 0)` menerima data dari server. `sock` socket yang digunakan untuk menerima data, `&num` alamat tempat penyimpanan data, `sizeof(num)` ukuran data.
- `username[num][100]` variabel untuk menyimpan username sebanyak num, dan panjang username 100.
- `msg[200]` menyimpan pesan yang akan dikirim / diterima.
- `scanf("%s", username[i]);` input username ke -i(menngunakan forloop).
- `send(sock, username[i], strlen(username[i]), 0)` setiap inputan username akan dikirimkan ke server untuk di cek.
- `recv(sock, msg, sizeof(msg), 0)` menerima pesan yang dikirimkan dari server. 
- `strstr(msg, "already exists") i--` jika dari pesan terkandung substring "already exists". Akan meminta user memasukkan username lain dan i akan di decrement.

**_server.c_**
pengecekkan username:
```
char username[num][100];
for(int i = 0; i < num; i++){
    int valread = read(new_socket, username[i], sizeof(username[i]));
    if (valread <= 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    username[i][valread] = '\0';
    printf("Received username from client: %s\n", username[i]);

    bool exists = false;
    for(int j = 0; j < i; j++){
        if(strcmp(username[i], username[j]) == 0){
            exists = true;
            break;
        }
    }

    if (exists) {
        sprintf(msg, "Username %s already exists. Try again.\n", username[i]);
        send(new_socket, msg, strlen(msg), 0);
        i--; 
        continue;
    }

    printf("Player %s joined --(%d)\n", username[i], i + 1);
    fflush(stdout);
    sprintf(msg, "%s registered.\n", username[i]);
    send(new_socket, msg, strlen(msg), 0);
}
```

#### Penjelasan
- `valread = read(new_socket, username[i], sizeof(username[i]))` membaca data dari socket client dan menyimpannya ke dalam username. `valread` akan berisi jumlah byte yang berhasil dibaca.
- `username[i][valread] = '\0'` Menambahkan null-terminator di akhir string untuk memastikan string berakhir dengan benar.
- `bool exists = false` untuk menandai apakah username sudah tersedia atau belum.
- `if(strcmp(username[i], username[j]) == 0)` pengecekkan username.
- jika username sudah tersedia `sprintf(msg, "Username %s already exists. Try again.\n", username[i])` memformat string ke msg.
- `send(new_socket, msg, strlen(msg), 0)` mengirimkan msg jika username sudah tersedia.
- jika user name belum tersedia `sprintf(msg, "%s registered.\n", username[i])` memformat string ke msg.
- `send(new_socket, msg, strlen(msg), 0)` mengirimkan msg jika username belum tersedia.

#### Output
![task 2](https://drive.google.com/uc?id=1wtaYVC7aKT77zsXurJuGUdhyaP1fWlSE)



### Poin B
Setiap client akan diberikan prompt untuk mengetikkan service yang dibutuhkan yaitu container name, img, command, volme. Urutan client yang mengirimkan request service ke server sesuai urutan.

**_client.c_**
```
struct Services serv[num];
for(int i = 0; i < num; i++){
    printf("%d. %s services\n", i+1,  username[i]);
    printf("Nama Container: "); scanf(" %[^\n]s", serv[i].containerName);
    printf("Image yang Digunakan: "); scanf(" %[^\n]s", serv[i].img);
    printf("Perintah Saat Container Dimulai: "); scanf(" %[^\n]s", serv[i].command);
    printf("Volume: "); scanf(" %[^\n]s", serv[i].volume);

    send(sock, &serv[i], sizeof(serv[i]), 0);
    recv(sock, msg, sizeof(msg), 0);
    printf("%s", msg);
}

recv(sock, msg, sizeof(msg), 0);
```

#### Penjelasan 
- `struct Services serv[num]` variabel serv untuk menampung prompt masing masing client sebanyak num.
- `for(...)` forloop untuk meminta inputan kepada user.
- `send(sock, &serv[i], sizeof(serv[i]), 0)` mengirimkan setiap prompt dari user ke server.
- `recv(sock, msg, sizeof(msg), 0)` menerima pesan dari server.
- `printf("%s", msg)` menampilkan pesan.


**_server.c_**
```
struct Services serv[num];
for(int i = 0; i < num; i++){
    recv(new_socket, &serv[i], sizeof(serv[i]), 0);
    sprintf(msg, "Service data received from client %d.\n", i + 1);
    send(new_socket, msg, strlen(msg), 0);
}
```

#### Penjelasan 
- `struct Services serv[num]` variabel serv untuk menampung prompt yang dikirimkan client sebanyak num.
- `for(...)` forloop untuk menerima inputan yang dikirim dari client.
- `recv(new_socket, &serv[i], sizeof(serv[i]), 0)` menerima pesan dari client, dan disimpan pada serv.
- `sprintf(msg, ...)` menulsikan string ke dalam msg.
- `send(new_socket, msg, strlen(msg), 0)` mengirimkan msg ke client.

#### Output
![task 2](https://drive.google.com/uc?id=1tyU67k4jHsUxgNkwtr6aYiGz50g01IEN)

### Poin C
server menggabungkansetiap service menjadi sebuah file docker compose. lalu file docker compose yang dibuat dijalankan.

```
FILE *compose = fopen("docker-compose.yml", "w");
fprintf(compose, "version: '3'\n\nservices:\n");

for (int i = 0; i < num; i++) {
    fprintf(compose, "  %s:\n", serv[i].containerName);
    fprintf(compose, "    image: %s\n", serv[i].img);
    fprintf(compose, "    command: %s\n", serv[i].command);
    fprintf(compose, "    volumes:\n");
    fprintf(compose, "      - %s\n", serv[i].volume);
    fprintf(compose, "\n");
}

fclose(compose);
printf("Running Docker Compose...\n");

pid_t pid = fork();
if (pid == 0) {
    execlp("docker-compose", "docker-compose", "up", "-d", NULL);
    perror("exec failed");
    exit(EXIT_FAILURE);
} else if (pid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
} else {
    wait(NULL); 
}
```

#### Penjelasan
- `FILE *compose = fopen("docker-compose.yml", "w")` membuat / membuka file docker-compose.yml dengan mode 'w' write.
- `fprinf(compose, ...)` menuliskan string ke dalam compose file.
- `fclose(compose)` menutup file compose.
- `pid_t pid = fork()` membuat chile process.
- jika true `execlp("docker-compose", "docker-compose", "up", "-d", NULL)` menjalankan argumen "docker-compose up -d" pada file docker.
- `wait(NULL)` menunggu child process selesai.

#### Output
![task 2](https://drive.google.com/uc?id=1QZ8JuapZ-w3_mf0AyoqRYcKbC_IL80QM)


### Poin D
Setelah file docker dijalankan, client akan minta prompt lagi dari user. prompt yang diberikan sesuai Poin B. Lalu server akan membuat file docker compose baru, file lama dihancurkan, yang kemudiaan akan digantikan oleh service terbaru.

**_client.c_**
```
recv(sock, msg, sizeof(msg), 0);
struct Services newServ[num];
if(strstr(msg, "Docker Compose selesai dijalankan")) {
    printf("Masukkan serv baru: \n");
    for(int i = 0; i < num; i++){
        printf("%d. %s services\n", i+1,  username[i]);
        printf("Nama Container: "); scanf(" %[^\n]s", newServ[i].containerName);
        printf("Image yang Digunakan: "); scanf(" %[^\n]s", newServ[i].img);
        printf("Perintah Saat Container Dimulai: "); scanf(" %[^\n]s", newServ[i].command);
        printf("Volume: "); scanf(" %[^\n]s", newServ[i].volume);

        send(sock, &newServ[i], sizeof(newServ[i]), 0);
        recv(sock, msg, sizeof(msg), 0);
        printf("%s", msg);
    }
}

close(sock);
```

#### Penjelasan
- `recv(sock, msg, sizeof(msg), 0)` menerima pesan dari server.
- `struct Services newServ[num];` struct baru untuk menyimpan services sebanyak num;
- Sisanya prompt seperti poin b.

**_server.c_**
```
struct Services newServ[num];
for(int i = 0; i < num; i++){
    recv(new_socket, &newServ[i], sizeof(newServ[i]), 0);
    sprintf(msg, "Service data received from client %d.\n", i + 1);
    send(new_socket, msg, strlen(msg), 0);
}

FILE *new_compose = fopen("new-docker-compose.yml", "w");
fprintf(new_compose, "version: '3'\n\nservices:\n");

for (int i = 0; i < num; i++) {
    fprintf(new_compose, "  %s:\n", newServ[i].containerName);
    fprintf(new_compose, "    image: %s\n", newServ[i].img);
    fprintf(new_compose, "    command: %s\n", newServ[i].command);
    fprintf(new_compose, "    volumes:\n");
    fprintf(new_compose, "      - %s\n", newServ[i].volume);
    fprintf(new_compose, "\n");
}

fclose(new_compose);
printf("Running New Docker Compose...\n");

pid = fork();
if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
} else if (pid == 0) {
    char *args[] = {"docker-compose", "down", NULL};
    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
} else {
    wait(NULL);
}

pid = fork();
if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
} else if (pid == 0) {
    char *args[] = {"docker-compose", "-f", "new-docker-compose.yml", "up", "-d", NULL};
    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
} else {
    wait(NULL);
}
```

#### Penjelasan
- `struct Services newServ[num];` struct untuk menyimpan data services baru.
- `FILE *new_compose = fopen("new-docker-compose.yml", "w")` membuat file compose baru dengan mode write.
- `char *args[] = {"docker-compose", "down", NULL}` argumen untuk menghancurkan file docker lama.
- `execvp(args[0], args)` menjalankan argumen.
- `char *args[] = {"docker-compose", "-f", "new-docker-compose.yml", "up", "-d", NULL}` argumen untuk menjalankan docker compose baru.

#### Output
![task 2](https://drive.google.com/uc?id=1OLf3E1qevzbYYN9Z8ATw_eNf9rYAxCTS)
![task 2](https://drive.google.com/uc?id=1MZ68BvU6tUwEgHSTLWCbufNpmoFcdwJC)


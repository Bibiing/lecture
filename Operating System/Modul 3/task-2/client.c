#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4040

struct Services{
    char containerName[100];
    char img[100];
    char command[100];
    char volume[100];
};

int main() {
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

    return 0;
}

//compile
//gcc -o client client.c && ./client

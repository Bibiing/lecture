#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdbool.h>

#define PORT 4040

struct Services{
    char containerName[100];
    char img[100];
    char command[100];
    char volume[100];
};

int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int num;
    char msg[200];

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

    printf("Waiting %d input from clients\n", num);
    struct Services serv[num];
    for(int i = 0; i < num; i++){
        recv(new_socket, &serv[i], sizeof(serv[i]), 0);
        sprintf(msg, "Service data received from client %d.\n", i + 1);
        send(new_socket, msg, strlen(msg), 0);
    }
    
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

    sprintf(msg, "Docker Compose selesai dijalankan.\n");
    send(new_socket, msg, strlen(msg), 0);

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

    return 0;
}

//gcc -o server server.c && ./server

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

struct Horse{
    int position;
    char name[20];
    char owner[20]; 
    int guess; 
    double race_time;
};

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

char *removeNewline(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
    return str;
}

void HorseRaceHooray(char *fileHorses){
    int lastRace = getLastRace();
    int num = lastRace + 1;

    FILE *file = fopen(fileHorses, "r");
    if (file == NULL) {
        printf("Error opening file %s.\n", fileHorses);
        return;
    }
    
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
        if (strstr(line, "----Horse Race") != NULL) {
            printf("----HORSE CHAMPIONSHIP %d----\n", num);
        } else {
            printf("%s", line);
        }
    }

    fclose(file);
}

int main(){
    char *url = "https://docs.google.com/uc?export=download&id=1gJkoTbiHZcJ1M36vqZPTrj3qTQkRMElQ";
    char *namefile = "horses.zip";
    HorseFetcher(url, namefile);

    char *file = getFile();
    if(file != NULL){
        printf("Random horse data file: %s\n", file);
        HorseRaceHooray(file);

        free(file);
    } 
    
    HorseChampionLeaderboard();

    return 0;
}

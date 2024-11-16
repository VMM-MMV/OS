#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#define FILE_NAME "common_file.txt"
sem_t mutex;
int i = 0;

void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
}

void write_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    fwrite(content, sizeof(char), 1, file);
    fclose(file);
}

void *reader(void *arg) {
    while (1) {
        sem_wait(&mutex);
        sem_post(&mutex);
        read_file(FILE_NAME);
        sleep(2);
    }
    return NULL;
}

void *writer(void *arg) {
    while (1) {
        sem_wait(&mutex);
        sem_post(&mutex);
        write_file(FILE_NAME, &ALPHABET[i++%26]);
        sleep(5);
    }
    return NULL;
}

int main() {
    sem_init(&mutex, 0, 1);

    int reader_count = 5;
    int writer_count = 2;

    pthread_t readers[reader_count];
    pthread_t writers[writer_count];

    for (int i = 0; i < reader_count; i++) {
        pthread_create(&readers[i], NULL, reader, NULL);
    }

    for (int i = 0; i < writer_count; i++) {
        pthread_create(&writers[i], NULL, writer, NULL);
    }

    for (int i = 0; i < reader_count; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < writer_count; i++) {
        pthread_join(writers[i], NULL);
    }

    sem_destroy(&mutex);

    return 0;
}


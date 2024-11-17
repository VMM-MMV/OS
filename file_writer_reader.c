#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

const char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
#define FILE_NAME "common_file.txt"

sem_t mutex;
int current_index = 0;

void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    char ch;
    if ((ch = fgetc(file)) != EOF) {
        printf("\r%c", ch);
        fflush(stdout);
    }
    fclose(file);
}

void write_file(const char *filename, char content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    fputc(content, file);
    fclose(file);
}

void *reader(void *arg) {
    while (1) {
        sem_wait(&mutex);
        sem_post(&mutex);
        read_file(FILE_NAME);
    }
    return NULL;
}

void *writer(void *arg) {
    while (1) {
        sem_wait(&mutex);
        char current_char = ALPHABET[current_index];
        write_file(FILE_NAME, current_char);
        current_index = (current_index + 1) % 26;
        sem_post(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    write_file(FILE_NAME, 'A');
    sem_init(&mutex, 0, 1);
    int reader_count = 5;
    int writer_count = 2;
    
    pthread_t readers[reader_count];
    pthread_t writers[writer_count];
    
    for (int i = 0; i < writer_count; i++) {
        pthread_create(&writers[i], NULL, writer, NULL);
    }
    for (int i = 0; i < reader_count; i++) {
        pthread_create(&readers[i], NULL, reader, NULL);
    }
    for (int i = 0; i < writer_count; i++) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < reader_count; i++) {
        pthread_join(readers[i], NULL);
    }
    sem_destroy(&mutex);
    return 0;
}


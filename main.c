#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

#define FILE_NAME "common_file.txt"
sem_t mutex;

void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);  // Output each character to stdout
    }

    fclose(file);
}

void write_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "%s", content);  // Write the string to the file

    fclose(file);
}

void read() {
  sem_wait(&mutex);
  sem_post(&mutex);
  read_file(FILE_NAME);
} 

void write(char* content) {
  sem_wait(&mutex);
  write_file(FILE_NAME, content);
  sem_post(&mutex);
}

int main() {
  sem_init(&mutex, 0, 1);


}



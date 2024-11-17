#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE] = {0};
int writer_i = 0;
int reader_i = 0;
sem_t writer_lock;
sem_t reader_lock;

void print_arr(int arr[], int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        if (arr[i] == -1) {
            printf("_ ");  // Empty cell
        } else {
            printf("%d ", arr[i]);
        }
    }
    printf("]\n");
}

void *writer(void* args) {
    long id = (long)args;
    while (1) {
        sem_wait(&writer_lock);
        if (buffer[writer_i % BUFFER_SIZE] != -1) {
            sem_post(&writer_lock);
            usleep(100000);  // Wait 100ms before trying again
            continue;
        }
        
        int pos = writer_i % BUFFER_SIZE;
        writer_i++;
        
        printf("\033[0;32m[Writer %ld] Starting to write at position %d\033[0m\n", id, pos);
        print_arr(buffer, BUFFER_SIZE);
        
        sem_post(&writer_lock);
        
        buffer[pos] = id * 100 + pos;  // Makes it easy to identify which writer wrote what
        
        usleep((rand() % 1000000) + 500000);  // 0.5 to 1.5 second pause
    }
}

void *reader(void* args) {
    long id = (long)args;
    while(1) {
        sem_wait(&reader_lock);
        if (buffer[reader_i % BUFFER_SIZE] == -1) {
            sem_post(&reader_lock);
            usleep(200000);  // Wait 200ms before trying again
            continue;
        }
        
        int pos = reader_i % BUFFER_SIZE;
        reader_i++;
        
        printf("\033[0;34m[Reader %ld] Starting to read from position %d\033[0m\n", id, pos);
        print_arr(buffer, BUFFER_SIZE);
        
        sem_post(&reader_lock);
        
        int value = buffer[pos];
        buffer[pos] = -1;  // Mark as empty
        
        usleep((rand() % 2000000) + 1000000);  // 1 to 3 second pause
    }
}

int main() {
    srand(time(NULL));
    
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = -1;
    }
    
    sem_init(&writer_lock, 0, 1);
    sem_init(&reader_lock, 0, 1);
    
    int reader_count = 4;
    int writer_count = 2;
    
    pthread_t readers[reader_count];
    pthread_t writers[writer_count];
    
    printf("\033[0;37mStarting %d writers and %d readers...\n", writer_count, reader_count);
    
    for (long i = 0; i < writer_count; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }
    
    for (long i = 0; i < reader_count; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }
    
    for (int i = 0; i < writer_count; i++) {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < reader_count; i++) {
        pthread_join(readers[i], NULL);
    }
    
    sem_destroy(&writer_lock);
    sem_destroy(&reader_lock);
    
    return 0;
}

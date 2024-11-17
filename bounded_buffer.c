#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 100
int buffer[BUFFER_SIZE]= {0};
int writer_i = 0;
int reader_i = 0;

sem_t writer_lock;
sem_t reader_lock;

void *writer(void* args) {
  while (1) {
    sem_wait(&writer_lock);
    if (buffer[writer_i%BUFFER_SIZE] != -1) {
      sem_post(&writer_lock);
      continue;
    }
    int temp = writer_i++;
    printf("Producer: ");
    print_arr(buffer, BUFFER_SIZE);
    sem_post(&writer_lock);

    buffer[temp%BUFFER_SIZE] = temp;
    sleep(1);
  } 
}

void *reader(void* args) {
  while(1) {
    sem_wait(&reader_lock);
    if (buffer[reader_i%BUFFER_SIZE] == -1) { 
      sem_post(&reader_lock);
      continue;
    }
    int temp = reader_i++;
    printf("Consumer: ");
    print_arr(buffer, BUFFER_SIZE);
    sem_post(&reader_lock);

    buffer[temp%BUFFER_SIZE] = -1;
    sleep(2);
  }
}

void print_arr(int arr[], int size) {
  printf("[");
  for (int i = 0; i < size; i++) {
    printf("%i ", arr[i]);
  }
  printf("]\n");
}

int main() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = -1;
  }

  sem_init(&writer_lock, 0, 1);
  sem_init(&reader_lock, 0, 1);
  
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

  sem_destroy(&writer_lock);
  sem_destroy(&reader_lock);
}

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>

#define BUFFER_SIZE 1024

sem_t producer_sem;
sem_t consumer_sem;

// Pipe file descriptors
int pipe_fd[2];

void* writer_thread(void* arg) {
    while(1) {
      sem_wait(&producer_sem);
      const char* message = "Hello from writer thread!";
      
      write(pipe_fd[1], message, strlen(message) + 1);
      printf("Writer thread: Message sent: %s\n", message);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    while(1) {
      sem_wait(&consumer_sem);
      char buffer[BUFFER_SIZE];
      
      read(pipe_fd[0], buffer, BUFFER_SIZE);
      printf("Reader thread: Message received: %s\n", buffer);
      sem_post(&producer_sem);
      sem_post(&consumer_sem);
    } 
    return NULL;
}

int main() {
    // Create an unnamed pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    sem_init(&producer_sem, 0, 3);
    sem_init(&consumer_sem, 0, 5);

    // Create writer and reader threads
    pthread_t writer, reader;
    
    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        perror("Failed to create writer thread");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&reader, NULL, reader_thread, NULL) != 0) {
        perror("Failed to create reader thread");
        exit(EXIT_FAILURE);
    }

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return 0;
}


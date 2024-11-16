#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>

sem_t producer_sem;
sem_t consumer_sem;
// Pipe file descriptors
int pipe_fd[2];

int get_sem_value(sem_t *sem) {
    // Function to get current semaphore value
    int val;
    sem_getvalue(sem, &val);
    return val;
}

void* writer_thread(void* arg) {
    while(1) {
        sem_wait(&producer_sem);
        
        int input_number = rand() % 101;
        write(pipe_fd[1], &input_number, sizeof(input_number));
        
        printf("Writer thread [Producer sem ID: P%d]: Number sent: %d\n", 
               get_sem_value(&producer_sem), input_number);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    while(1) {
        sem_wait(&consumer_sem);
        
        int output_number;
        read(pipe_fd[0], &output_number, sizeof(output_number));
        
        printf("Reader thread [Consumer sem ID: C%d]: Number received: %d\n", 
               get_sem_value(&consumer_sem), output_number);
        
        // Post to both semaphores and show their new values
        sem_post(&producer_sem);
        printf("Released producer_sem (new ID: P%d)\n", get_sem_value(&producer_sem));
        
        sem_post(&consumer_sem);
        printf("Released consumer_sem (new ID: C%d)\n", get_sem_value(&consumer_sem));
    } 
    return NULL;
}

int main() {
    srand(time(NULL));
    
    // Create an unnamed pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    sem_init(&producer_sem, 0, 3);
    sem_init(&consumer_sem, 0, 5);
    printf("Initial producer_sem value (ID: P%d)\n", get_sem_value(&producer_sem));
    printf("Initial consumer_sem value (ID: C%d)\n", get_sem_value(&consumer_sem));
    
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
    sem_destroy(&producer_sem);
    sem_destroy(&consumer_sem);
    return 0;
}

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
int pipe_fd[2];

// Function to get current semaphore value
int get_sem_value(sem_t *sem) {
    int val;
    sem_getvalue(sem, &val);
    return val;
}

void random_delay(int min_ms, int max_ms) {
    int delay = min_ms + rand() % (max_ms - min_ms + 1);
    usleep(delay * 1000); // Convert to microseconds
}

void* writer_thread(void* arg) {
    while(1) {
        int sem_id = get_sem_value(&producer_sem);
        sem_wait(&producer_sem);
        
        random_delay(100, 500);  // 100-500ms delay
        
        int input_number = rand() % 101;
        write(pipe_fd[1], &input_number, sizeof(input_number));
        
        printf("Writer thread [Producer sem ID: P%d]: Number sent: %d\n", 
               sem_id, input_number);
               
        random_delay(50, 200);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    while(1) {
        int sem_id = get_sem_value(&consumer_sem);
        sem_wait(&consumer_sem);
        
        random_delay(150, 400);  // 150-400ms delay
        
        int output_number;
        read(pipe_fd[0], &output_number, sizeof(output_number));
        
        printf("Reader thread [Consumer sem ID: C%d]: Number received: %d\n", 
               sem_id, output_number);
        
        sem_post(&producer_sem);
        random_delay(50, 150);
        sem_post(&consumer_sem);
        random_delay(50, 200);
    } 
    return NULL;
}

int main() {
    srand(time(NULL));
    
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    sem_init(&producer_sem, 0, 3);
    sem_init(&consumer_sem, 0, 5);
    
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

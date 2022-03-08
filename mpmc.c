#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int producers, consumers, tasks, buffersize;

int *buffer;
int front = 0, rear = 0;

sem_t mutex, empty, exist;
sem_t fill, get;

int data = 0, count_fill = 0, count_get = 0;

void do_fill(int value) {
    buffer[rear] = value;
    rear = (rear + 1) % buffersize;
}

int do_get() {
    int value = buffer[front];
    front = (front + 1) % buffersize;
    return value;
}

void* producer(void *args) {
    long int tid = (long int)args;
    int tmp = 0;
    while(1) {
        sem_wait(&fill);
        tmp = count_fill++;
        sem_post(&fill);
        
        if(tmp >= tasks) break;
        sem_wait(&empty);
        sem_wait(&mutex);
        int value = data++;
        do_fill(value);
        printf("%02ld: append %d\n", tid, value);
        sem_post(&mutex);
        sem_post(&exist);
    }

    return NULL;
}

void* consumer(void *args) {
    long int tid = (long int)args;
    int tmp = 0;
    while(1) {
        sem_wait(&get);
        tmp = count_get++;
        sem_post(&get);

        if(tmp >= tasks) break;
        sem_wait(&exist);
        sem_wait(&mutex);
        int value = do_get();
        printf("%02ld: pop %d\n", tid, value);
        sem_post(&mutex);
        sem_post(&empty);
    }

    return NULL;
}

int main(int argc, char* argv[]) {

    if(argc != 5) {
    	fprintf(stderr, "usage: %s <producers> <consumers> <tasks> <buffersize>\n", argv[0]);
	exit(1);
    }
    
    producers = atoi(argv[1]);
    consumers = atoi(argv[2]);
    tasks = atoi(argv[3]);
    buffersize = atoi(argv[4]);

    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, buffersize);
    sem_init(&exist, 0, 0);

    sem_init(&fill, 0, 1);
    sem_init(&get, 0, 1);

    buffer = (int *)malloc(sizeof(int) * buffersize);
    int thread_amount = producers + consumers;
    pthread_t thread[thread_amount];

    for(int i = 0; i < thread_amount; i++) {
        if(i < producers) {
            pthread_create(&thread[i], NULL, &producer, (void*)(long int)i);
        } else {
            pthread_create(&thread[i], NULL, &consumer, (void*)(long int)i);
        }
    }

    for(int i = 0; i < thread_amount; i++) {
        pthread_join(thread[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&exist);

    sem_destroy(&fill);
    sem_destroy(&get);

    printf("main: end\n");
    return 0;
}

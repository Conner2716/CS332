#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define PRODUCER_THREADS 10
#define CONSUMER_THREADS 20
#define NUMBERS_PER_PRODUCER 500
#define NUMBERS_PER_CONSUMER 250
#define RANDOM_LIMIT 1000

int pipefd[2];
long consumer_sums[CONSUMER_THREADS];
sem_t pipe_semaphore;
pthread_mutex_t random_mutex;
pthread_mutex_t print_mutex;

static void *producer(void *arg) {
    int thread_id = *(int *)arg;
    int used[RANDOM_LIMIT + 1] = {0};
    int number;
    int count = 0;
    ssize_t bytes_written;

    while (count < NUMBERS_PER_PRODUCER) {
        pthread_mutex_lock(&random_mutex);
        number = rand() % (RANDOM_LIMIT + 1);
        pthread_mutex_unlock(&random_mutex);

        if (used[number] == 0) {
            used[number] = 1;

            sem_wait(&pipe_semaphore);
            bytes_written = write(pipefd[1], &number, sizeof(number));
            sem_post(&pipe_semaphore);

            if (bytes_written != (ssize_t)sizeof(number)) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            count++;

            if (count % 100 == 0) {
                pthread_mutex_lock(&print_mutex);
                if (count == NUMBERS_PER_PRODUCER) {
                    fprintf(stderr, "Producer thread %d completed: %d/%d (100%%)\n",
                            thread_id, count, NUMBERS_PER_PRODUCER);
                } else {
                    fprintf(stderr, "Producer thread %d progress: %d/%d (%d%%)\n",
                            thread_id, count, NUMBERS_PER_PRODUCER,
                            (count * 100) / NUMBERS_PER_PRODUCER);
                }
                pthread_mutex_unlock(&print_mutex);
            }
        }
    }

    return NULL;
}

static void *consumer(void *arg) {
    int thread_id = *(int *)arg;
    int number;
    int count;
    long sum = 0;
    ssize_t bytes_read;

    for (count = 0; count < NUMBERS_PER_CONSUMER; count++) {
        sem_wait(&pipe_semaphore);
        bytes_read = read(pipefd[0], &number, sizeof(number));
        sem_post(&pipe_semaphore);

        if (bytes_read != (ssize_t)sizeof(number)) {
            if (bytes_read == 0) {
                fprintf(stderr, "Consumer thread %d reached the end of the pipe early\n",
                        thread_id);
            } else {
                perror("read");
            }
            exit(EXIT_FAILURE);
        }

        sum += number;
    }

    consumer_sums[thread_id - 1] = sum;

    pthread_mutex_lock(&print_mutex);
    fprintf(stderr, "Consumer thread %d completed with sum %ld\n", thread_id, sum);
    pthread_mutex_unlock(&print_mutex);

    return NULL;
}

int main(void) {
    pid_t pid;
    pthread_t producers[PRODUCER_THREADS];
    pthread_t consumers[CONSUMER_THREADS];
    int producer_ids[PRODUCER_THREADS];
    int consumer_ids[CONSUMER_THREADS];
    int i;
    long total = 0;
    double average;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        if (sem_init(&pipe_semaphore, 0, 1) == -1) {
            perror("sem_init");
            exit(EXIT_FAILURE);
        }

        if (pthread_mutex_init(&print_mutex, NULL) != 0) {
            fprintf(stderr, "Unable to initialize child print mutex\n");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < CONSUMER_THREADS; i++) {
            consumer_ids[i] = i + 1;
            if (pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]) != 0) {
                fprintf(stderr, "Unable to create consumer thread %d\n", i + 1);
                exit(EXIT_FAILURE);
            }
        }

        for (i = 0; i < CONSUMER_THREADS; i++) {
            if (pthread_join(consumers[i], NULL) != 0) {
                fprintf(stderr, "Unable to join consumer thread %d\n", i + 1);
                exit(EXIT_FAILURE);
            }
        }

        for (i = 0; i < CONSUMER_THREADS; i++) {
            total += consumer_sums[i];
        }

        average = (double)total / CONSUMER_THREADS;
        printf("Average of the consumer thread sums: %.2f\n", average);

        close(pipefd[0]);
        sem_destroy(&pipe_semaphore);
        pthread_mutex_destroy(&print_mutex);
        exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);
    srand((unsigned int)getpid());

    if (sem_init(&pipe_semaphore, 0, 1) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&random_mutex, NULL) != 0) {
        fprintf(stderr, "Unable to initialize random-number mutex\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&print_mutex, NULL) != 0) {
        fprintf(stderr, "Unable to initialize parent print mutex\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < PRODUCER_THREADS; i++) {
        producer_ids[i] = i + 1;
        if (pthread_create(&producers[i], NULL, producer, &producer_ids[i]) != 0) {
            fprintf(stderr, "Unable to create producer thread %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < PRODUCER_THREADS; i++) {
        if (pthread_join(producers[i], NULL) != 0) {
            fprintf(stderr, "Unable to join producer thread %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    close(pipefd[1]);
    wait(NULL);

    sem_destroy(&pipe_semaphore);
    pthread_mutex_destroy(&random_mutex);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}

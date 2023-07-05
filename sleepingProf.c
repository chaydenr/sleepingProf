#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define STUDENT_COUNT_MIN 1
#define STUDENT_COUNT_MAX 10
#define HELPS_MAX 5

// globals
sem_t professor_sleeping;
sem_t student_waiting;
int chair_count;
sem_t* student_chairs;
pthread_t* student_threads;
int* student_help_counts;
pthread_mutex_t mutex;

void* studentsThd(void* arg) {

}

void* professorThd(void* arg) {

}

int main() {

    return 0;
}

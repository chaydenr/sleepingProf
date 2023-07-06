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
    printf("thread created");
}

void* professorThd(void* arg) {

}

int main() {
    printf("CS 370 - Sleeping Professor Project");

    // have user enter student count, check for valid entry
    int student_count;
        do {
        printf("How many students coming to professor's office? ");
        scanf("%d", &student_count);
    } while (student_count < STUDENT_COUNT_MIN || student_count > STUDENT_COUNT_MAX);

    // initialize student help count array
    student_help_counts = (int*)malloc(sizeof(int) * student_count);
    for (int i = 0; i < student_count; i++) {
        student_help_counts[i] = rand() % HELPS_MAX + 1; //set each student's help count
    }

    // initialize student threads array
    student_threads = (pthread_t*)malloc(sizeof(pthread_t) * student_count);

    // initialize professor and student semaphores, set to 0
    sem_init(&professor_sleeping, 0, 0);
    sem_init(&student_waiting, 0, 0);

    // initialize 3 spaces in student chairs array, set all to 0
    sem_t student_chairs[3];
    for (int i = 0; i < 3; i++) {
        sem_init(&student_chairs[i], 0, 0);
    }
    // student_chairs = (sem_t*)malloc(sizeof(sem_t) * student_count);
    // for (int i = 0; i < student_count; i++) {
    //     sem_init(&student_chairs[i], 0, 0);
    // }

    // initialize mutex for shared variables
    pthread_mutex_init(&mutex, NULL);

    // initialize one professor thread
    pthread_t professor_thread;
    pthread_create(&professor_thread, NULL, professorThd, NULL);

    // initialize student student threads
    int student_ids[student_count];
    for (int i = 0; i < student_count; i++) {
        student_ids[i] = i; // set student IDs
        pthread_create(&student_threads[i], NULL, studentsThd, &student_ids[i]); // create student threads
    }

    return 0;
}

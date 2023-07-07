#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define STUDENT_COUNT_MIN 1
#define STUDENT_COUNT_MAX 10
#define CHAIR_COUNT 3
#define HELPS_MAX 3

// globals
sem_t professor_sleeping;
sem_t student_waiting;
int chair_index = 0;
int chair_left = 0;
pthread_t* student_threads;
int* student_help_counts;
pthread_mutex_t mutex_chair;
pthread_mutex_t mutex_chair_index;
pthread_mutex_t mutex_help_counts;
sem_t student_chairs[CHAIR_COUNT];

void* studentsThd(void* arg) {
    int student_id = *(int*)arg;

    while (1) {
        printf("\033[0;93mStudent %d doing assignment.\033[0m\n", student_id);

        // student works on the assignment for a random amount of time
        usleep(rand() % 2000000);

        printf("\033[0;31mStudent %d needs help from the professor.\033[0m\n", student_id);

        // get chair count
        pthread_mutex_lock(&mutex_chair_index);
        int current_chair_count = chair_index;
        pthread_mutex_unlock(&mutex_chair_index);

        // check if chairs available
        if (current_chair_count >= 3) {
            // awaken the sleeping professor
            printf("\033[0;31mChairs occupied, student %d will return later.\033[0m\n", student_id);
            continue;
        } else {
            if (current_chair_count == 0) {
                // all chairs are available, wake professor
                sem_post(&professor_sleeping);
            }

            // occupy a chair
            // update chair count
            pthread_mutex_lock(&mutex_chair_index);
            pthread_mutex_lock(&mutex_chair);
            chair_index++;
            pthread_mutex_unlock(&mutex_chair_index);
            int j = chair_left;
            chair_left = (chair_left + 1) % CHAIR_COUNT; // get index
            pthread_mutex_unlock(&mutex_chair);

            // wait until professor is available to help
            sem_wait(&student_chairs[j]);

            // Professor helps the student
            printf("\033[0;31mStudent %d is getting help from the professor.\033[0m\n", student_id);

            // wait until professor is done helping
            sem_wait(&student_waiting);

            // decrement the help count for that student
            pthread_mutex_lock(&mutex_help_counts);
            student_help_counts[student_id - 1]--;
            pthread_mutex_unlock(&mutex_help_counts);

            if (student_help_counts[student_id - 1] == 0) {
                // Student's help count is 0, exit thread
                return NULL;
            }

            // student done receiving help
        }
    }
}

void* professorThd(void* arg) {
    while (1) {
        // wait until awakened by a student
        sem_wait(&professor_sleeping);
        printf("\033[0;92mProfessor is awakened by a student.\033[0m\n");

        // loop to help all waiting students
        while (1) {
            pthread_mutex_lock(&mutex_chair);
            // if chairs empty, break
            if (chair_index == 0) {
                pthread_mutex_unlock(&mutex_chair);
                break;
            }

            // student frees hallway chair and enters office
            pthread_mutex_lock(&mutex_chair_index);
            sem_post(&student_chairs[chair_index - 1]);
            chair_index--; 
            pthread_mutex_unlock(&mutex_chair_index);
            printf("\033[0;92mStudent frees chair and enters professor's office. Remaining chairs: %d\033[0m\n", 3 - chair_index);
            // chair_left++; // check logic
            chair_left = (chair_left + 1) % CHAIR_COUNT;
            pthread_mutex_unlock(&mutex_chair);

            // professor helping the student for random time
            printf("\033[0;92mProfessor is helping a student.\033[0m\n");
            usleep(rand() % 1500000);

            // signal that next student can enter
            sem_post(&student_waiting);
        }

        // check if all students have been helped
        int student_helps_check = 1;
        pthread_mutex_lock(&mutex_help_counts);
        for (int i = 0; i < STUDENT_COUNT_MAX; i++) {
            if (student_help_counts[i] > 0) {
                student_helps_check = 0;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_help_counts);

        if (student_helps_check) {
            // help check == 1, all students have been helped
            printf("\033[0;94mAll students assisted, professor is leaving.\033[0m\n");
            return NULL;
        }
    }
}

int main() {
    printf("CS 370 - Sleeping Professor Project\n");

    // have user enter student count, check for valid entry
    int student_count;
        do {
        printf("How many students coming to professor's office? \n");
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
    for (int i = 0; i < 3; i++) {
        sem_init(&student_chairs[i], 0, 0);
    }

    // initialize mutex for shared variables
    pthread_mutex_init(&mutex_chair, NULL);
    pthread_mutex_init(&mutex_chair_index, NULL);
    pthread_mutex_init(&mutex_help_counts, NULL);

    // initialize one professor thread
    pthread_t professor_thread;
    pthread_create(&professor_thread, NULL, professorThd, NULL);

    // initialize student count threads
    int student_ids[student_count];
    for (int i = 0; i < student_count; i++) {
        student_ids[i] = i; // set student IDs
        pthread_create(&student_threads[i], NULL, studentsThd, &student_ids[i]); // create student threads
    }

    // wait for appropriate threads to complete
    pthread_join(professor_thread, NULL);
    for (int i = 0; i < student_count; i++) {
        pthread_join(student_threads[i], NULL);
    }

    // free malloc memory
    return 0;
}

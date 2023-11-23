#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CARS 100

sem_t bridge_sema;     // Semaphore for controlling access to the bridge
sem_t mutex;           // Mutex for critical sections
int num_cars_on_bridge = 0;  // Number of cars currently on the bridge

void passing(int dir, int car_id) {
    if (dir == 0) {
        printf("Left car %d is crossing the bridge.\n", car_id);
    } else {
        printf("Right car %d is crossing the bridge.\n", car_id);
    }
    sleep(1);  // Simulate the time it takes to cross the bridge
}

void* left(void* args) {
    int car_id = *((int*)args);

    while (1) {
        sem_wait(&mutex);
        printf("Left car %d is waiting to cross.\n", car_id);
        sem_post(&mutex);

        sem_wait(&bridge_sema);

        sem_wait(&mutex);
        printf("Left car %d is on the bridge.\n", car_id);
        num_cars_on_bridge++;
        passing(0, car_id);
        num_cars_on_bridge--;
        printf("Left car %d has crossed the bridge.\n", car_id);
        sem_post(&mutex);

        sem_post(&bridge_sema);

        sleep(1);  // Simulate some delay before the next car
    }
}

void* right(void* args) {
    int car_id = *((int*)args);

    while (1) {
        sem_wait(&mutex);
        printf("Right car %d is waiting to cross.\n", car_id);
        sem_post(&mutex);

        sem_wait(&bridge_sema);

        sem_wait(&mutex);
        printf("Right car %d is on the bridge.\n", car_id);
        num_cars_on_bridge++;
        passing(1, car_id);
        num_cars_on_bridge--;
        printf("Right car %d has crossed the bridge.\n", car_id);
        sem_post(&mutex);

        sem_post(&bridge_sema);

        sleep(1);  // Simulate some delay before the next car
    }
}

int main() {
    int total_left_cars, total_right_cars;

    printf("Enter the number of cars on the left: ");
    scanf("%d", &total_left_cars);

    printf("Enter the number of cars on the right: ");
    scanf("%d", &total_right_cars);

    if (total_left_cars <= 0 || total_right_cars <= 0 || total_left_cars + total_right_cars > MAX_CARS) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    sem_init(&bridge_sema, 0, 5);
    sem_init(&mutex, 0, 1);

    pthread_t left_threads[total_left_cars], right_threads[total_right_cars];
    int left_ids[total_left_cars], right_ids[total_right_cars];

    for (int i = 0; i < total_left_cars; ++i) {
        left_ids[i] = i + 1;
        pthread_create(&left_threads[i], NULL, left, (void*)&left_ids[i]);
    }

    for (int i = 0; i < total_right_cars; ++i) {
        right_ids[i] = i + 1;
        pthread_create(&right_threads[i], NULL, right, (void*)&right_ids[i]);
    }

    for (int i = 0; i < total_left_cars; ++i) {
        pthread_join(left_threads[i], NULL);
    }

    for (int i = 0; i < total_right_cars; ++i) {
        pthread_join(right_threads[i], NULL);
    }

    return 0;
}

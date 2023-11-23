#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PASSENGERS 100

int total_passengers;
int car_capacity;

sem_t car_sema;          // Semaphore for car
sem_t passenger_sema;    // Semaphore for passengers
sem_t mutex;             // Mutex for critical sections

int passengers_on_board = 0;

void load() {
    printf("Car is loading passengers...\n");
    sleep(2);
    printf("Car is fully loaded and ready to go!\n");
}

void unload() {
    printf("Car is unloading passengers...\n");
    sleep(2);
    printf("All passengers have disembarked.\n");
}

void board(int passenger_id) {
    printf("Passenger %d is boarding.\n", passenger_id);
    sleep(1);
    printf("Passenger %d has boarded.\n", passenger_id);
}

void offboard(int passenger_id) {
    printf("Passenger %d is getting off the car.\n", passenger_id);
    sleep(1);
    printf("Passenger %d has gotten off the car.\n", passenger_id);
}

void* car(void* args) {
    while (1) {
        sem_wait(&car_sema);  // Wait until passengers are ready to board
        load();
        sem_post(&passenger_sema);  // Signal passengers that car is loaded
        sem_wait(&car_sema);  // Wait until all passengers have disembarked
        unload();
    }
}

void* passenger(void* args) {
    int passenger_id = *((int*)args);

    sem_wait(&mutex);
    printf("Passenger %d is waiting to board.\n", passenger_id);
    sem_post(&mutex);

    sem_post(&car_sema);  // Signal that passenger is ready to board
    sem_wait(&passenger_sema);  // Wait for the car to be fully loaded

    sem_wait(&mutex);
    board(passenger_id);
    sem_post(&mutex);

    sleep(3);  // Simulate ride duration

    sem_wait(&mutex);
    offboard(passenger_id);
    sem_post(&mutex);

    sem_post(&car_sema);  // Signal that passenger has disembarked

    pthread_exit(NULL);
}

int main() {
    printf("Enter the total number of passengers: ");
    scanf("%d", &total_passengers);

    printf("Enter the car capacity: ");
    scanf("%d", &car_capacity);

    if (car_capacity <= 0 || total_passengers <= 0 || car_capacity > MAX_PASSENGERS) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    sem_init(&car_sema, 0, 0);
    sem_init(&passenger_sema, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_t car_thread, passenger_threads[MAX_PASSENGERS];
    int passenger_ids[MAX_PASSENGERS];

    pthread_create(&car_thread, NULL, car, NULL);

    for (int i = 0; i < total_passengers; ++i) {
        passenger_ids[i] = i + 1;
        pthread_create(&passenger_threads[i], NULL, passenger, (void*)&passenger_ids[i]);
    }

    for (int i = 0; i < total_passengers; ++i) {
        pthread_join(passenger_threads[i], NULL);
    }

    return 0;
}

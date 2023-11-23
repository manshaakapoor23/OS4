#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>

sem_t forks[5];
sem_t bowl;

void EAT() {
    sleep(5);
}
void THINK() {
    sleep(2);
}
void* start(void* pos) {

    int p = *(int *)pos;
    printf("Philosopher %d is thinking\n", p);
    THINK();
    int t;
    if (p != 4) {
        sem_wait(&forks[p % 5]);
        printf("Philosopher %d has picked up left fork\n", p);
        sem_wait(&forks[(p + 1) % 5]);
        printf("Philosopher %d has picked up right fork\n", p);
	sem_getvalue(&bowl, &t);
        if (t == 1) {
            sem_wait(&bowl);
            printf("Philosopher %d has picked up the first bowl\n", p);
            printf("Philosopher %d is eating\n", p);
            EAT();
            sem_post(&bowl);
            printf("Philosopher %d has put down the bowl\n", p);
            sem_post(&forks[p % 5]);
            printf("Philosopher %d has put down left fork\n",p);
            sem_post(&forks[(p + 1) % 5]);
            printf("Philosopher %d has put down right fork\n", p);
            printf("Philosopher %d is done eating\n", p);
        }  else {
            printf("Philosopher %d has picked up the second bowl\n", p);
            printf("Philosopher %d is eating\n", p);
            EAT();
            printf("Philosopher %d has put down the bowl\n", p);
            printf("Philosopher %d is done eating\n", p);
            sem_post(&forks[(p + 1) % 5]);
            printf("Philosopher %d has put down right fork\n", p);
            sem_post(&forks[p % 5]);
            printf("Philosopher %d has put down left fork\n",p);
        }

    } else {
        sem_wait(&forks[(p + 1) % 5]);
        printf("Philosopher %d has picked up right fork\n", p);
        sem_wait(&forks[p % 5]);
        printf("Philosopher %d has picked up left fork\n", p);
	sem_getvalue(&bowl, &t);
        if (t == 1) {
            sem_wait(&bowl);
            printf("Philosopher %d has picked up the first bowl\n", p);
            printf("Philosopher %d is eating\n", p);
            EAT();
            printf("Philosopher %d is done eating\n", p);
            sem_post(&bowl);
            printf("Philosopher %d has put down the bowl\n", p);
            sem_post(&forks[(p + 1) % 5]);
            printf("Philosopher %d has put down right fork\n", p);
            sem_post(&forks[p % 5]);
            printf("Philosopher %d has put down left fork\n",p);
        }  else {
            printf("Philosopher %d has picked up the second bowl\n", p);
            printf("Philosopher %d is eating\n", p);
            EAT();
            printf("Philosopher %d is done eating\n", p);
            sem_post(&forks[(p + 1) % 5]);
            printf("Philosopher %d has put down right fork\n", p);
            sem_post(&forks[p % 5]);
            printf("Philosopher %d has put down left fork\n",p);
            printf("Philosopher %d has put down the bowl\n", p);
        }
    }
    return NULL;

}



int main() {

    int ind[5];
    for (int i = 0; i < 5; i++) {
        sem_init(&forks[i], 0, 1);
    }
    sem_init(&bowl, 0, 1);

    pthread_t threads[5];
    for (int i = 0; i < 5; i++) {
        ind[i] = i;
        pthread_create(&threads[i], NULL, &start, (void*)&ind[i]);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < 5; i++) {
        sem_destroy(&forks[i]);
    }
    sem_destroy(&bowl);
}

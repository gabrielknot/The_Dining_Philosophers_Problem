#include <pthread.h> 
#include <stdlib.h>
#include <stdio.h>
#include<semaphore.h>

#define NTHRDS 5

int sharedData = 0; 
typedef struct{
	int id;
	sem_t *leftFork;
 	sem_t * rightFork;
}philosopher;

sem_t forks[NTHRDS];

void delay(int secs) { //utility function
  time_t beg = time(NULL), end = beg + secs;
  do ; while (time(NULL) < end);
}

void thinking(philosopher*philo){
  printf("#%d, be thinking: Do you feel what I feel, see what I see, hear what I hear...\n",philo->id);
  delay(rand() % 2);
}

//Try commenting out the calls to the lock and unlock functions.
void *eat(void *philo) {
 //Obtain exclusive mutex lock
  philosopher *philo_cast = (philosopher*)philo;
  int valueLeftFork,valueRightFork;
  sem_getvalue(philo_cast->leftFork,&valueLeftFork);
  sem_getvalue(philo_cast->rightFork,&valueRightFork);
  if(valueLeftFork==0||valueRightFork==0){
    thinking(philo_cast);
    sem_getvalue(philo_cast->leftFork,&valueLeftFork);
    sem_getvalue(philo_cast->rightFork,&valueRightFork);
  };
  
  sem_wait(philo_cast->leftFork);
  sem_wait(philo_cast->rightFork);
  printf("\nTHE PHILOSOPHER #%d eating...\n",philo_cast->id);
  delay(rand() % 6);
  printf("\nTHE PHILOSOPHER #%d finish eating...\n",philo_cast->id);
  delay(rand() % 3);
  sem_post(philo_cast->rightFork);
  sem_post(philo_cast->leftFork);
  sem_getvalue(philo_cast->leftFork,&valueLeftFork);
  sem_getvalue(philo_cast->rightFork,&valueRightFork);
  if(valueLeftFork==0||valueRightFork==0){
    
    thinking(philo_cast);

    sem_getvalue(philo_cast->leftFork,&valueLeftFork);
    sem_getvalue(philo_cast->rightFork,&valueRightFork);
  };

  sem_getvalue(philo_cast->leftFork,&valueLeftFork);
  sem_getvalue(philo_cast->rightFork,&valueRightFork);
  
  return philo;
}

int main() {
philosopher* philosophers = (philosopher *) malloc(sizeof(philosopher)*NTHRDS);
  pthread_attr_t attr;
  pthread_attr_init(&attr); //initialise thread attribute data ...
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

  pthread_t philosophers_thrds[NTHRDS]; //Data strs form managing several threads

  int t;

  srand(time(NULL)); // initialise random num generator

  for (t=0; t<NTHRDS; t++) //initialise the semaphores
    sem_init(&forks[t], 0, 1);

  for (t=0; t<NTHRDS; t++) {//create & initialise several thread instances
    philosophers[t].id = t;
    philosophers[t].leftFork = &forks[t];
    philosophers[t].rightFork = &forks[(t+1)%NTHRDS];
    printf("Creating Philosophers %d\n", t);
    pthread_create(&philosophers_thrds[t], &attr, eat, &philosophers[t]);
  }

  for (t=0; t<NTHRDS; t++) //initialise the semaphores
    pthread_join(philosophers_thrds[t], NULL);
 
}
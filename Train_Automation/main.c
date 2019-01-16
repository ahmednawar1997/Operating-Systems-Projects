#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <windows.h>

#define MAX_SEATS 10
#define MAX_PASSENGERS 205
struct Train
{
    int passengersOnBoard;
    int id;
};

struct Station
{
    struct Train train;
    int passengersWaiting;
    int totalBoardedPassengers;
    int passengerNumber;
};


sem_t waitingPassengersSemaphore;
sem_t passengersOnBoardSemaphore;
sem_t passengersTryingToBoardSemaphore;
sem_t train;
pthread_mutex_t lock;
pthread_mutex_t mutex;
pthread_cond_t newTrain;


void *station_load_train(void *st)
{
    struct Station *station = st;
    printf("Train |%d| has arrived at the station\n",station->train.id);

    while(1){

        sem_wait(&waitingPassengersSemaphore);//to block thread if no passengers waiting
        sem_wait(&passengersTryingToBoardSemaphore);//maximum 10 passengers try to board
        sem_wait(&passengersOnBoardSemaphore);//only 10 passengers can enter this function
        sem_post(&train);
        station_on_board(station);
        sem_post(&passengersOnBoardSemaphore);
    }
}

void *station_wait_for_train(void *st)
{
    struct Station *station = st;
    int passengerNumber = ++station->passengerNumber;
    sem_post(&waitingPassengersSemaphore);
    printf("New passenger %d has arrived at the station\n",passengerNumber);

    sem_wait(&train);//Block passenger if no train at the station
    printf("Passenger %d boarded\n", passengerNumber);

}

void *generateTrain(void *st){
    struct Station *station = st;
    pthread_t trainThread;

    pthread_mutex_lock(&mutex);
    while(1){
        pthread_cond_wait(&newTrain,&mutex);
        Sleep(rand() % 50);
        station->train.passengersOnBoard = 0;
        station->train.id++;

        sem_init(&passengersTryingToBoardSemaphore, 0, MAX_SEATS);
        pthread_create(&trainThread, NULL, station_load_train, (void*)station);

    }
    pthread_mutex_unlock(&mutex);

    pthread_join(trainThread,NULL);

}


void *generateTrainFirstTime(void *st){
    struct Station *station = st;
    Sleep(rand() % 50);

    station->train.passengersOnBoard = 0;
    station->train.id++;

    pthread_t trainThread;
    pthread_create(&trainThread, NULL, station_load_train, (void*)station);
    pthread_join(trainThread,NULL);

}

void station_on_board(struct Station *station)
{

    station->train.passengersOnBoard++;
    station->totalBoardedPassengers++;


    if(station->train.passengersOnBoard == MAX_SEATS)
    {

        printf("Train is full!\nThe train has departed from the station\n");

        pthread_cond_signal(&newTrain);
        //notify condition variable that this train has departed and to generate a new train
        sem_post(&passengersOnBoardSemaphore);
        pthread_exit(3);
    }

}

void station_init(struct Station *station)
{
    sem_init(&waitingPassengersSemaphore, 0, 0);
    sem_init(&passengersOnBoardSemaphore, 0, MAX_SEATS);
    sem_init(&passengersTryingToBoardSemaphore, 0, MAX_SEATS);
    sem_init(&train, 0, 0);
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&newTrain, NULL);


    station->passengersWaiting = 0;
    station->train.passengersOnBoard = 0;
    station->totalBoardedPassengers = 0;
    station->train.id = 0;
    station->passengerNumber = 0;

}


void *generateRandomPassengers(void *st){
    struct Station *station = st;
    int i;
    pthread_t passengerThreads[MAX_PASSENGERS];

    for(i = 0; i < MAX_PASSENGERS; i++){
        pthread_create(&passengerThreads[i], NULL, station_wait_for_train, (void*)station);
        Sleep(rand() % 20);
    }

    for(i = 0; i < MAX_PASSENGERS; i++){
        pthread_join(passengerThreads[i],NULL);
    }
}

int main()
{
    struct Station station;
    station_init(&station);

    pthread_t generateTrainT;
    pthread_create(&generateTrainT,NULL,generateTrainFirstTime,(void*)&station);


    pthread_t randomPassengersT;
    pthread_create(&randomPassengersT,NULL,generateRandomPassengers,(void*)&station);


    pthread_t randomTrainsT;
    pthread_create(&randomTrainsT,NULL,generateTrain,(void*)&station);



    pthread_join(generateTrainT, NULL);
    pthread_join(randomPassengersT, NULL);


    return 0;
}

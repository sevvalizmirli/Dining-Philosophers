#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<signal.h>



#define EATING 0
#define HUNGRY 1
#define THINKING 2
#define WANT_TO_SING 3
#define SINGING 4

int phi_number;


#define LEFT (p_number + (phi_number-1)) % phi_number
#define RIGHT (p_number + 1) % phi_number

int* state=NULL;
int* phil=NULL;
sem_t mutex;
sem_t mutex2;
sem_t* S;
sem_t* sing_;

int **print;        //for every phi --> thinking_number ,  eating_number , singing_number;

int temp=0;
//int *bastir=NULL;



void test(int p_number)
{
    if (state[p_number] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {

        state[p_number] = EATING;
        print[p_number][1]++;


        sleep(0.1);
        printf("Philosopher %d is Eating\n", p_number + 1);
         sem_post(&S[p_number]);
    }
}


void pickup(int p_number)
{

    sem_wait(&mutex);

    state[p_number] = HUNGRY;

    printf("Philosopher %d is Hungry\n", p_number + 1);

    test(p_number);

    sem_post(&mutex);

    sem_wait(&S[p_number]);

    sleep(0.1);
}

void putdown(int p_number)
{

    sem_wait(&mutex);

    state[p_number] = THINKING;
    print[p_number][0]++;

    printf("Philosopher %d is thinking\n", p_number + 1);

    test(LEFT);
    test(RIGHT);

    sem_post(&mutex);
}

void singing_test(int p_number)
{
    int flag=0;  //sadece 1 kişinin söylemesini garanti ediyor.
    for(int i=0;i<phi_number;i++){
        if(state[i]==SINGING)
            flag=1;
    }
    if (state[p_number] == WANT_TO_SING && flag==0) {

        state[p_number] = SINGING;
        print[p_number][2]++;

        sleep(0.1);
        printf("Philosopher %d is Singing\n", p_number + 1);
       sem_post(&sing_[p_number]);       //******************************************************************************************
    }

}


void sing(int p_number)
{
    sem_wait(&mutex2);

    // state that hungry
    state[p_number] = WANT_TO_SING;
    print[p_number][3]++;
    printf("Philosopher %d  Wants to Sing\n", p_number + 1);
    singing_test(p_number);

    sem_post(&mutex2);
    sem_wait(&sing_[p_number]);



    sleep(0.1);

}


void stopsinging(int p_number){
    sem_wait(&mutex2);
    state[p_number] = THINKING;
    print[p_number][0]++;

    for(int i=temp;i<phi_number;i++){
        singing_test(i);

    }
    temp++;     //starvation ı önlemek için
    if(temp==phi_number)
        temp=0;


    printf("Philosopher %d is thinking\n", p_number + 1);
    sem_post(&mutex2);
}




void* philospher(void* num)
{

    while (1) {

        int *i = num;

        sleep(0.1);

        pickup(*i);

        sleep(0);

        putdown(*i);

        sleep(0.1);

        srand(time(NULL));

        int o;
        o = 1+(int)( rand() % 2);   //ya 1 ya 2 üretiyor

        int flag2=0;
        if (o == 1) {
            sing(*i);
            flag2=1;

        }
        if(flag2==1) {
            sleep(0);
            stopsinging(*i);
        }
        sleep(0);
    }
}


void handle_sigint(int sig)
{

    printf("\n");

    for(int i=0;i<phi_number;i++){

        printf("Philosopher %d's THINKING number is %d EATING number is %d WANTS TO SING %d times SINGING number is %d \n",i+1,print[i][0],print[i][1],print[i][3],print[i][2]);


    }
    int assolist=0;
    for(int i=0;i<phi_number;i++){
        if(print[i][2]>print[assolist][2])
            assolist=i;

    }
    printf("\nAssolist philosopher is %d'th one and song %d sing\n" , assolist+1,print[assolist][2]);

    exit(0);
}


int main(int argc, char* argv[])
{
    signal(SIGINT, handle_sigint);
while(1) {
    phi_number = atoi(argv[1]);
    if (!state)
        state = malloc(phi_number * sizeof(int));

    if (!phil) {
        phil = malloc(phi_number * sizeof(int));
        for (int i = 0; i < phi_number; i++) {
            phil[i] = i;
        }
    }
    if (!S)
        S = malloc(phi_number * sizeof(sem_t));
    if (!sing_)
        sing_ = malloc(phi_number * sizeof(sem_t));

    print = malloc(phi_number * sizeof(*print));
    for (int i = 0; i < phi_number; i++)
        print[i] = malloc(4 * sizeof(*print[i]));
    for (int i = 0; i < phi_number; i++) {
    for (int j = 0; j < 4; j++) {
        if(j==0)
            print[i][j]=1;
        else
        print[i][j] = 0;
    }
}




    int i;
    pthread_t thread_id[phi_number];

    sem_init(&mutex, 0, 1);
    sem_init(&mutex2, 0, 1);

    for (i = 0; i < phi_number; i++) {

        sem_init(&S[i], 0, 0);
        sem_init(&sing_[i], 0, 0);

    }
    for (i = 0; i < phi_number; i++) {

       
        pthread_create(&thread_id[i], NULL,
                       philospher, &phil[i]);

        printf("Philosopher %d is thinking\n", i + 1);
         // print[phi_number][0]++;
    }

    for (i = 0; i < phi_number; i++)

        pthread_join(thread_id[i], NULL);





    free(state);
    free(phil);
    free(S);
    free(sing_);
    free(print);





    return 0;
}
}

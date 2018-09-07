#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <queue>
#include <string>
#include <time.h>
#include <semaphore.h>
#include <sstream>

#define MOTHEREAGLE_SLEEP_TIME 1
#define BABYEAGLE_PLAY_TIME 2
#define BABYEAGLE_EAT_TIME 2
#define DEFAULT "10"
#define ARGS_MAX 4

using namespace std;

struct motherEagleData{
	int numFeedings;
};

struct babyEagleData{
	int eagleId;
	int numFeedings;
};

int numFeedingPots;
int numBabyEagles;
int numFeedings;
int fullPots;

int numWaitingBabyEagles=0;
bool alreadyWoken=false;
bool motherRetired=false;

sem_t semFeedingPots;
sem_t motherEagle; 
sem_t semWaiting;
pthread_mutex_t mutFeedingPots=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tty_lk=PTHREAD_MUTEX_INITIALIZER;

void goto_sleep();

void food_ready(int nthServing);

void ready_to_eat(int eagleId);

void finish_eating(int eagleId);

int pthread_sleep (int seconds);

void safePrint(string output);

string itos(int i);

string strSpaces(int numSpaces);

void *motherEagleThread_func(void *motherEagleArgs){
	struct motherEagleData *args;
	args=(struct motherEagleData *) motherEagleArgs;
	
	safePrint("Mother eagle started.");
	
    for(int i=0;i<numFeedings;i++){
        pthread_sleep(MOTHEREAGLE_SLEEP_TIME);
        goto_sleep();
        pthread_sleep(MOTHEREAGLE_SLEEP_TIME);
        food_ready(i);
    }
	
	motherRetired=true;
	
	for(int i=0;i<numBabyEagles;i++){
		sem_post(&semWaiting);
	}
	
	
	return (void*)NULL;
}


void *babyEagleThread_func(void *babyEagleArgs){
	struct babyEagleData *args;
	args=(struct babyEagleData *) babyEagleArgs;
	
	safePrint(strSpaces(args->eagleId)+"Baby eagle "+itos(args->eagleId)+" started.");
	
    while(1){

        ready_to_eat(args->eagleId);
        pthread_sleep(BABYEAGLE_EAT_TIME);

        finish_eating(args->eagleId);
        pthread_sleep(BABYEAGLE_PLAY_TIME);
    }
	return (void*) NULL;
}


int main(){
    printf("enter number of pots :  ");
    scanf("%d" , &numFeedingPots);
    printf("enter number of babycrows: ");
    scanf("%d" , &numBabyEagles);
    printf("enter t : ");
    scanf("%d" , &numFeedings);
    printf("there are %d babycrows , %d feeding pots , %d feeding \n" , numFeedingPots, numBabyEagles ,numFeedings);
		
   
	fullPots=0;
	
    sem_init(&semFeedingPots, 0, numFeedingPots);
    sem_init(&motherEagle, 0, 0);
	sem_init(&semWaiting, 0, 0);
	
	pthread_t motherEagle_tid;
	pthread_t babyEagleArr_tid[numBabyEagles];
	
	struct motherEagleData motherEagleArgs={ numFeedings };
	struct babyEagleData babyEagleArgs[numBabyEagles];
	
	int rc;

	rc = pthread_create(&motherEagle_tid, NULL, motherEagleThread_func, (void*) &motherEagleArgs);
	if ( rc == -1){
		perror("pthread_create");
		return -1;
	}

	for(int i=0; i<numBabyEagles; i++){
		babyEagleArgs[i].eagleId=i;
		babyEagleArgs[i].numFeedings=numFeedings;
		rc=pthread_create(&babyEagleArr_tid[i], NULL, &babyEagleThread_func, (void*) &babyEagleArgs[i]);
		if ( rc == -1){
			perror("pthread_create");
			return -1;
		}
	}

    void *status;
    for(int t=0; t<numBabyEagles; t++) {
		int rc = pthread_join(babyEagleArr_tid[t], &status);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n ", rc);
			exit(-1);
		}
	}
  
	pthread_join(motherEagle_tid, &status);

	cout<<"Mother eagle retires after serving "<<numFeedings<<" feedings.  Game ends!!!"<<endl;

    
    sem_destroy(&semFeedingPots); 
    sem_destroy(&motherEagle);
    sem_destroy(&semWaiting);
    pthread_mutex_destroy(&mutFeedingPots);
    pthread_mutex_destroy(&tty_lk);
	return 0;
}
void goto_sleep(){
    safePrint("Mother eagle takes a nap");
	sem_wait(&motherEagle);
}

void food_ready(int nthServing){

    pthread_mutex_lock(&mutFeedingPots);

	alreadyWoken=false;
	fullPots=numFeedingPots;
	cout<<"Mother eagle says \"Feeding ("<<nthServing + 1<<")\""<<endl;
	pthread_mutex_unlock(&mutFeedingPots);
	

	int semVal;
	sem_getvalue(&semFeedingPots, &semVal);

	for(int i=0;i<numFeedingPots;i++){
		numWaitingBabyEagles--;

		sem_post(&semWaiting);
	}
	sem_getvalue(&semFeedingPots, &semVal);	
}


void ready_to_eat(int eagleId){
	safePrint(strSpaces(eagleId)+"Baby eagle "+itos(eagleId)+" ready to eat, fullPots= "+itos(fullPots));
	
	bool fed=false;
	while(fed==false){

		pthread_mutex_lock(&mutFeedingPots);
		
		if(fullPots>0){
			sem_wait(&semFeedingPots);
			cout<<strSpaces(eagleId)<<"Baby eagle "<<eagleId<<" is eating using feeding pot "<< fullPots <<endl;
			cout.flush();
			fullPots--;	
			fed=true;
			cout.flush();
			pthread_mutex_unlock(&mutFeedingPots);
			sem_post(&semFeedingPots);
		}else if(motherRetired==false && fullPots ==0){
			if( alreadyWoken==false ){
				sem_post(&motherEagle);
				alreadyWoken=true;
				pthread_mutex_lock(&tty_lk);
				cout<<"Mother eagle is awoke by baby eagle "<<eagleId<<" and starts preparing food."<<endl;
				pthread_mutex_unlock(&tty_lk);
			}
			numWaitingBabyEagles++;
			pthread_mutex_unlock(&mutFeedingPots);

			sem_wait(&semWaiting);
			
		}else if(motherRetired==true && fullPots ==0){
			pthread_mutex_unlock(&mutFeedingPots);
			pthread_exit(NULL);
		}
	}
}

void finish_eating(int eagleId){
	safePrint(strSpaces(eagleId)+"Baby eagle "+itos(eagleId)+" finished eating");
	if(motherRetired==true && fullPots==0){
		pthread_exit(NULL);
	}
}

int pthread_sleep (int seconds)
{
	pthread_mutex_t mutex;
	pthread_cond_t conditionvar;
	struct timespec timetoexpire;
	if(pthread_mutex_init(&mutex,NULL))
	{
		return -1;
	}
	if(pthread_cond_init(&conditionvar,NULL))
	{
		return -1;
	}
	
	timetoexpire.tv_sec = (unsigned int)time(NULL) + seconds;
	timetoexpire.tv_nsec = 100;
	return pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
}

void safePrint(string output){
	pthread_mutex_lock(&tty_lk);
	cout<<output<<endl;
	cout.flush();
	pthread_mutex_unlock(&tty_lk);
}

string itos(int i){
	std::string s;
	std::stringstream out;
	out << i;
	return out.str();
}


string strSpaces(int numSpaces){
	string rs; 
	for(int i=0; i<numSpaces; i++){
		rs=rs+" ";
	}
	return rs;
}

/*
 * kosmos-mcv.c (mutexes & condition variables)
 *
 *
 * Here is some code from which to start.
 *
 * PLEASE FOLLOW THE INSTRUCTIONS REGARDING WHERE YOU ARE PERMITTED
 * TO ADD OR CHANGE THIS CODE. Read from line 133 onwards for
 * this information.
 */

#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logging.h"


/* Random # below threshold indicates H; otherwise C. */
#define ATOM_THRESHOLD 0.70
#define DEFAULT_NUM_ATOMS 40


#define MAX_ATOM_NAME_LEN 10
#define MAX_KOSMOS_SECONDS 5
#define BUFFER_SIZE 10

/* Global / shared variables */
int  cNum = 0, hNum = 0;
long numAtoms;
int radicals;
int num_free_c;
int num_free_h;

int combining_c1;
int combining_c2;
int combining_h;
char combiner[MAX_ATOM_NAME_LEN];

pthread_mutex_t mutex;
pthread_mutex_t mutex_c;
pthread_mutex_t mutex_h;
pthread_mutex_t logging;

pthread_cond_t wait_c;
pthread_cond_t wait_h;
pthread_mutex_t staging_area;


/* Function prototypes */
void kosmos_init(void);
void *c_ready(void *);
void *h_ready(void *);
void make_radical(int, int, int, char *);
void wait_to_terminate(int);


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
	int *pi = (int *)malloc(sizeof(int));
	assert( pi != NULL);
	*pi = i;
	return pi;
}


int main(int argc, char *argv[])
{
	long seed;
	numAtoms = DEFAULT_NUM_ATOMS;
	pthread_t **atom;
	int i;
	int status;

	if ( argc < 2 ) {
		fprintf(stderr, "usage: %s <seed> [<num atoms>]\n", argv[0]);
		exit(1);
	}

	if ( argc >= 2) {
		seed = atoi(argv[1]);
	}

	if (argc == 3) {
		numAtoms = atoi(argv[2]);
		if (numAtoms < 0) {
			fprintf(stderr, "%ld is not a valid number of atoms\n",
				numAtoms);
			exit(1);
		}
	}

    kosmos_log_init();
	kosmos_init();

	srand(seed);
	atom = (pthread_t **)malloc(numAtoms * sizeof(pthread_t *));
	assert (atom != NULL);
	for (i = 0; i < numAtoms; i++) {
		atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
		if ( (double)rand()/(double)RAND_MAX < ATOM_THRESHOLD ) {
			hNum++;
			status = pthread_create (
					atom[i], NULL, h_ready,
					(void *)dupInt(hNum)
				);
		} else {
			cNum++;
			status = pthread_create (
					atom[i], NULL, c_ready,
					(void *)dupInt(cNum)
				);
		}
		if (status != 0) {
			fprintf(stderr, "Error creating atom thread\n");
			exit(1);
		}
	}
    
            

    /* Determining the maximum number of ethynyl radicals is fairly
     * straightforward -- it will be the minimum of the number of
     * hNum and cNum/2.
     */

    int max_radicals = (hNum < cNum/2 ? hNum : (int)(cNum/2));
#ifdef VERBOSE
    
    printf("Maximum # of radicals expected: %d\n", max_radicals);  
    
#endif
    wait_to_terminate(max_radicals);
    
}


/*
* Now the tricky bit begins....  All the atoms are allowed
* to go their own way, but how does the Kosmos ethynyl-radical
* problem terminate? There is a non-zero probability that
* some atoms will not become part of a radical; that is,
* many atoms may be blocked on some condition variable of
* our own devising. How do we ensure the program ends when
* (a) all possible radicals have been created and (b) all
* remaining atoms are blocked (i.e., not on the ready queue)?
*/


/*
 * ^^^^^^^
 * DO NOT MODIFY CODE ABOVE THIS POINT.
 *
 *************************************
 *************************************
 *
 * ALL STUDENT WORK MUST APPEAR BELOW.
 * vvvvvvvv
 */


/* 
 * DECLARE / DEFINE NEEDED VARIABLES IMMEDIATELY BELOW.
 */



/*
 * FUNCTIONS YOU MAY/MUST MODIFY.
 */

void kosmos_init() {
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_c, NULL);
    pthread_mutex_init(&mutex_h, NULL);
    pthread_mutex_init(&logging, NULL);

    pthread_cond_init(&wait_c, NULL);
    pthread_cond_init(&wait_h, NULL);
    pthread_mutex_init(&staging_area, NULL);

    radicals = 0;
    combining_c1 = -1;
    combining_c2 = -1;
    combining_h = -1;
    num_free_c = 0;
    num_free_h = 0;
}


void *h_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "h%03d", id);

#ifdef VERBOSE
	printf("%s now exists\n", name);
#endif
    
    
    
    
    pthread_mutex_lock(&mutex);
    num_free_h++;
    pthread_mutex_unlock(&mutex);
    
    pthread_mutex_lock(&mutex_h);
    while(combining_h!=-1){
        pthread_cond_wait(&wait_h, &mutex_h);

    }
    pthread_mutex_unlock(&mutex_h);
    
    pthread_mutex_lock(&mutex);
    combining_h = id;
    pthread_mutex_unlock(&mutex);
    
    pthread_mutex_lock(&mutex);
    if(num_free_c >= 2){
        
        pthread_mutex_lock(&staging_area);
        strncpy(combiner, name, strlen(name));
        make_radical(combining_c1, combining_c2, combining_h, combiner);
        pthread_mutex_unlock(&staging_area);
        pthread_cond_signal(&wait_c);
        pthread_cond_signal(&wait_c);
    }
    pthread_mutex_unlock(&mutex);

    
    

    return NULL;


}



void *c_ready( void *arg )
{
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "c%03d", id);
    
    
    

#ifdef VERBOSE
	printf("%s now exists\n", name);
    
#endif
    
   

    pthread_mutex_lock(&mutex);
    num_free_c++;
    pthread_mutex_unlock(&mutex);
    
    pthread_mutex_lock(&mutex_c);
    while(combining_c1!=-1 && combining_c2!=-1){
        pthread_cond_wait(&wait_c, &mutex_c); 
    }
    pthread_mutex_unlock(&mutex_c);


    
    pthread_mutex_lock(&mutex);
    if (num_free_c==1 && combining_c1 == -1)
    {   
        combining_c1 = id;
    
    }
    pthread_mutex_unlock(&mutex);

    
    pthread_mutex_lock(&mutex);
    if (num_free_c > 1 && combining_c2 == -1)
    {
        combining_c2 = id;
    }
     pthread_mutex_unlock(&mutex);
    
    pthread_mutex_lock(&mutex);

    if (num_free_c >= 2 && num_free_h >= 1){
        
        pthread_mutex_lock(&staging_area);
        strncpy(combiner, name, strlen(name));
        make_radical(combining_c1, combining_c2, combining_h, combiner);
        pthread_mutex_unlock(&staging_area);
        pthread_cond_signal(&wait_c);
        pthread_cond_signal(&wait_h);
    }
    pthread_mutex_unlock(&mutex);


    return NULL;
    


    
                            
    
}
void make_radical(int c1, int c2, int h, char *maker){ 
    
    radicals++;
    kosmos_log_add_entry(radicals, c1, c2, h, maker);
    combining_c1 = -1;
    combining_c2 = -1;
    combining_h = -1;
    num_free_c-=2;
    num_free_h--;


      
}





void wait_to_terminate(int expected_num_radicals) {
    /* A rather lazy way of doing it, but good enough for this assignment. */
    
    sleep(MAX_KOSMOS_SECONDS);
    kosmos_log_dump();
    exit(0);
}

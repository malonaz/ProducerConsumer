// -*- C++ -*-
/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/




# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>

using namespace std;

#define SEM_KEY 0x68 // Change this number as needed
#define NUM_CMDLINE_ARGS 5
#define NUM_ARGS 4
#define ERROR -1
#define NO_ERROR 0


// producer and consumer status
#define DONE 0
#define EXECUTING 1
#define COMPLETED 2

struct Job{
  int id;
  int duration;
};


class circular_queue{
private:
  int size, front, back;
  Job** array;
  
public:
  circular_queue(int size);
  ~circular_queue(){delete [] array;}
  void add(Job* job_p);
  Job* get();
};


struct thread_info {           /* Used as argument to thread_start() */
  pthread_t      thread_id;    /* ID returned by pthread_create() */
  int            thread_num;   /* Application-defined thread # */
  int            sem_set_id;   /* Semaphore set id */
  circular_queue* c_queue;      /* Circular Queue */
  int            num_jobs;     /* For Producer use only */
};


union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_wait_with_time (int, short unsigned int, int);
void sem_signal (int, short unsigned int);
int sem_close (int);



/**
 * Mutator. Attempts to extract 4 integers from argv and copy them into arguments. 
 * returns -1 if an error is encountered, 0 otherwise.
 */
int get_args(int argc, char** argv, int* arguments);


/**
 * Prints output of a producer in a thread safe way!
 */
void print_producer(int thread_num, int status, Job* job_p);
void print_consumer(int thread_num, int status, Job* job_p);

void handle_sem_error(const char* msg, int sem_set_id);
void handle_thread_error(int en, const char* msg, int sem_set_id);

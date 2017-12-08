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
#define MICROSEC_PER_SEC 1000000

// parser definitions
#define NUM_CMDLINE_ARGS 5
#define NUM_ARGS 4
#define ERROR -1
#define NO_ERROR 0


// producer and consumer status
#define DONE 0
#define EXECUTING 1
#define COMPLETED 2

/**
 * Job. This structure represents a job as an id and a duration.
 */
struct job{
  int id;
  int duration;
};


/**
 * Circular Queue. This class represents a circular queue as a size
 * an array of pointers to jobs, and indices front and back that 
 * indicate what the indices of the job at the front of the queue
 * and the job at the back of the queue are.
 */
class circular_queue{
private:
  int size, front, back;
  job** array;
  
public:
  /**
   * Constructor. creates job pointers array of given size and initializes
   * all pointers to 0. Necessary for destructor delete calls!
   */
  circular_queue(int size);

  /**
   * Destructor. Deletes each job (safe to call on null pointers)
   * needed in case consumers wait longer than 20 seconds and don't 
   * "consume" jobs, leaving some jobs on heap. Then deletes array.
   */
  ~circular_queue();

  /**
   * Mutator. Requires semaphore to control access. Cannot be called on a 
   * full queue. Adds a job pointer to the back of the queue. 
   */
  void add(job* job_p);

  /**
   * Mutator. Requires semaphore to control access. Cannot be called on an
   * empty queue. Returns the pointer to the job at the front of the queue
   * after nulling its pointer in the queue. Caller is responsible for 
   * deletion of the job off the heap.
   */
  job* get();
  
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
 * returns ERROR if an error is encountered, 0 otherwise.
 */
int get_args(int argc, char** argv, int* arguments);


/**
 * Prints state of producer to std output stream in atomic manner.
 */
void print_producer(int thread_num, int status, job* job_p);

/**
 * Prints state of consumer to std output stream in atomic manner.
 */
void print_consumer(int thread_num, int status, job* job_p);


/**
 * prints error information to std error stream in atomic manner.
 * closes semaphore of given ID.
 */
void handle_sem_error(const char* msg, int sem_set_id);

/**
 * prints error information to std error stream using perror method.
 * closes semaphore of given ID.
 */
void handle_thread_error(int error, const char* msg, int sem_set_id);

/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include <iostream>


using namespace std;

// arguments
#define SIZE_ARG_INDEX 0
#define NUM_JOBS_ARG_INDEX 1
#define NUM_PRODUCERS_ARG_INDEX 2
#define NUM_CONSUMERS_ARG_INDEX 3


// Semaphore info
#define NUM_SEMS 3
#define MUTEX_SEM_INDEX 0
#define ITEMS_SEM_INDEX 1
#define SLOTS_LEFT_SEM_INDEX 2
#define SEM_WAIT_TIME 20


//forward declarations
void *producer (void *id);
void *consumer (void *id);

int main (int argc, char **argv){
  int arguments[NUM_ARGS], error;

  // try to parse arguments or handle error 
  if (get_args(argc, argv, arguments) == ERROR){
    return ERROR;
  }
  
  // initialize variables
  int queue_size = arguments[SIZE_ARG_INDEX];
  int num_jobs = arguments[NUM_JOBS_ARG_INDEX];
  int num_producers = arguments[NUM_PRODUCERS_ARG_INDEX];
  int num_consumers = arguments[NUM_CONSUMERS_ARG_INDEX];
  
  // setup circular queue
  circular_queue c_queue(queue_size);

  // try to create semaphore set or handle error
  int sem_set_id = sem_create(SEM_KEY, NUM_SEMS);
  if (sem_set_id == ERROR)
    handle_sem_error("Could not get semaphore set!\n", sem_set_id);
  
  // try to initiate semaphores or handle error
  if (sem_init(sem_set_id, MUTEX_SEM_INDEX, 1) == ERROR ||
      sem_init(sem_set_id, ITEMS_SEM_INDEX, 0) == ERROR ||
      sem_init(sem_set_id, SLOTS_LEFT_SEM_INDEX, queue_size) == ERROR)
    handle_sem_error("Could not initiate semaphores!\n", sem_set_id);

  // initiate producer and consumer thread info
  thread_info pt_info[num_producers], ct_info[num_consumers];
  
  // try to create a thread for each producer or handle error
  for (int tnum = 0; tnum < num_producers; tnum++){
    pt_info[tnum].thread_num = tnum + 1;
    pt_info[tnum].sem_set_id = sem_set_id;
    pt_info[tnum].c_queue = &c_queue;
    pt_info[tnum].num_jobs = num_jobs;
    error = pthread_create(&pt_info[tnum].thread_id, NULL, producer, (void*)&pt_info[tnum]);
    if (error != NO_ERROR)
      handle_thread_error(error, "pthread_create", sem_set_id);
  }

  // try to create a thread for each consumer or handle error
   for (int tnum = 0; tnum < num_consumers; tnum++){
    ct_info[tnum].thread_num = tnum + 1;
    ct_info[tnum].sem_set_id = sem_set_id;
    ct_info[tnum].c_queue = &c_queue;
    error = pthread_create(&ct_info[tnum].thread_id, NULL, consumer, (void*)&ct_info[tnum]);
    if (error != NO_ERROR)
      handle_thread_error(error, "pthread_create", sem_set_id);
  }
  
  // try to join threads or handle error
   for (int tnum = 0; tnum < num_producers; tnum++){
     error = pthread_join(pt_info[tnum].thread_id, NULL);
     if (error != NO_ERROR)
       handle_thread_error(error, "pthread_join", sem_set_id);
   }
   for (int tnum = 0; tnum < num_consumers; tnum++){
     error = pthread_join(ct_info[tnum].thread_id, NULL);
     if (error != NO_ERROR)
       handle_thread_error(error, "pthread_join", sem_set_id);
   }

  
  // close semaphore set or handle error
  if (sem_close(sem_set_id) == ERROR)
    cerr << "could not close semaphore!\n";
  
  exit(EXIT_SUCCESS);
}

void *producer(void *parameter){
  // get thread param info
  thread_info* pt_info = (thread_info *) parameter;

  // loop to create jobs
  for (int i = 0; i < pt_info->num_jobs; i++){

    // create new job. Consumer or Circular queue are responsible for freeing memory
    job* job_p = new job;
    // duration between 1 and 10 seconds
    job_p->duration = rand()%10  + 1;

    sem_wait_with_time(pt_info->sem_set_id, SLOTS_LEFT_SEM_INDEX, SEM_WAIT_TIME);
    if (errno == EAGAIN){
      delete job_p; // delete job to prevent memory leak  
      cerr << "wait time expired\n"; // thread safe
      pthread_exit(0);
    }
    sem_wait(pt_info->sem_set_id, MUTEX_SEM_INDEX);
    // -------------------------CRITICAL SECTION ---------------------------------
    
    // add job to circular queue
    pt_info->c_queue->add(job_p);

    //----------------------- CRITICAL SECTION END -------------------------------
    sem_signal(pt_info->sem_set_id, MUTEX_SEM_INDEX);
    sem_signal(pt_info->sem_set_id, ITEMS_SEM_INDEX);

    // output information. thread-safe
    print_producer(pt_info->thread_num, COMPLETED, job_p);
    
    // wait 1 to 5 seconds before adding the next job. 
    int wait_time = rand()%5 + 1;
    // multiply by 1000000 because usleep uses microseconds
    usleep(wait_time * MICROSEC_PER_SEC);
  }
  
  // output information. thread-safe
  print_producer(pt_info->thread_num, DONE, NULL);
  
  pthread_exit(0);
}

void *consumer (void *parameter){
  thread_info* ct_info = (thread_info *) parameter;

  while (true){
    sem_wait_with_time(ct_info->sem_set_id, ITEMS_SEM_INDEX, SEM_WAIT_TIME);
    // check time did not expire
    if (errno == EAGAIN)
      break;
    sem_wait(ct_info->sem_set_id, MUTEX_SEM_INDEX);
    // ------------------- CRITICAL SECTION ------------------------------------

    // get job. queue's pointer to this job is nulled after get operation 
    // so getting the job pointer is the only critical code.
    job* job_p = ct_info->c_queue->get();

    // ------------------- CRITICAL SECTION END --------------------------------
    sem_signal(ct_info->sem_set_id, MUTEX_SEM_INDEX);
    sem_signal(ct_info->sem_set_id, SLOTS_LEFT_SEM_INDEX); 
    
    // print execution information. thread-safe
    print_consumer(ct_info->thread_num, EXECUTING, job_p);
    
    // sleep for duration. * 1000 0000 because usleep uses microseconds
    usleep(job_p->duration * MICROSEC_PER_SEC);

    // print completion information. thread-safe
    print_consumer(ct_info->thread_num, COMPLETED, job_p);

    // free memory
    delete job_p;
    
  }

  // print information. thread-safe
  print_consumer(ct_info->thread_num, DONE, NULL);
  pthread_exit(0);

}

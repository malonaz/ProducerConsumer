/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include "circularQueue.h"
#include <iostream>

using namespace std;

// arguments
#define SIZE_ARG_INDEX 0
#define NUM_JOBS_ARG_INDEX 1
#define NUM_PRODUCERS_ARG_INDEX 2
#define NUM_CONSUMERS_ARG_INDEX 3

// Semaphore indices
#define NUM_SEMS 3
#define MUTEX_SEM_INDEX 0
#define ITEMS_SEM_INDEX 1
#define SLOTS_LEFT_SEM_INDEX 2

#define WAIT_TIME 5

// error handlers
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)


void *producer (void *id);
void *consumer (void *id);




int main (int argc, char **argv){
  int arguments[NUM_ARGS], s;

  // parse arguments or exit if error is encountered
  if (get_args(argc, argv, arguments) == ERROR)
    return ERROR;
  
  // initialize variables
  int queue_size = arguments[SIZE_ARG_INDEX];
  int num_jobs = arguments[NUM_JOBS_ARG_INDEX];
  int num_producers = arguments[NUM_PRODUCERS_ARG_INDEX];
  int num_consumers = arguments[NUM_CONSUMERS_ARG_INDEX];
  
  // setup circular queue
  CircularQueue c_queue(queue_size);

  // create semaphore set 
  int sem_set_id = sem_create(SEM_KEY, NUM_SEMS);
  if (sem_set_id == ERROR){
    cerr << "could not get semaphore set!\n";
    return ERROR;
  }
  cout << "opened semaphore: " << sem_set_id << endl;

  // initate semaphores
  if (sem_init(sem_set_id, MUTEX_SEM_INDEX, 1) == ERROR ||
      sem_init(sem_set_id, ITEMS_SEM_INDEX, 0) == ERROR ||
      sem_init(sem_set_id, SLOTS_LEFT_SEM_INDEX, queue_size) == ERROR){
    cerr << "could not initiate semaphores!\n";
    
    // close semaphore set
    sem_close(sem_set_id);
    return ERROR;
  }

  // initiate producer and consumer thread info
  thread_info pt_info[num_producers], ct_info[num_consumers];
  
  // create one thread for each producer
  for (int tnum = 0; tnum < num_producers; tnum++){
    pt_info[tnum].thread_num = tnum + 1;
    pt_info[tnum].sem_set_id = sem_set_id;
    pt_info[tnum].c_queue = &c_queue;
    pt_info[tnum].num_jobs = num_jobs;
    s = pthread_create(&pt_info[tnum].thread_id, NULL, producer, (void*)&pt_info[tnum]);
    if (s != NO_ERROR)
      handle_error_en(s, "pthread_create");
  }

  // create one thread for each consumer
   for (int tnum = 0; tnum < num_consumers; tnum++){
    ct_info[tnum].thread_num = tnum + 1;
    ct_info[tnum].sem_set_id = sem_set_id;
    ct_info[tnum].c_queue = &c_queue;
    s = pthread_create(&ct_info[tnum].thread_id, NULL, consumer, (void*)&ct_info[tnum]);
    if (s != NO_ERROR)
      handle_error_en(s, "pthread_create");
  }
  
  // join threads
   for (int tnum = 0; tnum < num_producers; tnum++){
     s = pthread_join(pt_info[tnum].thread_id, NULL);
     if (s != NO_ERROR)
       handle_error_en(s, "pthread_join");
   }
   for (int tnum = 0; tnum < num_consumers; tnum++){
     s = pthread_join(ct_info[tnum].thread_id, NULL);
     if (s != NO_ERROR)
       handle_error_en(s, "pthread_join");
   }

  
  // closing semaphore set
  if (sem_close(sem_set_id) == ERROR)
    cerr << "could not close semaphore!\n";
  
  exit(EXIT_SUCCESS);
}

void *producer(void *parameter){

  thread_info* pt_info = (thread_info *) parameter;
    
  for (int i = 0; i < pt_info->num_jobs; i++){
    
    // wait 1 to 5 seconds before adding the next job
    int wait_time = rand()%5 + 1; 
    usleep(wait_time * 1000);
    
    sem_wait_with_time(pt_info->sem_set_id, SLOTS_LEFT_SEM_INDEX, WAIT_TIME);
    if (errno == EAGAIN){
      cerr << "wait time expired\n";
      pthread_exit(0);
    }
      
    sem_wait(pt_info->sem_set_id, MUTEX_SEM_INDEX);
    
    // create new job. consumer is responsible for freeing memory
    Job* job_p = new Job;
    job_p->duration = rand()%10  + 1;
    
    // add to circular queue
    pt_info->c_queue->add(job_p);
    
    // output information
    cout << "Producer(" << pt_info->thread_num << "): Job id ";
    cout << job_p->id << " duration " << job_p->duration << endl;
    
    // end critical section
    sem_signal(pt_info->sem_set_id, MUTEX_SEM_INDEX);
    sem_signal(pt_info->sem_set_id, ITEMS_SEM_INDEX);
  }
  
  cout << "Producer(" << pt_info->thread_num << "): No more jobs to generate.\n";
  
  pthread_exit(0);
}

void *consumer (void *parameter){
  thread_info* ct_info = (thread_info *) parameter;

  while (true){
    sem_wait_with_time(ct_info->sem_set_id, ITEMS_SEM_INDEX, WAIT_TIME);
    // check time did not expire
    if (errno == EAGAIN)
      break;
    
    sem_wait(ct_info->sem_set_id, MUTEX_SEM_INDEX);
    // critical section

     

    // get job
    Job* job_p = ct_info->c_queue->get();
    
    cout << "Consumer(" << ct_info->thread_num << "): Job id " << job_p->id;
    cout << " executing sleep duration " << job_p->duration << endl;
    
    // sleep for duration
    usleep(job_p->duration * 1000);

    cout << "Consumer(" << ct_info->thread_num << "): Job id " << job_p->id;
    cout << " completed\n";
    
    // free memory
    delete job_p;
    
    // end critical section
    sem_signal(ct_info->sem_set_id, MUTEX_SEM_INDEX);
    sem_signal(ct_info->sem_set_id, SLOTS_LEFT_SEM_INDEX); 
  }
  cout << "Consumer(" << ct_info->thread_num << "): No more jobs left.\n";
  pthread_exit(0);

}

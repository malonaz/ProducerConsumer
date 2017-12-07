/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

#include "helper.h"
#include <sstream>

#define NUM_CMDLINE_ARGS 5
#define ERROR -1
#define NO_ERROR 0


int get_args(int argc, char** argv, int* arguments){
  // error processing
  if (argc != NUM_CMDLINE_ARGS){
    cerr << "Invalid number of arguments.";
    cerr << " You entered " << argc -1 << " arguments!\n";
    return ERROR;
  }
  
  argv++;  // move past self argument
  
  // attempts to read arguments
  istringstream ss;
  for (int i = 0; i < NUM_ARGS; i++){
    ss.str(argv[i]);
    if (!(ss >> arguments[i]) || arguments[i] < 0) {
      cerr << "Invalid input: " << argv[i] << endl;
      return ERROR;
    }
    ss.clear();
  }
  return NO_ERROR;
}


void handle_thread_error(int en, const char* msg, int sem_set_id){
  errno = en;
  perror(msg);

  // close semaphore set
  sem_close(sem_set_id);
  exit(EXIT_FAILURE);
}

void handle_sem_error(const char* msg, int sem_set_id){
  cerr << msg;
  
  // close semaphore set
  sem_close(sem_set_id);
  exit(EXIT_FAILURE);
}


circular_queue::circular_queue(int size): size(size), front(-1), back(-1){
    array = new job*[size];
    for (int i = 0; i < size; i++)
      array[i] = 0; // null pointers
  }

circular_queue::~circular_queue(){
  for (int i = 0; i < size; i++)
    // delete job pointer. safe to call on null pointers
    delete array[i];
  
  // deletes the array.
  delete [] array;
}


void circular_queue::add(job* job_p){
  // no logic checks for full queue as semaphores prevent threads
  // from adding job to a full queue
    
  if (front == -1)
    // queue is empty 
    front = back = 0;

  // since this is a circular queue, we go to first index.
  else if (back == size -1) 
    back = 0;
    
  else
    back++;

  job_p->id = back + 1;
  array[back] = job_p;
}


job* circular_queue::get(){
  // no logic check for empty queue as semaphores prevent threads
  // from getting job from an empty queue

  // save the job pointer
  job* job_p = array[front];

  // remove it from queue by nulling 
  array[front] = 0; 

  
  if (front == back)
    // there is only one item in the queue
    // set front and back to -1 to indicate queue is now empty
    front = back = -1; 
    
  else if (front == size -1)
    // wrap around
    front = 0;
  
  else
    front++;

  return job_p;

}


void print_producer(int thread_num, int status, job* job_p){
  stringstream stream;
  stream << "Producer(" << thread_num << "): ";
  if (status == DONE)
    stream << "No more jobs to generate.";
  else if (status == COMPLETED)
    stream << "job id " << job_p->id << " duration " << job_p->duration;
  stream << endl;
  cout << stream.str();
}

void print_consumer(int thread_num, int status, job* job_p){
  stringstream stream;
  stream << "Consumer(" << thread_num << "): ";
  if (status == DONE)
    stream << "No more jobs left.";
  else if (status == EXECUTING)
    stream << "job id " << job_p->id << " executing sleep duration " << job_p->duration;
  else if (status == COMPLETED)
    stream << "job id " << job_p->id << " completed";

  stream << endl;
  cout << stream.str();
}

int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}


int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

void sem_wait_with_time (int id, short unsigned int num, int seconds)
{
  struct timespec ts = {seconds, 0};
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semtimedop (id, op, 1, &ts);

}
void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}

/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include "circularQueue.h"
#include <iostream>

using namespace std;


#define SIZE_ARG_INDEX 0
#define NUM_JOBS_ARG_INDEX 1
#define NUM_PRODUCERS_ARG_INDEX 2
#define NUM_CONSUMERS_ARG_INDEX 3


void *producer (void *id);
void *consumer (void *id);

int main (int argc, char **argv){
  int arguments[NUM_ARGS];

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
  
  pthread_t producerid;
  int parameter = 5;

  pthread_create (&producerid, NULL, producer, (void *) &parameter);

  pthread_join (producerid, NULL);

  return 0;
}

void *producer(void *parameter) 
{

  // TODO

  int *param = (int *) parameter;

  cout << "Parameter = " << *param << endl;

  pthread_exit(0);
}

void *consumer (void *id) 
{
    // TODO 

  pthread_exit (0);

}

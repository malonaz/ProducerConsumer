/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

int main (int argc, char **argv)
{
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

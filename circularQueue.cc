#include "circularQueue.h"



CircularQueue::CircularQueue(int size): front(-1), back(-1), size(size){
  array = new Job*[size];
}
  
				       
void CircularQueue::add(Job* job_p){
  if ((front == 0 &&  back == size -1) || (back == front -1))
    // queue is full
    return;
    
  else if (front == -1)
    // queue is empty 
    front = back = 0;

  // we know front != 0. so queue cannot be empty.
  // since this is a circular queue, we go to first index.
  else if (back == size -1) 
    back = 0;
    
  else
    back++;

  job_p->id = back + 1;
  array[back] = job_p;
}


Job* CircularQueue::get(){
  if (front == -1 && back == -1)
    // queue is empty so return null pointer.
    return 0; 

  // save the job pointer
  Job* job_p = array[front];


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

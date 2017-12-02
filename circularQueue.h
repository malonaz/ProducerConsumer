// -*- C++ -*-
#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H




struct Job{
  int id;
  int duration;
};


class CircularQueue{
  
private:
  int front;
  int back;
  const int size;
  Job** array;
  
public:
  CircularQueue(int size);
  ~CircularQueue(){delete [] array;}
  void add(Job* job_p);
  Job* get();
};







struct ThreadParam{
  CircularQueue* c_queue_p;
  int num_jobs_producer;
  int sem_set_id;
};


#endif




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
  void add(Job* job_p);
  Job* get();
};

#endif

#ifndef PROCESS_H
#define PROCESS_H

struct thread
{
       int thread_id;
       int process_id;

       char name[20];
} ;

struct process
{
       int process_id;
       
       char name[20];
} ;

#endif

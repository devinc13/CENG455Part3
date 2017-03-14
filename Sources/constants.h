/*
 * constants.h
 *
 *  Created on: Mar 6, 2017
 *      Author: devinc
 */


#ifndef SOURCES_CONSTANTS_H_
#define SOURCES_CONSTANTS_H_

#include <mqx.h>
#include <message.h>



typedef struct scheduler_message
{
   MESSAGE_HEADER_STRUCT HEADER;
   int TYPE; // 0 - create, 1 - delete, 2 - return active, 3 - return overdue
   int TASKID;
   int DEADLINE;
} SCHEDULER_MESSAGE, * SCHEDULER_MESSAGE_PTR;


struct task_list {
    unsigned int tid;
    unsigned int deadline;
    unsigned int task_type;
    unsigned int creation_time;
    struct task_list *next_cell;
    struct task_list *previous_cell;
};


struct overdue_tasks {
    unsigned int tid;
    unsigned int deadline;
    unsigned int task_type;
    unsigned int creation_time;
    struct overdue_tasks *next_cell;
    struct overdue_tasks *previous_cell;
};

extern _pool_id message_pool;
extern _queue_id scheuler_qid;

#endif /* SOURCES_CONSTANTS_H_ */

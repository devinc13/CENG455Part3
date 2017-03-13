/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : ceng455part3
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-06, 16:10, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         task_generator - void task_generator(os_task_param_t task_init_data);
**         dd_scheduler   - void dd_scheduler(os_task_param_t task_init_data);
**         master_task    - void master_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "DDScheduler.h"
#include "TaskGenerator.h"
#include "constants.h"
#include "api.h"
#include "IdleTask.h"
#include <timer.h>

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
_pool_id message_pool;
_queue_id scheuler_qid;
struct task_list taskList;
struct overdue_tasks overdueTasks;


/*
** ===================================================================
**     Callback    : task_generator
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void task_generator(os_task_param_t task_init_data)
{
	// Create Idle task
	_task_id task_id = _task_create(0, IDLETASK_TASK, 0);

	if (task_id == 0) {
	  printf("\nCould not create idle task\n");
	  _task_block();
	}


	// Create simple task
	dd_tcreate(USERTASK_TASK, 50, 20);


  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : dd_scheduler
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void dd_scheduler(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	int timeout = 0;

	/* open a message queue */
	scheuler_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);

	if (scheuler_qid == 0) {
	  printf("\nCould not open the scheduler message queue\n");
	  _task_block();
	}

	/* create a message pool */
	message_pool = _msgpool_create(sizeof(SCHEDULER_MESSAGE),
	  10, 0, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
	  printf("\nCount not create scheduler message pool\n");
	  _task_block();
	}

  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
	  	SCHEDULER_MESSAGE_PTR msg_ptr = _msgq_receive(scheuler_qid, timeout);

		if (msg_ptr == NULL) {
			printf("\nTIMEOUT\n");
			// TODO: Handle timeout

		}

		printf("Message received:\n");
		printf("%d \n", msg_ptr->TYPE);

		// TODO: Add to task list, sort it, update priorities
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : master_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void master_task(os_task_param_t task_init_data)
{
	printf("\x1B[H\x1B[J");
	printf("Creating Scheduler and Task Generator\n");

	_task_id task_id = _task_create(0, DDSCHEDULER_TASK, 0);

	if (task_id == 0) {
	  printf("\nCould not create dd_scheduler task\n");
	  _task_block();
	}

	task_id = _task_create(0, TASKGENERATOR_TASK, 0);

	if (task_id == 0) {
	  printf("\nCould not create task_generator task\n");
	  _task_block();
	}
}

void turn_off_flag(_timer_id t, void* dataptr, unsigned int seconds, unsigned int miliseconds){
    (*(bool*)dataptr) = false;
}

/*
** ===================================================================
**     Callback    : user_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task(os_task_param_t task_init_data)
{
  unsigned int runtime = task_init_data;

  // Busy loop
  bool flag = true;
  _timer_start_oneshot_after(turn_off_flag, &flag, TIMER_ELAPSED_TIME_MODE, runtime);
  while(flag);
  dd_delete(_task_get_id());
  _task_block();

}

/*
** ===================================================================
**     Callback    : idle_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void idle_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */

	// TODO: Run busy loop that records its runtime
	printf("ILDE TASK!\n");



  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/

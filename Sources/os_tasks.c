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
struct task_list *taskList = NULL;
struct overdue_tasks *overdueTasks = NULL;


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
	struct task_list * active_tasks_head_ptr = NULL;
	struct overdue_tasks * overdue_tasks_head_ptr = NULL;

	// Create Idle task
	_task_id task_id = _task_create(0, IDLETASK_TASK, 0);

	if (task_id == 0) {
	  printf("\nCould not create idle task\n");
	  _task_block();
	}

	int n_total_tasks = 2;

	// TODO: ADD PERIODIC TASKS/TESTS

	// Create simple task 1
	_task_id t1 = dd_tcreate(USERTASK_TASK, 50, 20);

	// Create simple task 2
	_task_id t2 = dd_tcreate(USERTASK_TASK, 40, 30);

	printf("TASK GENERATOR: %d tasks created.\n\r", n_total_tasks);


	// WAIT A BIT
	_time_delay(3000);


	// OBTAIN STATUS FROM SCHEDULER
	printf("TASK GENERATOR: collecting statistics\n\r");
	if(!dd_return_active_list(&active_tasks_head_ptr) || !dd_return_overdue_list(&overdue_tasks_head_ptr)){
		printf("error: failed to obtain the tasks list!\n\r");
		return 1;
	}


	int n_completed_tasks = 0;
	int n_failed_tasks = 0;
	int n_running_tasks = 0;

	struct task_list *temp_at_ptr = active_tasks_head_ptr;
	while(temp_at_ptr){
		n_running_tasks++;
		temp_at_ptr = temp_at_ptr->next_cell;
	}



	struct task_list *temp_ot_ptr = overdue_tasks_head_ptr;
	while(temp_at_ptr){
		n_failed_tasks++;
		temp_ot_ptr = temp_ot_ptr->next_cell;
	}

	n_completed_tasks = n_total_tasks-(n_failed_tasks+n_running_tasks);
	printf("TASK GENERATOR: %d failed, %d completed, %d still running.\n\r", n_failed_tasks, n_completed_tasks, n_running_tasks);

	return 0;
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
		switch(msg_ptr->TYPE) {
			case 0:
			{
				if (taskList == NULL) {
					TIME_STRUCT start_time;
					_time_get(&start_time);

					struct task_list newTask;
					newTask.tid = msg_ptr->TASKID;
					newTask.deadline = msg_ptr->DEADLINE;
					newTask.creation_time = start_time.MILLISECONDS;
					newTask.next_cell = NULL;
					newTask.previous_cell = NULL;
					taskList = &newTask;
					printf("Actual address of new task = %d\n", taskList);

					// TODO: Set priority of task

				} else {
					//TODO: Put into list sorted
				}
				break;
			}
			case 1:
			{
				printf("Delete");
				break;
			}
			case 2:
			{
				/*allocate a message*/
				TASK_LIST_MESSAGE_PTR new_msg_ptr = (TASK_LIST_MESSAGE_PTR)_msg_alloc(message_pool);

				if (new_msg_ptr == NULL) {
					printf("\nCould not allocate a message\n");
					_task_block();
				}

				new_msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
				new_msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
				// TODO: COPY LIST
				new_msg_ptr->task_list_head_ptr = taskList;


				int result = _msgq_send(new_msg_ptr);

				if (result != TRUE) {
				 printf("\nCreate could not send a message\n");
				 _task_block();
				}

				break;
			}
			case 3:
			{
				break;
			}
		}
   
    
    
    
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
	while(1);
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

/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : CENG455Part3
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
_queue_id scheduler_qid;
struct task_list *taskList = NULL;
struct overdue_tasks *overdueTasks = NULL;
volatile uint32_t ms_loop;
unsigned int idle_time = 0;
unsigned int total_tasks = 0;
unsigned int reporting_period_ms = 5000;

typedef struct periodic_data {
    unsigned int deadline;
    unsigned int runtime;
} periodic_data;



void create_periodic_task(_timer_id t, void* dataptr, unsigned int seconds, unsigned int miliseconds){
	periodic_data * data = dataptr;
    dd_tcreate(USERTASK_TASK, data->deadline, data->runtime);
    total_tasks++;
};


void report_statistics(_timer_id t, void* dataptr, unsigned int seconds, unsigned int miliseconds){
	// OBTAIN STATUS FROM SCHEDULER
	struct task_list * active_tasks_head_ptr = NULL;
	struct overdue_tasks * overdue_tasks_head_ptr = NULL;

	printf("\x1B[H\x1B[J");
	if(!dd_return_active_list(&active_tasks_head_ptr) || !dd_return_overdue_list(&overdue_tasks_head_ptr)){
		printf("error: failed to obtain the tasks list!\n\r");
		return;
	}

	int n_completed_tasks = 0;
	int n_failed_tasks = 0;
	int n_running_tasks = 0;

	struct task_list *temp_at_ptr = active_tasks_head_ptr;
	while(temp_at_ptr){
		n_running_tasks++;
		temp_at_ptr = temp_at_ptr->next_cell;
	}


	struct overdue_tasks *temp_ot_ptr = overdue_tasks_head_ptr;
	while(temp_ot_ptr){
		n_failed_tasks++;
		temp_ot_ptr = temp_ot_ptr->next_cell;
	}

	n_completed_tasks = total_tasks-(n_failed_tasks+n_running_tasks);
	printf("TASK GENERATOR: %d failed, %d completed, %d still running.\n\r", n_failed_tasks, n_completed_tasks, n_running_tasks);

	int total_execution_time = reporting_period_ms - idle_time;
	float total_time = reporting_period_ms;
	if (total_execution_time <= 0) {
		printf("Utilization = 0%%\n");
	} else if (total_execution_time >= reporting_period_ms) {
		printf("Utilization = 100%%\n");
	} else {
		printf("Utilization = %d%%\n", (int)((float)total_execution_time / total_time * 100));
	}

	idle_time = 0;
}




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


	_mqx_uint priority;
	_task_set_priority(_task_get_id(), 11, &priority);
	_task_get_priority(_task_get_id(), &priority);
	printf("Generator priority = %d\n", priority);

	// Create Idle task
	_task_id idle_task_id = _task_create(0, IDLETASK_TASK, 0);

	if (idle_task_id == 0) {
	  printf("\nCould not create idle task\n");
	  _task_block();
	}

	_task_set_priority(idle_task_id, 20, &priority);
	_task_get_priority(idle_task_id, &priority);
	printf("Idle task priority = %d\n", priority);


	// Aperiodic tasks
	// First parameter is deadline, second is runtime

//	dd_tcreate(USERTASK_TASK, 400, 500);
//	total_tasks++;

//	dd_tcreate(USERTASK_TASK, 690, 300);
//	total_tasks++;
	printf("TASK GENERATOR: %d aperiodic tasks created.\n\r", total_tasks);



	// Periodic tasks - comma separated string - first is deadline, second is runtime

	periodic_data * task_data = _mem_alloc(sizeof(unsigned int) * 2);
	task_data->deadline = 5000;
	task_data->runtime = 2500;
	int period_ms = 5000;
	_timer_start_periodic_every(create_periodic_task, task_data, TIMER_KERNEL_TIME_MODE, period_ms);



	// Report statistics periodically
	_timer_start_periodic_every(report_statistics, NULL, TIMER_KERNEL_TIME_MODE, reporting_period_ms);
	_task_block();
}

// Call this whenever making a new task active that might already be past its deadline
void remove_already_overdue_tasks() {
	struct task_list * next_task_to_run_ptr = taskList;

	TIME_STRUCT curr_time;
	_time_get(&curr_time);
	curr_time.MILLISECONDS;

	int timeout = (next_task_to_run_ptr->creation_time + next_task_to_run_ptr->deadline) - curr_time.MILLISECONDS;

	while (timeout < 0) {
		// These tasks have already gone past their deadline

		// Set complete task to 25 so it doesn't run anymore
		_mqx_uint priority;
		_task_set_priority(next_task_to_run_ptr->tid, 25, &priority);

		struct overdue_tasks * overdue_ptr = _mem_alloc(sizeof(unsigned int) * 4 + sizeof(void*) * 2);
		overdue_ptr->tid = next_task_to_run_ptr->tid;
		overdue_ptr->deadline = next_task_to_run_ptr->deadline;
		overdue_ptr->creation_time = next_task_to_run_ptr->creation_time;
		overdue_ptr->next_cell = NULL;
		overdue_ptr->previous_cell = NULL;

		if (overdueTasks == NULL) {
			overdueTasks = overdue_ptr;
		} else {
			overdue_ptr->next_cell = overdueTasks;
			overdueTasks->previous_cell = overdue_ptr;
			overdueTasks = overdue_ptr;
		}

		_mem_free(next_task_to_run_ptr);

		TIME_STRUCT curr_time;
		_time_get(&curr_time);
		curr_time.MILLISECONDS;

		next_task_to_run_ptr = next_task_to_run_ptr->next_cell;
		if (next_task_to_run_ptr == NULL) {
			return;
		}

		timeout = (next_task_to_run_ptr->creation_time + next_task_to_run_ptr->deadline) - curr_time.MILLISECONDS;
	}


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

	_mqx_uint priority;
	_task_set_priority(_task_get_id(), 9, &priority);
	_task_get_priority(_task_get_id(), &priority);
	printf("Scheduler priority = %d\n", priority);


  /* Write your local variable definition here */
	int timeout = 0;

	/* open a message queue */
	scheduler_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);

	if (scheduler_qid == 0) {
	  printf("\nCould not open the scheduler message queue\n");
	  _task_block();
	}

	/* create a message pool */
	message_pool = _msgpool_create(sizeof(SCHEDULER_MESSAGE),
	  10, 0, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
	  printf("\nCould not create scheduler message pool\n");
	  _task_block();
	}

  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
	  	SCHEDULER_MESSAGE_PTR msg_ptr = _msgq_receive(scheduler_qid, timeout);

		if (msg_ptr == NULL) {
			struct task_list * timeout_task_ptr = taskList;
			struct task_list * next_task_ptr = taskList->next_cell;

			// Set complete task to 25 so it doesn't run anymore
			_mqx_uint priority;
			_task_set_priority(timeout_task_ptr->tid, 25, &priority);

			if (next_task_ptr == NULL) {
				taskList = NULL;
				timeout = 0;
			} else {
				next_task_ptr->previous_cell = NULL;

				taskList = next_task_ptr;
				remove_already_overdue_tasks();

				// Make next task active
				_task_set_priority(taskList->tid, 15, &priority);

				TIME_STRUCT curr_time;
				_time_get(&curr_time);
				curr_time.MILLISECONDS;

				timeout = (taskList->creation_time + taskList->deadline) - curr_time.MILLISECONDS;
			}

			struct overdue_tasks * overdue_ptr = _mem_alloc(sizeof(unsigned int) * 4 + sizeof(void*) * 2);
			overdue_ptr->tid = timeout_task_ptr->tid;
			overdue_ptr->deadline = timeout_task_ptr->deadline;
			overdue_ptr->creation_time = timeout_task_ptr->creation_time;
			overdue_ptr->next_cell = NULL;
			overdue_ptr->previous_cell = NULL;

			if (overdueTasks == NULL) {
				overdueTasks = overdue_ptr;
			} else {
				overdue_ptr->next_cell = overdueTasks;
				overdueTasks->previous_cell = overdue_ptr;
				overdueTasks = overdue_ptr;
			}

			_mem_free(timeout_task_ptr);

			continue;
		}

		bool timeoutCreated = false;

		switch(msg_ptr->TYPE) {
			case 0:
			{
				TIME_STRUCT start_time;
				_time_get(&start_time);

				if (taskList == NULL) {
					struct task_list * newTask_ptr = _mem_alloc(sizeof(unsigned int) * 4 + sizeof(void*) * 2);
					newTask_ptr->tid = msg_ptr->TASKID;
					newTask_ptr->deadline = msg_ptr->DEADLINE;
					newTask_ptr->creation_time = start_time.MILLISECONDS;
					newTask_ptr->next_cell = NULL;
					newTask_ptr->previous_cell = NULL;
					taskList = newTask_ptr;
					_mqx_uint priority;
					_task_set_priority(newTask_ptr->tid, 15, &priority);

					timeoutCreated = true;
					timeout = newTask_ptr->deadline;

				} else {
					// Put into list sorted
					struct task_list * temp_task_list_ptr = taskList;
					// If this is the highest priority, put at front and make this task active
					if (msg_ptr->DEADLINE < taskList->deadline) {
						// Create new higher priority task
						struct task_list * newTask_ptr = _mem_alloc(sizeof(unsigned int) * 4 + sizeof(void*) * 2);
						newTask_ptr->tid = msg_ptr->TASKID;
						newTask_ptr->deadline = msg_ptr->DEADLINE;
						newTask_ptr->creation_time = start_time.MILLISECONDS;
						newTask_ptr->next_cell = taskList;
						newTask_ptr->previous_cell = NULL;
						// set previous of old highest priority
						taskList->previous_cell = newTask_ptr;
						// point to new higher priority task
						taskList = newTask_ptr;

						// Set old task to 25
						_mqx_uint priority;
						_task_set_priority(newTask_ptr->next_cell->tid, 25, &priority);

						// Make new task active
						_task_set_priority(newTask_ptr->tid, 15, &priority);

						timeoutCreated = true;
						timeout = newTask_ptr->deadline;

					} else {
						// Find where it goes in the list
						while (temp_task_list_ptr->next_cell != NULL && msg_ptr->DEADLINE > temp_task_list_ptr->deadline) {
							temp_task_list_ptr = taskList->next_cell;
						}

						if (temp_task_list_ptr->next_cell == NULL) {
							// Put it at the end of the list
							struct task_list * newTask_ptr = _mem_alloc(sizeof(unsigned int) * 4 + sizeof(void*) * 2);
							newTask_ptr->tid = msg_ptr->TASKID;
							newTask_ptr->deadline = msg_ptr->DEADLINE;
							newTask_ptr->creation_time = start_time.MILLISECONDS;
							newTask_ptr->next_cell = NULL;
							newTask_ptr->previous_cell = temp_task_list_ptr;
							temp_task_list_ptr->next_cell = newTask_ptr;
						} else {
							// Put it in between cells
							struct task_list * newTask_ptr = _mem_alloc(sizeof(unsigned int) * 4 + sizeof(void*) * 2);
							newTask_ptr->tid = msg_ptr->TASKID;
							newTask_ptr->deadline = msg_ptr->DEADLINE;
							newTask_ptr->creation_time = start_time.MILLISECONDS;
							newTask_ptr->next_cell = temp_task_list_ptr;
							newTask_ptr->previous_cell = temp_task_list_ptr->previous_cell;

							temp_task_list_ptr->previous_cell->next_cell = newTask_ptr;
							temp_task_list_ptr->previous_cell = newTask_ptr;
						}
					}

				}
				break;
			}
			case 1:
			{
				struct task_list * complete_task_ptr = taskList;
				struct task_list * next_task_ptr = taskList->next_cell;

				// Set complete task to 25 so it doesn't run anymore
				_mqx_uint priority;
				_task_set_priority(complete_task_ptr->tid, 25, &priority);

				if (next_task_ptr == NULL) {
					taskList = NULL;
				} else {
					next_task_ptr->previous_cell = NULL;

					taskList = next_task_ptr;
					remove_already_overdue_tasks();

					// Make next task active
					_task_set_priority(taskList->tid, 15, &priority);

					timeoutCreated = true;

					TIME_STRUCT curr_time;
					_time_get(&curr_time);
					curr_time.MILLISECONDS;

					timeout = (taskList->creation_time + taskList->deadline) - curr_time.MILLISECONDS;
				}

				_mem_free(complete_task_ptr);

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
				/*allocate a message*/
				TASK_LIST_MESSAGE_PTR new_msg_ptr = (TASK_LIST_MESSAGE_PTR)_msg_alloc(message_pool);

				if (new_msg_ptr == NULL) {
					printf("\nCould not allocate a message\n");
					_task_block();
				}

				new_msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
				new_msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
				// TODO: COPY LIST
				new_msg_ptr->overdue_tasks_head_ptr = overdueTasks;


				int result = _msgq_send(new_msg_ptr);

				if (result != TRUE) {
				 printf("\nCreate could not send a message\n");
				 _task_block();
				}

				break;
			}
		}

		_msg_free(msg_ptr);

		if (timeoutCreated == false) {
			if (taskList == NULL) {
				timeout = 0;
				continue;
			}

			TIME_STRUCT curr_time;
			_time_get(&curr_time);
			curr_time.MILLISECONDS;

			timeout = (taskList->creation_time + taskList->deadline) - curr_time.MILLISECONDS;
			if (timeout < 0) {
				printf("SOMETHING WENT VERY WRONG\n");
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
	// This was our calibration
	//	  bool flag = true;
	//	  _timer_start_oneshot_after(turn_off_flag, &flag, TIMER_ELAPSED_TIME_MODE, 10000);
	//	  int counter = 0;
	//	  while(flag) {
	//		  counter++;
	//	  }
	//
	//	  printf("****\n");
	//	  printf("Counter = %d\n", counter);
	//	  printf("****\n");

	// Calibration = 10862 per millisecond

//	while(1) {
//		idle_counter++;
//	}

	while(1) {
		int ms_loop = 1;

		while(ms_loop < 10862){
			ms_loop += 1;
		}

		idle_time += 1;
	}
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

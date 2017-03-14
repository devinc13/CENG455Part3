/*
 * api.c
 *
 *  Created on: Mar 6, 2017
 *      Author: devinc
 */

#include "api.h"
_pool_id message_pool;

_task_id dd_tcreate(int template_index, int deadline, int runtime) {
	_task_id task_id = _task_create(0, template_index, runtime);

	if (task_id == 0) {
	  printf("\nCould not create dd_scheduler task\n");
	  _task_block();
	}

	/*allocate a message*/
	SCHEDULER_MESSAGE_PTR msg_ptr = (SCHEDULER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (msg_ptr == NULL) {
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, scheuler_qid);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
	msg_ptr->TYPE = 0;
	msg_ptr->DEADLINE = deadline;
	msg_ptr->TASKID = _task_get_id();

	int result = _msgq_send(msg_ptr);

	if (result != TRUE) {
	 printf("\nCreate could not send a message\n");
	 // 0 is an error
	 return 0;
	}

	return task_id;
}

int dd_delete(int task_id) {
	/*allocate a message*/
	SCHEDULER_MESSAGE_PTR msg_ptr = (SCHEDULER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (msg_ptr == NULL) {
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, scheuler_qid);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
	msg_ptr->TYPE = 1;
	msg_ptr->TASKID = task_id;

	int result = _msgq_send(msg_ptr);

	if (result != TRUE) {
	 printf("\nCreate could not send a message\n");
	 // 0 is an error
	 return 0;
	}

	return task_id;
}

int dd_return_active_list(struct task_list **list) {
	return;
}

int dd_return_overdue_list(struct overdue_tasks **list) {
	return;
}



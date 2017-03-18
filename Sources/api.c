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

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, scheduler_qid);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
	msg_ptr->TYPE = 0;
	msg_ptr->DEADLINE = deadline;
	msg_ptr->TASKID = task_id;

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

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, scheduler_qid);
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
	/* open a message queue */
	_queue_id qid = _msgq_open(MSGQ_FREE_QUEUE, 0);

	if (qid == 0) {
	  printf("\nCould not open the return active message queue\n");
	  _task_block();
	}

	/*allocate a message*/
	SCHEDULER_MESSAGE_PTR msg_ptr = (SCHEDULER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (msg_ptr == NULL) {
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, scheduler_qid);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
	msg_ptr->TYPE = 2;

	int result = _msgq_send(msg_ptr);

	if (result != TRUE) {
	 printf("\nCreate could not send a message\n");
	 _task_block();
	}

	TASK_LIST_MESSAGE_PTR new_msg_ptr = _msgq_receive(qid, 0);


	(*list) = new_msg_ptr->task_list_head_ptr;
	_msgq_close(qid);
}

int dd_return_overdue_list(struct overdue_tasks **list) {
	/* open a message queue */
	_queue_id qid = _msgq_open(MSGQ_FREE_QUEUE, 0);

	if (qid == 0) {
	  printf("\nCould not open the return overdue message queue\n");
	  _task_block();
	}

	/*allocate a message*/
	SCHEDULER_MESSAGE_PTR msg_ptr = (SCHEDULER_MESSAGE_PTR)_msg_alloc(message_pool);

	if (msg_ptr == NULL) {
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, scheduler_qid);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(int) * 4;
	msg_ptr->TYPE = 3;

	int result = _msgq_send(msg_ptr);

	if (result != TRUE) {
	 printf("\nCreate could not send a message\n");
	 _task_block();
	}

	TASK_LIST_MESSAGE_PTR new_msg_ptr = _msgq_receive(qid, 0);
	(*list) = new_msg_ptr->overdue_tasks_head_ptr;
	_msgq_close(qid);
}



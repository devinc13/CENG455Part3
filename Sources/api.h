/*
 * api.h
 *
 *  Created on: Mar 6, 2017
 *      Author: devinc
 */

#ifndef SOURCES_API_H_
#define SOURCES_API_H_

#include "constants.h"


_task_id dd_tcreate(int template_index, int deadline, int runtime);
int dd_delete(int task_id);
int dd_return_active_list(struct task_list **list);
int dd_return_overdue_list(struct overdue_tasks **list);


#endif /* SOURCES_API_H_ */

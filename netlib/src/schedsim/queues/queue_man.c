/**
 * @file queue_man.c
 * Implementation of essential queue operations.
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#include <stdio.h>
#include "queue_man.h"
#include "error.h"

/**
 * Initialization of a Queue manager
 * @param qm : queue manager
 * @return Error code (defined in def.h and libs/error.h)
 */
int squeue_man_init (QUEUE_MAN *qm) {
  check_null_pointer(qm);
  qm->curr_queue = NULL;
  linked_list_man_init_conf(&qm->list, LL_CONF_STORE_ENTRY);
  return SUCCESS;
}

/**
 * Check the existence of the specific type in queue manager
 * @param qm : queue manager
 * @param type : queue type needs to be checked
 * @return Negative number if error in input. Return 1 if existing type in Queue manager. Otherwise return 0.
 */
static int queue_man_match_type (QUEUE_MAN *qm, int type) {
  LINKED_LIST *next = NULL;
  check_null_pointer(qm);
  next = qm->list.entries.next;
  while (next != &qm->list.entries) {
    QUEUE_TYPE *qt = container_of(next, QUEUE_TYPE, list_node);
    if (qt->type == type) {
      return 1;
    }
    next = next->next;
  }
  return 0;
}

/**
 * Register new queue-type into queue-manager
 * @param qm : queue manager
 * @param qi : queue type
 * @return Error code (defined in def.h and libs/error.h)
 */
int squeue_man_register_new_type (QUEUE_MAN *qm, QUEUE_TYPE *qi) {
  check_null_pointer(qm);
  check_null_pointer(qi);
  while (queue_man_match_type(qm, qi->type)) {
    // must change type of new queue_type
    // BUG: may loop forever :D
    qi->type++;
  }

  try ( linked_list_man_insert(&qm->list, &qi->list_node) );
  return SUCCESS;
}

/**
 * Remove a queue type out of queue-manager
 * @param qm : queue manager
 * @param qi : queue type
 * @return Error code (defined in def.h and libs/error.h)
 */
int squeue_man_unregister_type (QUEUE_MAN *qm, QUEUE_TYPE *qi) {
  check_null_pointer(qm);
  check_null_pointer(qi);
  try ( linked_list_man_remove(&qm->list, &qi->list_node) );
  return SUCCESS;
}

/**
 * Activate a queue-type in queue-manager. Simulation is based on this queue-type.
 * @param qm : queue manager
 * @param type : queue type
 * @return Error code (defined in def.h and libs/error.h)
 */
int squeue_man_activate_type (QUEUE_MAN *qm, int type) {
  LINKED_LIST *next = NULL;
  check_null_pointer(qm);
  next = qm->list.entries.next;
  while (next != &qm->list.entries) {
    QUEUE_TYPE *qt = container_of(next, QUEUE_TYPE, list_node);
    if (qt->type == type) {
      // activate this queue_type
      qm->curr_queue = qt;
      break;
    }
    next = next->next;
  }
  return SUCCESS;
}

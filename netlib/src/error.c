/**
 * @file error.c
 *
 * @date Created on: Jun 5, 2011
 * @author iizke
 */

#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "list/linked_list.h"

TRASH trash;

/**
 * Print out error message to file
 * @param msg : Message
 * @param fp : file pointer
 */
void error (char *msg, FILE * fp) {
  fprintf (fp, "ERROR: %s\n", msg);
  exit (1);
}

/**
 * Update new malloc link to garbage
 * @param g : garbage link
 * @return Error code
 */
int trash_collect_garbage(GARBAGE *g) {
  check_null_pointer(g);
  linked_list_insert(&trash.garbage_list, &g->list_node);
  return SUCCESS;
}

/**
 * Update new malloc link to garbage
 * @param g : garbage link
 * @return Error code
 */
int trash_remove_garbage(GARBAGE *g) {
  check_null_pointer(g);
  linked_list_remove(&g->list_node);
  return SUCCESS;
}

/**
 * Malloc with garbage collector
 * @param size : size of memory
 * @return pointer to new allocated memory
 */
void * gc_malloc(int size) {
  GARBAGE *g = NULL;
  g = (GARBAGE*)malloc(sizeof(GARBAGE) + size);
  if (!g) {
    iprint(LEVEL_WARNING, "Cannot allocate new memory \n");
    return NULL;
  }
  memset(g, 0,sizeof(GARBAGE) + size);
  linked_list_init(&g->list_node);
  g->link = (char*)g + sizeof(GARBAGE);
  trash_collect_garbage(g);
  return g->link;
}

void gc_free (void *p) {
  if (!p) return;
  GARBAGE *g = NULL;
  g = p - sizeof(GARBAGE);
  trash_remove_garbage (g);
  free(g);
  return;
}

/**
 * Empty trash
 * @return Error code
 */
int trash_clean () {
  GARBAGE *g = container_of(trash.garbage_list.prev, GARBAGE, list_node);

  while (g->list_node.prev != &g->list_node) {
    linked_list_remove(&g->list_node);
    free(g);
    g = container_of(trash.garbage_list.prev, GARBAGE, list_node);
  }
  return SUCCESS;
}

int trash_init() {
  linked_list_init(&trash.garbage_list);
  //linked_list_init(&trash.last_garbage);
  return SUCCESS;
}

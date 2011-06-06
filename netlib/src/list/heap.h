/**
 * @file heap.h
 * Heap structure
 *
 * @date Created on: Jun 6, 2011
 * @author iizke
 */

#ifndef HEAP_H_
#define HEAP_H_

typedef struct heap {
  int size;
  //int (*mapping) (void*);
  void* (*get_key) (void*);
  void **entries;
} HEAP;

typedef struct _item_heap {
  int id;
  float value;
} ITEM_HEAP;

int heap_init(HEAP **);
int heap_setup(HEAP *, int);
int heap_insert(HEAP *, void *, int);
int heap_remove(HEAP *, void *, int);

#endif /* HEAP_H_ */

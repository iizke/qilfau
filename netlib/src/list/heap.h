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

int heap_new(HEAP **);
int heap_init(HEAP *, int);
//int heap_insert(HEAP *, void *, int);
//int heap_remove(HEAP *, void *, int);
void heap_heapify(HEAP * h, int start);
void heap_build(HEAP * h);
void* heap_get_min(HEAP * h);
#endif /* HEAP_H_ */

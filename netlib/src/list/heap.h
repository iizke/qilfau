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
  int (*mapping) (double);
  void *entries;
} HEAP;

int heap_init(HEAP **);
int heap_setup(HEAP *);
int heap_insert(HEAP *, void *, int);
int heap_remove(HEAP *, void *, int);

#endif /* HEAP_H_ */

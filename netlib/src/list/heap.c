/**
 * @file heap.c
 * Heap implementation.
 * Inherit from graph/routing.c
 *
 * @date Created on: Jun 6, 2011
 * @author iizke
 */

#include "error.h"
#include "heap.h"

/**
 * Heap initialization
 * @param h : Heap
 * @return Error code (see more in def.h and error.h)
 */
int heap_new(HEAP **h) {
  check_null_pointer(h);
  (*h) = malloc_gc(sizeof(HEAP));
  check_null_pointer(*h);
  return SUCCESS;
}

static void * _heap_get_key (void * entry) {
  return NULL;
}

/**
 * Setup a heap with size
 * @param h : Heap
 * @param n : size of heap
 * @return Error code (see more in def.h and error.h)
 */
int heap_init(HEAP *h, int n) {
  check_null_pointer(h);
  h->entries = malloc_gc(n*sizeof(void*));
  check_null_pointer(h->entries);
  h->size = n;
  h->get_key = _heap_get_key;
  return SUCCESS;
}

/**
 * swap: swap two entries in a heap
 */
void heap_swap(HEAP * h, int a, int b) {
  void *tmp = NULL;
  if (h){
    tmp = h->entries[a];
    h->entries[a] = h->entries[b];
    h->entries[b] = tmp;
  }
}

/**
 * heapify: is called recursively to build-heap
 */
void heap_heapify(HEAP * h, int start) {
  int left, right, min, num_nodes = h->size;
  float* lkey = 0, *rkey = 0, *skey = 0, *mkey = 0;

  left = (start * 2) + 1;
  right = (start * 2) + 2;
  skey = h->get_key(h->entries[start]);
  mkey = skey;
  if (left < num_nodes) {
    lkey = h->get_key(h->entries[left]);
    if (lkey < skey) {
      min = left;
      mkey = lkey;
    } else
      min = start;
  }

  if (right < num_nodes) {
    rkey = h->get_key(h->entries[right]);
    if (rkey < mkey)
      min = right;
  }

  if (min != start) {
    heap_swap(h, start, min);
    heap_heapify(h, min);
  }
}

/**
 * Build a heap structure
 * @param h : Heap
 */
void heap_build(HEAP * h) {
  int i;

  for (i = (h->size / 2); i >= 0; i--)
    heap_heapify(h, i);
}

/**
 * heap_extract_mean: that the function
 * i) becomes the vector into a new heap (losing properties of the heap in case
 * of partial updates cost during construction of the tree of shortest paths and
 * ii) returns the first element of the structure (a minimum distance from the
 * source element
 */
void* heap_get_min(HEAP * h) {
  void* node_min_key = h->entries[0];

  heap_build(h);
  node_min_key = h->entries[0];
  h->entries[0] = h->entries[h->size - 1];
  return (node_min_key);
}

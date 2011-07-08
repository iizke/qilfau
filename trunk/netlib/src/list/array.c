/**
 * @file array.c
 * Array implementation
 *
 * @date Created on: Jun 12, 2011
 * @author iizke
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array.h"
#include "error.h"

int array_init (ARRAY** a, int len, int size) {
  check_null_pointer(a);
  *a = malloc(sizeof(ARRAY));
  check_null_pointer(*a);
  (*a)->data = NULL;
  array_setup(*a, len, size);
  return SUCCESS;
}

int array_setup(ARRAY* a, int len, int size) {
  check_null_pointer(a);
  if (len < 0 || size < 0)
    return ERR_INVALID_VAL;
  a->len = len;
  a->size = size;
  a->data = malloc_gc(len*size);
  if (! (a->data))
    return ERR_POINTER_NULL;
  return SUCCESS;
}

#define array_id_is_ok(_a,_id) ((_id >= 0) && (_id < _a->size))

POINTER_VAL array_get(ARRAY *a, int id) {
  POINTER_VAL ret;
  ret.pointer = NULL;
  if (!a)
    return (ret);
  if (! array_id_is_ok(a, id))
    return (ret);
  //ret = ((char*)a->data + id*a->len);
  return a->data[id];
}

int array_set(ARRAY* a, int id, void* data) {
  void *p = NULL;
  check_null_pointer(a);
  p = array_get(a, id).pointer;
  check_null_pointer(p);
  memcpy(p, data, a->len);
  return SUCCESS;
}

/**
 * @file array.h
 * Array implementation
 *
 * @date Created on: Jun 12, 2011
 * @author iizke
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include "def.h"

typedef struct array {
  POINTER_VAL* data;
  /// length of each data element
  int len;
  /// number of element
  int size;
} ARRAY;

int array_init (ARRAY**, int, int);
int array_setup(ARRAY*, int, int);
POINTER_VAL array_get(ARRAY *, int);
int array_set(ARRAY*, int, void*);

#endif /* ARRAY_H_ */

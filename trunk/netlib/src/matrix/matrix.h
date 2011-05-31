/**
 * @file matrix.h
 * Matrix data structure
 *
 * @date Created on: May 25, 2011
 * @author iizke
 */

#ifndef MATRIX_H_
#define MATRIX_H_
#include <stdio.h>

typedef struct edge {
  int source;
  int dest;
  void *data;
} EDGE;

typedef struct row {
  int nlinks;
  EDGE *links;
} ROW;

typedef struct matrix {
  int nnodes;
  ROW *links;
} MATRIX;

int matrix_get_row (MATRIX *, int row, ROW **);
int matrix_init (MATRIX **);
int matrix_setup (FILE *);
int matrix_build_index(MATRIX *, int);
void* matrix_get_value (MATRIX *, int, int);

#endif /* MATRIX_H_ */

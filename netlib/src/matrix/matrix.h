/**
 * @file matrix.h
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
  double weight;
} EDGE;

typedef struct row {
  int nlinks;
  EDGE *links;
} ROW;

typedef struct matrix {
  int nnodes;
  ROW *links;
} MATRIX;

int matrix_get_row (MATRIX *, ROW *);
int matrix_init (MATRIX *);
int matrix_setup (FILE *);
#endif /* MATRIX_H_ */

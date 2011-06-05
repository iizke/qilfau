/**
 * @file matrix.c
 *
 * @date Created on: May 25, 2011
 * @author iizke
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "matrix.h"

/**
 * Initialize a matrix structure.
 *
 * @param m : Matrix structure
 * @param type : type of matrix (dense or sparse). If not decided, fill with 0.
 * @return Error code (see in def.h and error.h)
 */
int matrix_init (MATRIX **m, int type) {
  *m = (MATRIX*)malloc (sizeof(MATRIX));
  check_null_pointer(*m);
  (*m)->type = type;
  return SUCCESS;
}

/**
 * Initialize dense-matrix
 * @param m : matrix structure
 * @param nr : number of rows
 * @param nc : number of columns
 * @param f : File
 * @return Error code (see more in def.h and error.h)
 */
int dense_matrix_init(DENSE_MATRIX **m, int nr, int nc, FILE *f) {
  DENSE_MATRIX * mx = NULL;
  int i = 0, j = 0;
  //int nrows = 0, ncols = 0;
  mx = *m = malloc_gc (sizeof(DENSE_MATRIX));
  check_null_pointer(mx);

  mx->vals = malloc_gc(sizeof(float*) * nr);
  check_null_pointer(mx->vals);

  for (i = 0; i < nr; i++) {
    mx->vals[i] = malloc_gc(sizeof(float) * nc);
    check_null_pointer(mx->vals[i]);
  }
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      /// No check wrong input here!
      fscanf(f, "%f", &mx->vals[i][j]);

  return SUCCESS;
}

/**
 * Setup a matrix structure from file.
 * At this time, only setup matrix following dense structure
 *
 * @param m : matrix structure
 * @param f : file containing matrix value
 * @return Error code (see more in def.h and error.h)
 */
int matrix_setup (MATRIX *m, FILE* f) {
  check_null_pointer(m);
  check_null_pointer(f);
  fscanf(f, "%d %d", &m->nrows, &m->ncols);
  try ( dense_matrix_init(&m->data._dense, m->nrows, m->ncols, f) );
  return SUCCESS;
}

/**
 * Extract cell value of matrix
 * @param m : Matrix
 * @param row : row position
 * @param col : column position
 * @return : cell value (real number)
 */
float matrix_get_value (MATRIX *m, int row, int col) {
  check_null_pointer(m);
  switch (m->type) {
  case MATRIX_TYPE_DENSE:
    return m->data._dense->vals[row][col];
  case MATRIX_TYPE_SPARSE:
    return m->data._sparse->rows[row].cols[col].data;
  default:
    return 0;
  }
  return 0;
}

int matrix_build_index(MATRIX *m, int type) {
  return SUCCESS;
}

int matrix_get_row (MATRIX *m, int row, ROW **r) {
  return SUCCESS;
}

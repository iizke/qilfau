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
  if (*m != NULL)
    iprint(LEVEL_WARNING, "matrix initted is not NULL. Take care of it.\n");

  *m = (MATRIX*)malloc_gc (sizeof(MATRIX));
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
int dense_matrix_init(DENSE_MATRIX **m, int nr, int nc) {
  DENSE_MATRIX * mx = NULL;
  int i = 0;
  if (*m != NULL)
    iprint(LEVEL_WARNING, "Dense matrix initted is not NULL.");
  //int nrows = 0, ncols = 0;
  mx = *m = malloc_gc (sizeof(DENSE_MATRIX));
  check_null_pointer(mx);

  mx->vals = malloc_gc(sizeof(float*) * nr);
  check_null_pointer(mx->vals);

  for (i = 0; i < nr; i++) {
    mx->vals[i] = malloc_gc(sizeof(float) * nc);
    check_null_pointer(mx->vals[i]);
  }
  return SUCCESS;
}

int dense_matrix_setup_file(DENSE_MATRIX *m, int nr, int nc, FILE *f) {
  int i = 0, j = 0;
  check_null_pointer(m);
  check_null_pointer(f);
  //int nrows = 0, ncols = 0;
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      /// No check wrong input here!
      fscanf(f, "%f", &m->vals[i][j].value);

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
int matrix_setup_file (MATRIX *m, FILE* f) {
  check_null_pointer(m);
  check_null_pointer(f);
  fscanf(f, "%d %d", &m->nrows, &m->ncols);
  switch (m->type) {
  case MATRIX_TYPE_DENSE:
    try ( dense_matrix_init(&m->data._dense, m->nrows, m->ncols) );
    dense_matrix_setup_file(m->data._dense, m->nrows, m->ncols, f);
    break;
  case MATRIX_TYPE_SPARSE:
  default:
    break;
  }
  return SUCCESS;
}

int matrix_setup (MATRIX *m, int nr, int nc) {
  check_null_pointer(m);

  m->nrows = nr;
  m->ncols = nc;
  switch (m->type) {
  case MATRIX_TYPE_DENSE:
    try ( dense_matrix_init(&m->data._dense, nr, nc) );
    break;
  case MATRIX_TYPE_SPARSE:
  default:
    break;
  }
  return SUCCESS;
}

/**
 * Extract cell value of matrix
 * @param m : Matrix
 * @param row : row position
 * @param col : column position
 * @return : cell value (real number)
 */
MATRIX_VAL matrix_get_value (MATRIX *m, int row, int col) {
  MATRIX_VAL val;
  val.value = 0;
  val.pointer = NULL;

  if (!m)
    return val;

  switch (m->type) {
  case MATRIX_TYPE_DENSE:
    return m->data._dense->vals[row][col];
  case MATRIX_TYPE_SPARSE:
    return m->data._sparse->rows[row].cols[col].data;
  default:
    return val;
  }
  return val;
}

void* matrix_get_row (MATRIX *m, int row) {
  if (!m) return NULL;
  switch (m->type) {
  case MATRIX_TYPE_DENSE:
    return m->data._dense->vals[row];
  case MATRIX_TYPE_SPARSE:
    return &m->data._sparse->rows[row];
  default:
    break;
  }
  return NULL;
}

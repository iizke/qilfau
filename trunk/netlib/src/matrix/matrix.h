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
#include "def.h"

typedef struct column {
  int id;
  POINTER_VAL data;
} COLUMN;

typedef struct row {
  int id;
  int ncols;
  COLUMN *cols;
} ROW;

typedef struct sparse_data_matrix {
  int nrows;
  ROW *rows;
} SPARSE_MATRIX;

typedef struct dense_data_matrix {
  POINTER_VAL** vals;
}DENSE_MATRIX;

typedef struct matrix {
  int type;
  int nrows;
  int ncols;
  union {
    DENSE_MATRIX *_dense;
    SPARSE_MATRIX *_sparse;
  } data;
} MATRIX;

#define MATRIX_TYPE_DENSE           1
#define MATRIX_TYPE_SPARSE          2

int matrix_new (MATRIX **, int);
int matrix_init (MATRIX *m, int nr, int nc);
int matrix_setup_file (MATRIX *, FILE*);
POINTER_VAL matrix_get_value (MATRIX *, int, int);
void* matrix_get_row (MATRIX *, int row);
int matrix_set_value (MATRIX *m, int row, int col, POINTER_VAL val);
#endif /* MATRIX_H_ */

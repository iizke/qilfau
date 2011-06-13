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

typedef union {
  void* pointer;
  float value;
} MATRIX_VAL;

typedef struct column {
  int id;
  MATRIX_VAL data;
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
  MATRIX_VAL** vals;
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

int matrix_init (MATRIX **, int);
int matrix_setup (MATRIX *m, int nr, int nc);
int matrix_setup_file (MATRIX *, FILE*);
MATRIX_VAL matrix_get_value (MATRIX *, int, int);
void* matrix_get_row (MATRIX *, int row);

#endif /* MATRIX_H_ */

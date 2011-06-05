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

typedef struct column {
  int id;
  float data;
} COLUMN;

typedef struct row {
  int id;
  int ncols;
  COLUMN *cols;
} ROW;

typedef struct sparse_matrix {
  int nrows;
  ROW *rows;
} SPARSE_MATRIX;

typedef struct dense_matrix {
  float** vals;
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

int matrix_get_row (MATRIX *, int row, ROW **);
int matrix_init (MATRIX **, int);
int matrix_setup (MATRIX *, FILE*);
int matrix_build_index(MATRIX *, int);
float matrix_get_value (MATRIX *, int, int);

#endif /* MATRIX_H_ */

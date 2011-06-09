/**
 * @file netlib.c
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

#include <stdio.h>
#include "error.h"
#include "netsim/netsim.h"
#include "graph/main_graph.h"
#include "graph/main_rwa.h"
#include "matrix/matrix.h"

int check_netsim () {
  netsim_start_thread("src/netsim/conf/test.conf");
  return 0;
}

int check_matrix () {
  MATRIX m;
  FILE *f = fopen("src/matrix/matrix.txt", "r");
  int i, j;
  m.type = MATRIX_TYPE_DENSE;
  matrix_setup(&m, f);
  fclose(f);
  for (i = 0; i < m.nrows; i++)
    for (j = 0; j < m.ncols; j++)
      printf("[%d][%d] = %f\n", i,j, matrix_get_value(&m,i,j));

  return 0;
}

int main (int nargs, char** args) {
  trash_init();
  random_init();
  //check_netsim();
  main_graph(nargs, args);
  //check_matrix();
  trash_clean();
  return SUCCESS;
}

/**
 * @file netlib.c
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

//#include <stdio.h>
#include "error.h"
#include "netsim/netsim.h"
#include "poligraph/main_graph.h"
#include "matrix/matrix.h"
#include "graph/graph.h"
#include "netsim/conf/config.h"
#include "netsim/conf/channelparser.h"
#include "netsim/netqueue.h"
#include "schedsim/schedsim.h"
#include "netsimv2/onequeue/onequeue.h"

int check_netsim () {
  //netsim_start_thread("src/netsim/conf/test.conf");
  netsim_start("src/netsim/conf/test.conf");
  return 0;
}

int check_schedsim () {
  //netsim_start_thread("src/netsim/conf/test.conf");
  schedsim_start("src/schedsim/conf/test.conf");
  return 0;
}

int check_matrix () {
  MATRIX m;
  FILE *f = fopen("src/matrix/matrix.txt", "r");
  int i, j;
  m.type = MATRIX_TYPE_DENSE;
  matrix_setup_file(&m, f);
  fclose(f);
  for (i = 0; i < m.nrows; i++)
    for (j = 0; j < m.ncols; j++)
      printf("[%d][%d] = %f\n", i,j, matrix_get_value(&m,i,j).value);

  return 0;
}

int check_graph() {
  GRAPH g;
  graph_setup_matrix(&g, 5);
  return 0;
}

int check_net_queue() {
  //netq_run("");
  nqthr_start("");
  return SUCCESS;
}

int main (int nargs, char** args) {
  trash_init();
  random_init();
  //main_markov_parser(nargs, args);

  check_netsim();

  //check_schedsim();
  //main_graph(nargs, args);
  //main_rwa(nargs, args);
  //check_matrix();
  //check_graph();
  //check_net_queue();
  //test_gen_distribution();
  trash_clean();
  return SUCCESS;
}

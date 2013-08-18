/**
 * @file netlib.c
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

//#include <stdio.h>
#include <signal.h>
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
#include "optimal/knapsack.h"

int check_netsim () {
  //netsim_start_thread("src/netsim/conf/test.conf");
  netsim_start("src/netsim/conf/test_onequeue.conf");
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

int check_babsq () {
  //netsim_start_thread("src/netsim/conf/test.conf");
  babs_start("src/netsim/conf/test_babs.conf");
  printf("Check BABS Queue with Sched1 -----------------\n");
  babs_start("src/netsim/conf/test_babs_sched1.conf");
  return 0;
}

/**
 * Signal handler (SIGINT and SIGTERM) used whenever main process is stopped by
 * user (normally with Ctrl+C).
 * @param n : interrupt number (no use)
 */
static void main_sig_handler(int n) {
  printf("Trash cleaning ... ");
  trash_clean();
  //printf("Terminated.");
  exit(1);
}

int main (int nargs, char** args) {
  trash_init();
  signal(SIGINT, main_sig_handler);
  signal(SIGTERM, main_sig_handler);
  random_init();
  printf("Seed value: %ld\n", irand_get_seed());
  //main_markov_parser(nargs, args);

  //printf("CHECK ONE_QUEUE -------------------------\n");
  //check_netsim();

  //check_schedsim();
  //main_graph(nargs, args);
  //main_rwa(nargs, args);
  //check_matrix();
  //check_graph();
  //check_net_queue();
  //test_gen_distribution();

  //printf("CHECK BABS_QUEUE -------------------------\n");
  //check_babsq();

  knapsack01_test();
  trash_clean();
  return SUCCESS;
}

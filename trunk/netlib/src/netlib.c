/**
 * @file netlib.c
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

#include <stdio.h>
#include "error.h"
#include "netsim/netsim.h"
#include "poligraph/main_graph.h"
#include "matrix/matrix.h"
#include "graph/graph.h"
#include "netsim/conf/config.h"
#include "netsim/conf/channelparser.h"

int check_netsim () {
  netsim_start_thread("src/netsim/conf/test.conf");
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

int check_channel_parser () {
  extern FILE *nqin;
  extern int nqlex();
  extern int nqparse();
  extern NET_CONFIG netconf;
  LINKED_LIST *t = &netconf.channels;

  netconfig_init(&netconf, 2);
  nqin = fopen("src/netsim/conf/netconf.conf", "r");
  nqparse();
  while (&netconf.channels != t->next) {
    CHANNEL_CONF *c = container_of(t->next, CHANNEL_CONF, list_node);
    printf ("channel src %d dest %d delay %f \n", c->src, c->dest, c->delay.constval);
    t = t->next;
  }

  return SUCCESS;
}

int main (int nargs, char** args) {
  trash_init();
  random_init();
  //check_netsim();
  //main_graph(nargs, args);
  //check_matrix();
  //check_graph();
  check_channel_parser();

  trash_clean();
  return SUCCESS;
}

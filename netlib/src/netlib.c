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

int check_netsim () {
  netsim_start("/home/iizke/projects/netlib/src/netsim/conf/test.conf");
  return 0;
}

int main (int nargs, char** args) {
  //check_netsim();
  main_graph(nargs, args);
  return SUCCESS;
}

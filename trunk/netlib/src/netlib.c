/**
 * @file netlib.c
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

#include <stdio.h>
#include "error.h"
#include "netsim/netsim.h"

int check_netsim () {
  netsim_start("/home/iizke/projects/netlib/src/netsim/conf/test.conf");
  return 0;
}

int main (int nargs, char** args) {
  check_netsim();
  return SUCCESS;
}

/**
 * @file netlib.c
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

#include <stdio.h>

#include "netsim/netsim.h"

int main (int nargs, char** args) {
  printf("hello \n");
  netsim_start("/home/iizke/projects/netlib/src/netsim/conf/test.conf");
  return 0;
}

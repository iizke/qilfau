/**
 * @file network.c
 *
 * @date Created on: May 25, 2011
 * @author iizke
 */

#include <stdlib.h>
#include "error.h"
#include "network.h"

SP_DIJKSTRA_LIST* _nw_shortest_path_dijkstra (NETWORK *net, int src) {
  SP_DIJKSTRA_LIST *sol_list;
  check_null_pointer(net);
  int n = net->costs.nnodes;

  sol_list = malloc(sizeof(SP_DIJKSTRA_LIST));
  linked_list_man_init(sol_list);
  while (n>0) {

  }
  return NULL;
}

void* network_find_shortest_path (NETWORK *net, int alg, int src, int dest) {
  switch (alg) {
  case NETWORK_SP_DIJKSTRA:
    return _nw_shortest_path_dijkstra(net, src);
    break;
  case NETWORK_SP_BELLFORD:
    break;
  default:
    break;
  }
  return 0;
}

/**
 * @file network.h
 * Network structure: traffic, cost, ...
 *
 * @date Created on: May 25, 2011
 * @author iizke
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "list/linked_list.h"
#include "matrix/matrix.h"

typedef struct shortest_path_dijkstra {
  LINKED_LIST list_node;
  int node;
  int last_node;
  double total_cost;
} SP_DIJKSTRA;

typedef LINKED_LIST_MAN SP_DIJKSTRA_LIST;

typedef struct network {
  MATRIX costs;
  //MATRIX capacities;
} NETWORK;

#define NETWORK_SP_DIJKSTRA           1
#define NETWORK_SP_BELLFORD           2

void* network_find_shortest_path (NETWORK *, int, int, int);

#endif /* NETWORK_H_ */

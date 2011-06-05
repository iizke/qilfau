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

typedef struct shortest_path_distance {
  LINKED_LIST list_node;
  int end_node;
  int last_node;
  float distance;
} SP_DISTANCE;

typedef struct sp_distance_list {
  LINKED_LIST list;
}SP_DISTANCE_LIST;

typedef struct network {
  MATRIX costs;
  //MATRIX capacities;
} NETWORK;

//typedef struct network_node {
//  int id;
//} NW_NODE;


typedef struct network_node_list {
  float *data;
  int nnodes;
  int next_scan;
} NW_NODE_LIST;

#define NETWORK_SP_DIJKSTRA           1
#define NETWORK_SP_BELLFORD           2

void* nw_find_shortest_path (NETWORK *, int, int, int);
#define nw_get_cost(nw,r,c) matrix_get_value(&nw->costs, r, c)
int nw_get_neighbor(NETWORK*, int, NW_NODE_LIST*);

int nw_node_list_scan(NW_NODE_LIST*, int*);

int sp_distance_list_init(SP_DISTANCE_LIST **list, int nentries, int src);
int sp_distance_list_remove_smallest(SP_DISTANCE_LIST *, SP_DISTANCE **);
int sp_distance_list_insert(SP_DISTANCE_LIST *, SP_DISTANCE*);
float sp_distance_list_get_value(SP_DISTANCE_LIST *, int);
int sp_distance_list_update(SP_DISTANCE_LIST*, int, int, float);
int sp_distance_list_is_empty(SP_DISTANCE_LIST*);
SP_DISTANCE* sp_distance_list_get(SP_DISTANCE_LIST *l, int id);
#endif /* NETWORK_H_ */

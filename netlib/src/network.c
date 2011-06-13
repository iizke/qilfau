/**
 * @file network.c
 * Networking problem of operation research: For example Shortest path,
 * Min Cost Flow, ...
 *
 * @date Created on: May 25, 2011
 * @author iizke
 */

#include <stdlib.h>
#include "error.h"
#include "network.h"

int nw_get_neighbor(NETWORK* nw, int node, NW_NODE_LIST* neighbor) {
  neighbor->data = nw->costs.data._dense->vals[node];
  neighbor->nnodes = nw->costs.ncols;
  neighbor->next_scan = 0;
  return SUCCESS;
}

int nw_node_list_scan(NW_NODE_LIST* nl, int *nodeid) {
  int i = 0;
  i = nl->next_scan;
  while (i < nl->nnodes) {
    if (nl->data[i] != 0) {
      *nodeid = i;
      nl->next_scan = i+1;
      return TRUE;
    }
    i++;
  }
  //nl->next_scan = 0;
  return FALSE;
}

int sp_distance_list_init0(SP_DISTANCE_LIST **list) {
  *list = malloc_gc(sizeof(SP_DISTANCE_LIST));
  check_null_pointer(*list);
  linked_list_init(&(*list)->list);
  return SUCCESS;
}

int sp_distance_list_init(SP_DISTANCE_LIST **list, int nentries, int src) {
  SP_DISTANCE *d_list = NULL;
  int i;

  sp_distance_list_init0(list);

  d_list = malloc_gc(sizeof(SP_DISTANCE)*nentries);
  check_null_pointer(d_list);
  for (i=0; i<nentries; i++) {
    linked_list_insert(&(*list)->list, &d_list[i].list_node);
    d_list[i].distance = -1;
    d_list[i].end_node = i;
    d_list[i].last_node = -1;
  }
  d_list[0].distance = 0;
  d_list[0].last_node = 0;

  return SUCCESS;
}

int sp_distance_list_remove_smallest(SP_DISTANCE_LIST *l, SP_DISTANCE **d) {
  SP_DISTANCE *next = NULL;
  *d = container_of(l->list.next, SP_DISTANCE, list_node);
  next = *d;
  while (next->list_node.next != next->list_node.prev) {
    if (next->distance < (*d)->distance)
      *d = next;
    next = container_of(next->list_node.next, SP_DISTANCE, list_node);
  }
  linked_list_remove(&(*d)->list_node);
  return SUCCESS;
}

int sp_distance_list_insert(SP_DISTANCE_LIST *l, SP_DISTANCE*d) {
  check_null_pointer(l);
  check_null_pointer(d);
  try ( linked_list_insert(&l->list, &d->list_node) );
  return SUCCESS;
}

SP_DISTANCE* sp_distance_list_get(SP_DISTANCE_LIST *l, int id) {
  SP_DISTANCE *first = NULL;
  if (!l) return NULL;
  first = container_of(l->list.next, SP_DISTANCE, list_node);
  return &first[id];
}

float sp_distance_list_get_value(SP_DISTANCE_LIST *l, int id) {
  SP_DISTANCE *d = NULL;
  check_null_pointer( d = sp_distance_list_get(l,id) );
  return d[id].distance;
}

int sp_distance_list_update(SP_DISTANCE_LIST* l, int end_node, int last_node, float d) {
  SP_DISTANCE * sp_d = sp_distance_list_get(l, end_node);
  check_null_pointer(sp_d);
  sp_d->distance = d;
  sp_d->last_node = last_node;
  return SUCCESS;
}

int sp_distance_list_is_empty(SP_DISTANCE_LIST* l) {
  return linked_list_is_empty(&l->list);
}

SP_DISTANCE_LIST* _nw_shortest_path_dijkstra (NETWORK *net, int src) {
  SP_DISTANCE_LIST *sol_list = NULL;
  SP_DISTANCE_LIST *distances = NULL;
  SP_DISTANCE *sol = NULL;
  int n = net->costs.nrows;
  int nodeid = 0;
  NW_NODE_LIST neighbor;
  //int i = 0;

  if (!net) return NULL;
  sp_distance_list_init(&distances, n, src);
  sp_distance_list_init0(&sol_list);

  while ( ! sp_distance_list_is_empty(distances) ) {
    sp_distance_list_remove_smallest(distances, &sol);
    sp_distance_list_insert(sol_list, sol);
    nw_get_neighbor(net, sol->end_node, &neighbor);
    while (nw_node_list_scan(&neighbor, &nodeid)) {
      float new_distance = sol->distance + nw_get_cost(net, sol->end_node, nodeid).value;
      float curr_distance = sp_distance_list_get_value(distances, nodeid);
      if (curr_distance > new_distance || curr_distance < 0 ) {
        // update new distance
        sp_distance_list_update(distances, nodeid, sol->end_node, new_distance);
      }
    }
  }
  return NULL;
}

void* nw_find_shortest_path (NETWORK *net, int alg, int src, int dest) {
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

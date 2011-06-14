/**
 * @file graph.c
 * Graph function
 *
 * @date Created on: Jun 11, 2011
 * @author iizke
 */

#include <string.h>
#include "graph.h"
#include "error.h"
#include "matrix/matrix.h"

int node_list_setup (NODE_LIST *nl) {
  check_null_pointer(nl);
  memset(nl, 0, sizeof(NODE_LIST));
  return SUCCESS;
}

int edge_list_setup (EDGE_LIST *el) {
  check_null_pointer(el);
  memset(el, 0, sizeof(EDGE_LIST));
  return SUCCESS;
}

int edge_list_setup_matrix (EDGE_LIST *el, int n) {
  check_null_pointer(el);
  if (!el->edge_data) {
    matrix_init(&el->edge_data, MATRIX_TYPE_DENSE);
    matrix_setup(el->edge_data, n, n);
    memset(el, 0, sizeof(EDGE_LIST));
  }
  return SUCCESS;
}

int graph_setup (GRAPH* g) {
  check_null_pointer(g);
  edge_list_setup(&g->edges);
  node_list_setup(&g->nodes);
  return SUCCESS;
}

void* edge_list_matrix_get_edge_from(EDGE_LIST *el, NODE_ID id) {
  MATRIX *m = NULL;
  if (!el)
    return NULL;
  m = el->edge_data;
  return m->data._dense->vals[id];
}

void* edge_list_matrix_get_edge (EDGE_LIST *el, NODE_ID id1, NODE_ID id2) {
  return matrix_get_value(el->edge_data, id1, id2).pointer;
}

int edge_list_matrix_set_edge (EDGE_LIST *el, NODE_ID id1, NODE_ID id2, char* val) {
  MATRIX_VAL v;
  check_null_pointer(el);
  v = matrix_get_value(el->edge_data, id1, id2);
  v.pointer = val;
  return SUCCESS;
}

void* node_list_matrix_get_node (NODE_LIST* nl, NODE_ID id) {
  char **val = NULL;
  if ((!nl) || (id >= nl->num_nodes) || (id < 0))
    return NULL;
  val = nl->node_data;
  return val[id];
}

int node_list_matrix_set_node (NODE_LIST* nl, NODE_ID id, void* data) {
  char **val = NULL;
  if ((!nl) || (id >= nl->num_nodes) || (id < 0))
    return NULL;
  val = nl->node_data;
  val[id] = data;
  return SUCCESS;
}

int graph_setup_matrix (GRAPH* g, int n) {
  try ( graph_setup(g) );
  edge_list_setup_matrix(&g->edges, n);
  g->edges.get_edge = edge_list_matrix_get_edge;
  g->edges.get_edge_from = edge_list_matrix_get_edge_from;
  g->edges.set_edge = edge_list_matrix_set_edge;
  //g->edges.scan_edge_to = edge_list_matrix_scan_edge_to;
  g->nodes.num_nodes = n;
  g->nodes.node_data = malloc_gc(sizeof(void*)*n);
  memset(g->nodes.node_data, 0, sizeof(void*)*n);
  g->nodes.get_node = node_list_matrix_get_node;
  g->nodes.set_node = node_list_matrix_set_node;
  return SUCCESS;
}

void* graph_get_neighbor(GRAPH*g, int r,int from_col) {
  int i = 0;
  void *val = NULL;
  if (!g) return NULL;

  for (i=from_col; i<g->nodes.num_nodes; i++) {
    val = g->edges.get_edge(&g->edges, r, i);
    if (val)
      return g->nodes.get_node(&g->nodes, i);
  }
  return NULL;
}

/**
 * @file graph.h
 * Graph data structure
 *
 * @date Created on: Jun 11, 2011
 * @author iizke
 */

#ifndef GRAPH_H_
#define GRAPH_H_

typedef int NODE_ID;

typedef struct node_list {
  void* node_data;
  int num_nodes;
  NODE_ID next_scan;
  void* (*get_node) (void*, NODE_ID);
  int (*set_node) (void*, NODE_ID, void*);
} NODE_LIST;

typedef struct edge_list {
  void* edge_data;
  //int num_edges;
  //void* (*scan_edge_to)(void*, NODE_ID, NODE_ID);
  void* (*get_edge_from)(void*, NODE_ID);
  void* (*get_edge) (void*, NODE_ID, NODE_ID);
  int (*set_edge) (void*, NODE_ID, NODE_ID, void*);
} EDGE_LIST;

typedef struct graph {
  NODE_LIST nodes;
  EDGE_LIST edges;
} GRAPH;

int graph_init (GRAPH*);
int graph_setup_matrix (GRAPH* g, int n) ;
void* graph_get_end_neighbor(GRAPH*g, int r,int from_col);
void* graph_get_head_neighbor(GRAPH*g, int r, int from_col);

#endif /* GRAPH_H_ */

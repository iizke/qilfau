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

int graph_setup (GRAPH*);
int graph_setup_matrix (GRAPH* g, int n) ;
void* graph_traverse_in_row(GRAPH*g, int r,int from_col);
#endif /* GRAPH_H_ */

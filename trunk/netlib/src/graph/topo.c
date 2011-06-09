/**
 * @file topo.c
 *
 * @date Created on: Jun 9, 2011
 * @author iizke
 */

#include <string.h>
#include "main_graph.h"

/**
 * Build simple ring topology with 1 transmiter, 1 receiver
 * @param g : GRAPH
 * @param num_nodes
 * @return 0 (if success)
 */
int build_ring_topology1(PGRAPH g) {
  int i = 0;
  for (i = 0; i < g->num_nodes; i++) {
    int dest = (i + 1) % g->num_nodes;
    add_link(g, i, dest, 10);
  }
  return 0;
}

/**
 *  Build random ring topology.
 *  Method: Generate random node ID and connect to the previous node (the first
 *  node is always 0) so that the generated node ID is not matched to any
 *  assigned nodes.
 * @param g : Graph
 * @param num_tx  : number of transmitters also number of receivers
 * @return 0 (if success)
 */
int build_ring_topology2(PGRAPH g, int num_tx) {
  int i = 0, j = 0, k = 0;
  int src, dest;
  int * check = malloc(sizeof(int) * g->num_nodes);
  for (j = 0; j < num_tx; j++) {
    memset(check, -1, num_tx * sizeof(int));
    check[0] = 0;
    for (i = 1; i < g->num_nodes; i++) {
      src = check[i - 1];
      k = 0;
      while (k != i) {
        dest = polirand_randint(1, g->num_nodes - 1, &seed);
        for (k = 0; k < i; k++)
          if (check[k] == dest)
            break;
      }
      check[i] = dest;
      //if (find_link(g, first_link_out (g, src), src, dest) == NULL)
      add_link(g, src, dest, 0.0);
    }
    src = check[g->num_nodes - 1];
    //if (find_link(g, first_link_out (g, src), src, 0) == NULL)
    add_link(g, src, 0, 0.0);
  }
  return 0;
}

double print_maxflow(PGRAPH g) {
  int i;//, k;
  PLINK p_link;
  //double d, db;
  //double tot_weighted_d = 0.0, tot_weighted_db = 0.0, tot_flow = 0.0;
  //double tot_d = 0.0, tot_db = 0.0, tot_link = 0.0;
  double max_flow = 0;

  for (i = 0; i < num_nodes_graph (g); i++) {
    for (p_link = first_link_out (g, i); p_link != NULL; p_link
        = next_link (p_link)) {
      //d = 1500.0 / ((link_capacity (p_link) - link_flow (p_link))) / 1000000.0;
      //db = 1500.0 / ((link_broken_capacity (p_link)
      //    - link_broken_max_flow (p_link))) / 1000000.0;

      //tot_d += d;
      //tot_weighted_d += d * link_flow (p_link);
      //tot_weighted_db += db * link_flow (p_link);
      //tot_flow += link_flow (p_link);
      max_flow = (max_flow < link_flow(p_link)) ? (link_flow(p_link)) : max_flow;
      //tot_db += db;
      //tot_link++;
      //              print_path_item(fp, p_link);
    }
  }
  return max_flow;
}

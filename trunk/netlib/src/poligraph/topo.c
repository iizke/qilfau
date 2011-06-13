/**
 * @file topo.c
 *
 * @date Created on: Jun 9, 2011
 * @author iizke
 */

#include <string.h>
#include <time.h>
#include "main_graph.h"
#include "random.h"

/**
 * Build simple ring topology with 1 transmiter, 1 receiver
 * @param g : POLIGRAPH
 * @param num_nodes
 * @return 0 (if success)
 */
int topo_build_ring1(PGRAPH g) {
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
int topo_build_ring2(PGRAPH g, int num_tx) {
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
        //dest = polirand_randint(1, g->num_nodes - 1, &seed);
        dest = gen_int_uniform(1, g->num_nodes - 1);
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

double topo_print_maxflow(PGRAPH g) {
  int i;
  PLINK p_link;
  double max_flow = 0;

  for (i = 0; i < num_nodes_graph (g); i++) {
    for (p_link = first_link_out (g, i); p_link != NULL; p_link
        = next_link (p_link)) {
       max_flow = (max_flow < link_flow(p_link)) ? (link_flow(p_link)) : max_flow;
    }
  }
  return max_flow;
}


//int do_experiment(int nrepl, FILE *fp_result) {
//  int nstep = 4;
//  int dstep = 1;
//  int num_nodes = 10;
//  int delta = 1;
//  int MAX_N = 100;
//  int MAX_D = 10;
//  int i = 0;
//
//  for (num_nodes = 10; num_nodes < MAX_N; num_nodes += nstep)
//    for (delta = 1; delta < MAX_D; delta += dstep)
//      for (i = 0; i < nrepl; i++)
//        _run(num_nodes, delta, fp_result);
//
//  return 1;
//}

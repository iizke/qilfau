/**
 * @file topo.h
 *
 * @date Created on: Jun 9, 2011
 * @author iizke
 */

#ifndef TOPO_H_
#define TOPO_H_

#include "graph.h"

int topo_build_ring(PGRAPH g);
int topo_build_random_ring(PGRAPH g, int num_tx);
int topo_build_random_ring1(PGRAPH g, int num_tx);
int topo_build_ER_graph(PGRAPH g, double prob);
int topo_build_greedy(PGRAPH g, int delta);
double topo_print_maxflow (PGRAPH g);

#endif /* TOPO_H_ */

/**
 * @file topo.h
 *
 * @date Created on: Jun 9, 2011
 * @author iizke
 */

#ifndef TOPO_H_
#define TOPO_H_

#include "graph.h"

int build_ring_topology1(PGRAPH g);
int build_ring_topology2(PGRAPH g, int num_tx);
double print_maxflow (PGRAPH g);

#endif /* TOPO_H_ */

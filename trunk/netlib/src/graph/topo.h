/**
 * @file topo.h
 *
 * @date Created on: Jun 9, 2011
 * @author iizke
 */

#ifndef TOPO_H_
#define TOPO_H_

#include "graph.h"

int topo_build_ring1(PGRAPH g);
int topo_build_ring2(PGRAPH g, int num_tx);
double topo_print_maxflow (PGRAPH g);

#endif /* TOPO_H_ */

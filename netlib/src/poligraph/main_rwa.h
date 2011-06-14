/* main.h -- 20/01/01 */
#ifndef MAIN_RWA_H
#define MAIN_RWA_H

#include "main_graph.h"

#define MAX_REQ 200
#define LIBERO -1

/* prototipi per rwa.c */
void alloca_richieste(int ***L, int num_req);
void crea_richieste(int **L, int num_req, int num_nodes);
void stampa_richieste(int **L, int num_req);

void alloca_lambda(int ***W, int num_req);
void print_lambda(int **W, int num_req);
void max_lambda(int **lambda, int num_req);


void RWA_ssff(PGRAPH g, int **L, int **lambda, int num_req);

int main_rwa (int argc, char *argv[]);

#endif

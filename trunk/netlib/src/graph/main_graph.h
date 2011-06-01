/* main.h -- 20/01/01 */
#ifndef MAIN_GRAPH_H
#define MAIN_GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include	<math.h>
/*----------------------------------------------------------------------------*
  Tipi di dato scalare
*----------------------------------------------------------------------------*/

#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

#define CONNECTED TRUE
#define DISCONNECTED FALSE

#define ADD	0
#define DELETE	1
#define NO_MOVE 0
#define NEIGH_MOVE 1
#define BEST_MOVE	2
#define DELAY_MEAN	0
#define DEALAY_MAX	1

typedef double TRAFFIC;
typedef double CAPACITY;
typedef int NODEID;
typedef double COST;
#define MAX_COST MAX_DOUBLE

#include "graph.h"
#include "fault.h"
#include "print_structures.h"
#include "routing.h"
#include "matrix.h"
#include "polirand/random.h"
#include "error.h"
#include "def.h"

										/* variabili globali */

#ifdef MAIN_C
PITEM p_PATH_ITEM_free;
PLINK p_LINK_free;
PLINKLIST p_LINKLIST_free;
GRAPH g;
MATRIX trf_m, weight_route;
long int seed;
#else
extern PITEM p_PATH_ITEM_free;
extern PLINK p_LINK_free;
extern PLINKLIST p_LINKLIST_free;
extern GRAPH g;
extern MATRIX trf_m, dist_m, weight_route;
extern long int seed;
extern int length_pck;
#endif


/* Prototipi per graph.c     */
/* i) allocazione ed inizializzazione grafo con relative funzioni di gestione 
      liste free (malloc/free)	   */

PATH_ITEM *malloc_PATH_ITEM (void);
void free_PATH_ITEM (PITEM p);
PLINKLIST malloc_linklist (void);
void free_linklist (PLINKLIST p);
PLINKLIST find_linklist (PGRAPH g, PLINK p_link);
void remove_linklist (PGRAPH g, PLINK p_link);

LINK *malloc_LinksOut (void);
void free_LinksOut (PLINK p);
void make_empty_Links (PGRAPH g, NODEID node);
void make_empty_linklist (PGRAPH g, NODEID node);
void make_empty_PATH_ITEM (PLINK plink);
PLINK add_link (PGRAPH g, NODEID from, NODEID to, CAPACITY capacity);
PLINK find_link (PGRAPH g, PLINK link, NODEID from, NODEID to);
void remove_link (PGRAPH g, NODEID node, PLINK p_link);
PITEM add_item (PGRAPH g, PLINK p, NODEID from, NODEID to, TRAFFIC flow);
void remove_item (PGRAPH g, PLINK p, PITEM p_item);
PITEM find_item (PGRAPH g, PLINK p, NODEID from, NODEID to);
void init_graph (PGRAPH g, int n);
PGRAPH build_initial_solution (PGRAPH g, double prob, int iter_start);
void clone_graph (PGRAPH g1, PGRAPH g2);
void make_empty_graph (PGRAPH g);

/* Prototipi per  print_structures.c */

void print_path_item (FILE * fp, PLINK p);
void print_list_link (int num_nodes, FILE * fp, PLINK p);
void print_graph (PGRAPH g, FILE * fp);
void print_test_graph (PGRAPH g);
void read_graph (FILE * fl, PGRAPH g);


/* Prototipi per matrix.c */

MATRIX alloc_matrix (MATRIX matrix, int n);
DISCRETE_CAP_MATRIX alloc_discrete_cap_matrix (DISCRETE_CAP_MATRIX
					       dis_cap_matrix, int n,
					       int num_cap);
int read_input_matrix (int num_nodes, FILE * fp_trf, FILE * fp_weight_route);
void print_matrix (MATRIX matrix, int n, FILE * fp_result);
void print_discrete_cap_matrix (DISCRETE_CAP_MATRIX dis_cap_matrix,
				FILE * fp_result, int num_cap, int n);
int build_random_traffic_matrix0(int num_nodes, double lb, double ub);

/* Prototipi per routing.c        */

void alloc_PR_QUEUE (int n);
int init_PR_QUEUE (PGRAPH g, NODEID source, ITEM_HEAP * pr_queue);
void build_heap (ITEM_HEAP * pr_queue, int heap_size);
void heapify (ITEM_HEAP * pr_queue, int start_node, int heap_size);
void swap (ITEM_HEAP * pr_queue, int a, int b);
int heap_extract_min (ITEM_HEAP * pr_queue, int heapsize);
void dijkstra_heap (PGRAPH g, NODEID source, ITEM_HEAP * pr_queue);
void reset_fields_sh_path (PGRAPH g);
void reset_all_sh_path (PGRAPH g, BOOL bool);
void reset_sh_path_from_to (PGRAPH g, NODEID from, NODEID to, PLINK ** mp);
void insert_conn_on_link (PGRAPH g, NODEID source, BOOL bool);
void update_cost (PGRAPH g, ITEM_HEAP * pr_queue, int heapsize);
BOOL insert_sh_path_in_matrix (PGRAPH g, NODEID source, PLINK ** mp);
COST get_cost_sh_path_from_to (FILE * fp, PGRAPH g, PLINK ** mp, NODEID to);
void print_sh_path_from_to (FILE * fp, PGRAPH g, PLINK ** mp, NODEID to);
void print_all_sh_path_from (FILE * fp, PGRAPH g, PLINK ** mp, NODEID from);
void find_all_sh_path (FILE * fp, PGRAPH g, ITEM_HEAP * pr_queue);
void reset_flow (PGRAPH g);
void reset_broken_flow (PGRAPH g);
void reset_max_flow_field (PGRAPH g);
void update_flow (PGRAPH g, BOOL flag_mp, MATRIX trf_m, BOOL fault);
BOOL total_route_flow (PGRAPH g, ITEM_HEAP * pr_queue, BOOL fault,
		       NODEID node, BOOL option_move);


void routing_incrementale (FILE * fp, PGRAPH g, ITEM_HEAP * pr_queue);



/* Prototipi per fault.c  */

void disable_node (PGRAPH g, NODEID node);
void enable_node (PGRAPH g, NODEID node);
void enable_in_out_link (PGRAPH g, NODEID node);
void disable_in_out_link (PGRAPH g, NODEID node);
void broken_node (PGRAPH g, NODEID node);
void broken_link (PGRAPH g, PLINK link);

int main_graph (int argc, char *argv[]);

#endif

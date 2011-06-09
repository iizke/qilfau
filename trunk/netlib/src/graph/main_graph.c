#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAIN_C

#include "main_graph.h"
#include "topo.h"

/*  main() */
int main_graph(int argc, char *argv[]) {
  FILE *fp_trf = NULL; /* puntatore al file contenente la matrice di traffico nodo-nodo [Pck/s]       */
  FILE *fp_result = NULL; /* puntatore al file di output                                                                                                                          */
  FILE *fp_weight_route = NULL; /* puntatore al file di lettura dei pesi associati ad ogni linf (from -> to) */
  /* se il puntatore al file � NULL i pesi vengono posti ad 1.0 di default        */
  int i;

  double prob = 0.6;

  char *s;
  char in_dist[200] = "";
  char in_trf[200] = "";//"src/graph/trf_Mbps.dat";
  char in_weight_route[200] = ""; /* file contenente i pesi di ogni link utilizzati per il routing        */
  char out_result[200] = "or.log";
  clock_t start_time;

  int delta = 1;
  int num_nodes = 8;

  seed = 213423;

  /* Parsing of the Command line options. */

  while (--argc > 0) {
    if ((*++argv)[0] == '-') {
      s = argv[0] + 1;
      switch (*s) {
      case 'd':
        if (strncmp(s, "d=", 2) == 0) {
          s += 2;
          sscanf(s, "%d", &delta);
          if (delta <= 0)
            error("Warning:  Non positive num_nodes\n", stderr);
        } else
          error("Warning: Invalid option ignored \n", stderr);
        break;

      case 'n':
        if (strncmp(s, "n=", 2) == 0) {
          s += 2;
          sscanf(s, "%d", &num_nodes);
          if (num_nodes <= 0)
            error("Warning:  Non positive num_nodes\n", stderr);
        } else
          error("Warning: Invalid option -num_multi_start ignored \n", stderr);
        break;

      case 'i':
        if (strncmp(s, "in_dist=", 8) == 0) {
          s += 8;
          strcpy(in_dist, s);
        } else if (strncmp(s, "in_trf=", 7) == 0) {
          s += 7;
          strcpy(in_trf, s);
        } else if (strncmp(s, "in_weight_route=", 16) == 0) {
          s += 16;
          strcpy(in_weight_route, s);
        } else
          printf("Warning: Invalid option -%s ignored\n", s);
        break;

      case 'o':
        if (strncmp(s, "out_result=", 11) == 0) {
          s += 11;
          strcpy(out_result, s);
        } else
          printf("Warning: Invalid option -%s ignored\n", s);
        break;

      case 's':
        if (strncmp(s, "seed=", 5) == 0) {
          s += 5;
          sscanf(s, "%ld", &seed);
          if (seed <= 0)
            error("Non positive seed\n", stderr);
        } else
          printf("Warning: Invalid option -%s ignored\n", s);
        break;

      case 'p':
        if (strncmp(s, "prob=", 5) == 0) {
          s += 5;
          sscanf(s, "%lf", &prob);
          if (prob < 0 || prob > 1.0)
            error("No admissible prob parameter\n", stderr);
        } else
          printf("Warning: Invalid option -%s ignored\n", s);
        break;

      default:
        printf("Warning: Invalid option -%s ignored\n", s);
      }
    }
  }

  if (strlen(out_result) != 0) {
    fp_result = fopen(out_result, "a");
    if (fp_result == NULL) {
      error("Impossibile aprire file di output dei risultati", stderr);
      exit(1);
    } else {
      printf("\nWriting the results in %s\n", out_result);
    }
  } else {
    fp_result = stdout;
    //fprintf(fp_result, "Writing output to stdout\n");
    printf("Writing output to stdout\n");
  }

  if (strlen(in_trf) != 0) {
    fp_trf = fopen(in_trf, "r");
    if (fp_trf == NULL) {
      error(
          "Impossibile aprire file di input contenente la matrice di traffico",
          stderr);
      exit(1);
    } else {
      //fprintf(fp_result, "Reading the traffic matrix from %s\n", in_trf);
      printf("Reading the traffic matrix from %s\n", in_trf);
    }
  } else
    //fprintf(fp_result, "Traffico scambiato uniforme.\n");
    printf("Traffico scambiato uniforme.\n");

  if (strlen(in_weight_route) != 0) {
    fp_weight_route = fopen(in_weight_route, "r");
    if (fp_weight_route == NULL) {
      error("Impossibile aprire file di input contenente i pesi del routing",
          stderr);
    } else {
      //fprintf(fp_result, "Reading the routing weights from %s\n", in_weight_route);
      printf("Reading the routing weights from %s\n", in_weight_route);
    }
  } else
    //fprintf(fp_result, "Pesi del routing fissati da default a 1.0\n");
    printf("Pesi del routing fissati da default a 1.0\n");

  if (read_input_matrix(num_nodes, fp_trf, fp_weight_route) == FALSE)
    exit(1);

//  print_matrix(trf_m, num_nodes, fp_result);
//  print_matrix(weight_route, num_nodes, fp_result);

  init_graph(&g, num_nodes);
  alloc_PR_QUEUE(max_num_nodes_graph (&g));
  start_time = clock();
  //build_initial_solution(&g, prob, 1);
  build_ring_topology2(&g, 1);
  //build_ring_topology1(&g);

  for (i = 0; i < num_nodes; i++) {
    dijkstra_heap(&g, i, pr_queue);
    if (insert_sh_path_in_matrix(&g, i, g.node_set[i].matrix_path)
        == DISCONNECTED)
      printf("Disconnected topology\n");
    //print_all_sh_path_from(fp_result, &g, g.node_set[i].matrix_path, i);
  }
  update_flow(&g, MP, trf_m, 0);
  start_time = clock() - start_time;
  //print_graph(&g, fp_result);
  fprintf(fp_result, "fmax = %f, delta = %d, num_nodes = %d\n", print_maxflow (&g), delta, g.num_nodes);
  //fprintf(fp_result, "\nElapsed_time = %f\n", (double) start_time / CLOCKS_PER_SEC);

  fclose(fp_result);
  //sleep(1000);
  return (1);
}

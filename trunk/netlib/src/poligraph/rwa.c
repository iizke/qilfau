/* funzioni per RWA */
#include "main_rwa.h"
#include "polirand/random.h"

void alloc_requests(int ***L, int num_req) {
  int i;

  (*L) = (int **) malloc(num_req * sizeof(int**));

  for (i = 0; i < num_req; i++) {
    (*L)[i] = (int *) malloc(2 * sizeof(int*));
  }

}

void create_requests(int **L, int num_req, int num_nodes) {
  int i;

  for (i = 0; i < num_req; i++) {
    /* scelgo la sorgente a caso tra N nodi */
    L[i][0] = polirand_randint(0, num_nodes - 1, &seed);
    /* scelgo la destinazione a caso tra N meno s */
    do {
      L[i][1] = polirand_randint(0, num_nodes - 1, &seed);
    } while (L[i][0] == L[i][1]);
  }

}

void print_requests(int **L, int num_req) {
  int i;
  printf("Elenco richieste\n");
  for (i = 0; i < num_req; i++) {
    printf("req[%d]: %d -> %d\n", i, L[i][0], L[i][1]);
  }
}

void alloc_lambda(int ***W, int num_req) {
  int arc, w;

  (*W) = (int **) malloc(MAX_NUM_LINKS * sizeof(int**));

  for (arc = 0; arc < MAX_NUM_LINKS; arc++) {
    (*W)[arc] = (int *) malloc(num_req * sizeof(int*));
  }

  /* inizializza tutte le lambda a libero */
  for (arc = 0; arc < MAX_NUM_LINKS; arc++) {
    for (w = 0; w < num_req; w++)
      (*W)[arc][w] = LIBERO;
  }
}

void print_lambda(int **W, int num_req) {
  int arc, w;

  printf("Matrice stato lunghezze d'onda\n");
  for (arc = 0; arc < MAX_NUM_LINKS; arc++) {
    printf("Arco %3d: ", arc);
    for (w = 0; w < num_req; w++)
      printf("%2d ", W[arc][w]);
    printf("\n");
  }
}

void RWA_ssff(PGRAPH g, int **L, int **lambda, int num_req) {
  int i, j;
  int s, d, arco;
  PLINK ** mp;
  int w;
  BOOL ok;

  /* per ogni richiesta */
  for (i = 0; i < num_req; i++) {
    /* selezione cammino minimo tra s e d di richiesta i */
    s = L[i][0];
    d = L[i][1];
    /* prendi i percorsi da s */
    mp = g->node_set[s].matrix_path;

    w = -1;
    ok = TRUE;
    /* per ogni arco */
    do {
      w++;
      ok = TRUE;
      for (j = 0; mp[d][j] != NULL; j++) {
        arco = mp[d][j]->identif;
        if (lambda[arco][w] != LIBERO) {
          ok = FALSE;
          break;
        }
      }
    } while (ok == FALSE);

    /* qui w e la prima lambda libera su tutti gli archi*/
    for (j = 0; mp[d][j] != NULL; j++) {
      arco = mp[d][j]->identif;
      /* la i-esima richiesta usa questa lambda */
      lambda[arco][w] = i;
    }
  } /*fine per tutte le richieste */
}

void max_lambda(int **lambda, int num_req) {
  BOOL flag;
  int counter = 0, i, j;
  for (i = 0; i < num_req; i++) {
    flag = FALSE;
    for (j = 0; j < MAX_NUM_LINKS; j++) {
      if (lambda[j][i] != LIBERO) {
        flag = TRUE;
        break;
      }
    }
    if (flag == TRUE)
      counter++;
    else
      break;
  }
  printf("Numero di lunghezze d'onda massimo: %d\n", counter);
}


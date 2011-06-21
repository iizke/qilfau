#include "main_graph.h"
#include "polirand/random.h"
#include "graph.h"

/*---------------------------------------------------------------------------*/
/*            Graphs                                                         */
/*---------------------------------------------------------------------------*/

/* Gestione delle liste utilizzate nella struttura Grafo */
/* Att. utilizzare la funzione perror() al posto di fprintf() */

PITEM malloc_PATH_ITEM(void) /* Crea nuovo elemento/Prende elemento
 dalla lista PATH_ITEM_free */
{
  PITEM p;

  if (p_PATH_ITEM_free == NULL) {
    if ((p = (PITEM) malloc(sizeof(PATH_ITEM))) == NULL) {
      fprintf(stderr, "malloc_ITEM_LIST: Errore allocazione memoria\n");
      exit(1);
    }
  } else {
    p = p_PATH_ITEM_free;
    p_PATH_ITEM_free = next_item (p_PATH_ITEM_free);
  }
  *p = NullItem;
  return (p);
}

void free_PATH_ITEM(PITEM p) /* Inserisce nella lista
 PATH_ITEM_free l'elemento puntato da p */
{
  *p = NullItem;
  next_item (p) = p_PATH_ITEM_free;
  if (p_PATH_ITEM_free != NULL)
    pred_item (next_item (p)) = p;
  p_PATH_ITEM_free = p;
}

/* Libera la lista PATH_ITEM */

void make_empty_PATH_ITEM(PLINK plink) {
  PITEM p;

  p = first_path_item (plink);
  while (first_path_item (plink) != NULL) {
    p = first_path_item (plink);
    first_path_item (plink) = next_item (first_path_item (plink));
    free_PATH_ITEM(p);
  }
}

PLINKLIST malloc_linklist(void) /* Crea nuovo elemento/Prende elemento
 dalla lista p_LINKLIST_free */
{
  PLINKLIST p;

  if (p_LINKLIST_free == NULL) {
    if ((p = (PLINKLIST) malloc(sizeof(LINKLIST))) == NULL) {
      fprintf(stderr, "malloc_linklist: Errore allocazione memoria\n");
      exit(1);
    }
  } else {
    p = p_LINKLIST_free;
    p_LINKLIST_free = linklist_next (p_LINKLIST_free);
  }
  *p = NullLinkList;
  return (p);
}

void free_linklist(PLINKLIST p) /* Inserisce l'elemento p nella lista free */
{
  *p = NullLinkList;
  linklist_next (p) = p_LINKLIST_free;
  if (p_LINKLIST_free != NULL)
    linklist_pred (linklist_next (p)) = p;
  p_LINKLIST_free = p;
}

/* Libera la lista Links */

void make_empty_linklist(PGRAPH g, NODEID i) {
  PLINKLIST p;

  while (get_linklist (g, i) != NULL) {
    p = get_linklist (g, i);
    get_linklist (g, i) = linklist_next (get_linklist (g, i));
    free_linklist(p);
  }
}

PLINK malloc_LinksOut(void) /* Crea nuovo elemento/Prende elemento
 dalla lista p_LINK_free */
{
  PLINK p;

  if (p_LINK_free == NULL) {
    if ((p = (PLINK) malloc(sizeof(LINK))) == NULL) {
      fprintf(stderr, "malloc_LinksOut: Errore allocazione memoria\n");
      exit(1);
    }
  } else {
    p = p_LINK_free;
    p_LINK_free = next_link (p_LINK_free);
  }
  *p = NullLink;
  return (p);
}

void free_LinksOut(PLINK p) /* Inserisce l'elemento p nella lista free */
{
  *p = NullLink;
  next_link (p) = p_LINK_free;
  if (p_LINK_free != NULL)
    pred_link (next_link (p)) = p;
  p_LINK_free = p;
}

/* Libera la lista Links */

void make_empty_Links(PGRAPH g, NODEID i) {
  PLINK p;
  while (first_link_out (g, i) != NULL) {
    p = first_link_out (g, i);
    first_link_out (g, i) = next_link (first_link_out (g, i));
    free_LinksOut(p);
  }
}

/* Operazioni comuni sulla struttura Grafo */

/* Aggiunge un nuovo link from->to al grafo e ritorna un puntatore al nuovo 
 link. Aggiunge il link sia in ingresso al nodo 'to' sia in uscita dal nodo
 from. */

PLINK add_link(PGRAPH g, NODEID from, NODEID to, CAPACITY capacity) {
  PLINK p_link_out;
  PLINKLIST p_linklist_in;

  p_link_out = malloc_LinksOut();
  link_from (p_link_out) = from;
  link_to (p_link_out) = to;
  route_cost_link (p_link_out) = (entry (weight_route, from, to));
  link_capacity (p_link_out) = capacity;
  link_broken_capacity (p_link_out) = capacity;
  link_flow (p_link_out) = 0.0;
  link_broken_flow (p_link_out) = 0.0;
  link_broken_max_flow (p_link_out) = 0.0;
  link_state (p_link_out) = TRUE;

  if (first_link_out (g, from) == NULL) {
    next_link (p_link_out) = NULL;
    first_link_out (g, from) = p_link_out;
  } else {
    next_link (p_link_out) = first_link_out (g, from);
    pred_link (first_link_out (g, from)) = p_link_out;
    first_link_out (g, from) = p_link_out;
  }
  /* inserisco il puntatore al link nella linklist del nodo destinazione */
  p_linklist_in = malloc_linklist();
  get_link_in (p_linklist_in) = p_link_out;

  if (get_linklist (g, to) == NULL) {
    linklist_next (p_linklist_in) = NULL;
    get_linklist (g, to) = p_linklist_in;
  } else {
    linklist_next (p_linklist_in) = get_linklist (g, to);
    linklist_pred (get_linklist (g, to)) = p_linklist_in;
    get_linklist (g, to) = p_linklist_in;
  }
  return (p_link_out);
}

PLINKLIST find_linklist(PGRAPH g, PLINK p_link) {
  PLINKLIST plinklist;

  plinklist = get_linklist (g, link_to (p_link));
  while (get_link_in (plinklist) != p_link && get_link_in (plinklist) != NULL)
    plinklist = linklist_next (plinklist);
  if (plinklist == NULL) {
    perror("remove_link: link non trovato sulla lista dei link entranti!!\n");
    exit(1);
  }
  return (plinklist);
}

/* rimuove il puntatore a link dalla linklist dei link entranti */
void remove_linklist(PGRAPH g, PLINK p_link) {
  PLINKLIST plinklist = find_linklist(g, p_link);

  if (plinklist != get_linklist (g, link_to (p_link))) /* elemento non di testa */
  {
    linklist_next (linklist_pred (plinklist)) = linklist_next (plinklist);
    if (linklist_next (plinklist) != NULL) /* Non � l'ultimo */
      linklist_pred (linklist_next (plinklist)) = linklist_pred (plinklist);
  } else
    /*e' il primo */
    get_linklist (g, link_to (p_link)) = linklist_next (plinklist);

  free_linklist(plinklist);
}

/* Rimuove il link from->to dal grafo e ritorna un puntatore al nuovo link */
void remove_link(PGRAPH g, NODEID node, PLINK p_link) {
  remove_linklist(g, p_link);

  /* rimuovo il link dalla lista dei link uscenti */

  if (first_link_out (g, node) != p_link) { /* elemento non di testa */
    next_link (pred_link (p_link)) = next_link (p_link);
    if (next_link (p_link) != NULL) /* Non � l'ultimo */
      pred_link (next_link (p_link)) = pred_link (p_link);
  } else
    first_link_out (g, node) = next_link (p_link);

  free_LinksOut(p_link);
}

PLINK find_link(PGRAPH g, PLINK link, NODEID from, NODEID to) {
  PLINK p = link;
  while (p != NULL) {
    if ((link_to (p) == to) && (link_from (p) == from))
      return (p);
    p = next_link (p);
  }
  return (NULL);
}

PITEM add_item(PGRAPH g, PLINK p, NODEID from, NODEID to, TRAFFIC flow) {
  PITEM p_item;
  p_item = malloc_PATH_ITEM();
  item_from (p_item) = from;
  item_to (p_item) = to;
  flow_item (p_item) = flow;
  if (first_path_item (p) == NULL) {
    next_item (p_item) = NULL;
    first_path_item (p) = p_item;
  } else {
    next_item (p_item) = first_path_item (p);
    pred_item (first_path_item (p)) = p_item;
    first_path_item (p) = p_item;
  }
  return (p_item);
}

void remove_item(PGRAPH g, PLINK p, PITEM p_item) {
  if (first_path_item (p) != p_item) { /* non � un puntatore di testa */
    next_item (pred_item (p_item)) = next_item (p_item);
    if (next_item (p_item) != NULL) /* Non � l'ultimo */
      pred_item (next_item (p_item)) = pred_item (p_item);
  } else
    first_path_item (p) = next_item (p_item);
  free_PATH_ITEM(p_item);
}

PITEM find_item(PGRAPH g, PLINK p, NODEID from, NODEID to) {
  PITEM p_item = first_path_item (p);
  while (p_item != NULL) {
    if ((item_from (p_item) == from) && (item_to (p_item) == to))
      return (p_item);
    p_item = next_item (p_item);
  }
  return (NULL);
}

/* Costruisce il grafo iniziale utilizzato per il TabuSearch */
PGRAPH build_initial_solution(PGRAPH g, double prob, int iter_start) {
  PLINK p;
  int i, j, num_nodes = num_nodes_graph (g);
  double value;

  if (iter_start == 0) {
    int ident = 0;
    for (i = 0; i < num_nodes; i++) {
      for (j = 0; j < num_nodes; j++) {
        if ((i != j) && (find_link(g, first_link_out (g, i), i, j) == NULL)) {
          p = add_link(g, i, j, 0.0);
          p->identif = ident;
          ident++;
        }
      }
    }
    printf("Generated a Fully Connected Initial Solution \n");
  } else if (iter_start == 1) {
    int ident = 0;
    for (i = 0; i < num_nodes; i++) {
      for (j = 0; j < num_nodes; j++) {
        if ((i != j) && (find_link(g, first_link_out (g, i), i, j) == NULL)) {
          value = polirand_uniform(0, 1, &seed);
          if (value < prob) {/* Con probabilità prob inserisco il link */
            p = add_link(g, i, j, 0.0);
            p->identif = ident;
            ident++;
          }
        }
      }
    }
    printf("Generated a Random Initial Solution with p(i,j) = %f\n", prob);
  } else if (iter_start == 2) {
    int ident = 0;
    for (i = 0; i < num_nodes; i++) {
      j = ((i + 1) % num_nodes);
      if ((find_link(g, first_link_out (g, i), i, j) == NULL))
        p = add_link(g, i, j, 0.0);
      p->identif = ident;
      ident++;
    }
    printf("Generated a RING topology\n");
  }

  return (g);
}



void init_graph(PGRAPH g1, int num_nodes) {
  int i, j, k, max_num_nodes = MAX_NUM_NODES;

  /* Allocazione e Inizializzazione strutture dinamiche: tutte 
   le strutture dinamiche sono di dimensione
   max_num_nodes/max_num_links */

#ifdef MEM_DEBUG
  // printf ("\n ALLOCAZIONE CON MALLOC E INIZIALIZZAZIONE STRUTTURE in init_graph()\n");
#endif

  if ((g1->node_set = (PNODE) malloc(max_num_nodes * sizeof(NODE))) == NULL) {
    printf("init_graph: Errore allocazione memoria\n");
  }

  for (i = 0; i < max_num_nodes; i++) {
    node_id (g1, i) = i;
    first_link_out (g1, i) = NULL;
    get_linklist (g1, i) = NULL;
    num_links_out (g1, i) = 0;
    node_state (g1, i) = TRUE; /* Abilitato */
    node_path_weight (g1, i) = 0.0;
    ingoing_link_to_node (g1, i) = NULL;

    /* Allocazione matrice routing non incrementale */

    if ((node_graph (g1, i).matrix_path = (PLINK **) malloc((max_num_nodes)
        * sizeof(PLINK *))) == NULL) /* 1 indice: destinazione */
      printf("init_graph: Errore allocazione memoria\n");
    for (j = 0; j < (max_num_nodes); j++) {
      if ((node_graph (g1, i).matrix_path[j] = (PLINK *) malloc((max_num_nodes)
          * sizeof(PLINK))) == NULL) /* 1 indice: destinazione */
        printf("init_graph: Errore allocazione memoria\n");
      for (k = 0; k < max_num_nodes; k++)
        node_graph (g1, i).matrix_path[j][k] = NULL; /* inizializzo la matrice a NULL */
    }

    /* Allocazione matrice routing incrementale */
    if ((node_graph (g1, i).incr_matrix_path = (PLINK **) malloc(
        (max_num_nodes) * sizeof(PLINK *))) == NULL) /* 1 indice: destinazione */
      printf("init_graph: Errore allocazione memoria\n");
    for (j = 0; j < (max_num_nodes); j++) {
      if ((node_graph (g1, i).incr_matrix_path[j] = (PLINK *) malloc(
          (max_num_nodes) * sizeof(PLINK))) == NULL) /* 1 indice: destinazione */
        printf("init_graph: Errore allocazione memoria\n");
      for (k = 0; k < max_num_nodes; k++)
        node_graph (g1, i).incr_matrix_path[j][k] = NULL; /* inizializzo la matrice a NULL */
    }
  }

  num_nodes_graph (g1) = num_nodes;
  num_links_graph (g1) = 0;
  max_num_nodes_graph (g1) = max_num_nodes;
  max_num_links_graph (g1) = 0;
  cost_graph (g1) = 0.0;
}

/* Copia il grafo g1 in g2. Non copio la lista delle connessioni che si
 appoggiano sui vari link perch� non mi servir� */
void clone_graph(PGRAPH g1, PGRAPH g2) {
  int s, d, k, num_nodes = num_nodes_graph (g1);
  PLINK link, link1;
  // PATH_ITEM p_item;
  make_empty_graph(g2);
  num_nodes_graph (g2) = num_nodes_graph (g1);
  num_links_graph (g2) = num_links_graph (g1);
  max_num_nodes_graph (g2) = max_num_nodes_graph (g1);
  max_num_links_graph (g2) = max_num_links_graph (g1);
  cost_graph (g2) = cost_graph (g1);
  cost_broken_graph (g2) = cost_broken_graph (g1);
  for (s = 0; s < num_nodes; s++) {
    node_id (g2, s) = node_id (g1, s);
    node_state (g2, s) = node_state (g1, s);
    num_links_out (g2, s) = num_links_out (g1, s);
    node_path_weight (g2, s) = node_path_weight (g1, s);
    ingoing_link_to_node (g2, s) = ingoing_link_to_node (g1, s);
    for (d = 0; d < num_nodes; d++)
      for (k = 0; k < num_nodes; k++)
        set_path_from_to (g2, s, MP, d, k, path_from_to (g1, s, MP, d, k));
    for (d = 0; d < num_nodes; d++)
      for (k = 0; k < num_nodes; k++)
        set_path_from_to (g2, s, IMP, d, k, path_from_to (g1, s, IMP, d, k));
    link = first_link_out (g1, s);
    while (link != NULL) {
      link1 = add_link(g2, link_from (link), link_to (link),
          link_capacity (link));
      link_broken_capacity (link1) = link_broken_capacity (link);
      // economic_cost_link(link1)=economic_cost_link(link);
      visited_link (link1) = visited_link (link);
      link_state (link1) = link_state (link);
      link_flow (link1) = link_flow (link);
      link_broken_flow (link1) = link_broken_flow (link);
      link_broken_max_flow (link1) = link_broken_max_flow (link);
      link = next_link (link);
    }
  }
}

/* Funzione che libera un grafo, cancellando tutti i campi e 
 liberando le liste */

void make_empty_graph(PGRAPH g) {
  int i, j, k, num_nodes = num_nodes_graph (g);
  PLINK plink;
  // num_nodes_graph(g)=num_nodes;
  num_links_graph (g) = 0;
  // max_num_nodes_graph(g)=max_num_nodes;
  max_num_links_graph (g) = 0;
  cost_graph (g) = 0.0;
  for (i = 0; i < num_nodes; i++) {
    node_id (g, i) = i;
    node_state (g, i) = TRUE;
    node_path_weight (g, i) = 0.0;
    num_links_out (g, i) = 0;
    plink = first_link_out (g, i);
    while (plink != NULL) {
      make_empty_PATH_ITEM(plink);
      plink = next_link (plink);
    }
    ingoing_link_to_node (g, i) = NULL;
    for (j = 0; j < num_nodes; j++)
      for (k = 0; k < num_nodes; k++) {
        set_path_from_to (g, i, MP, j, k, NULL);
        set_path_from_to (g, i, IMP, j, k, NULL);
      }

    make_empty_Links(g, i);
    make_empty_linklist(g, i);
  }
}

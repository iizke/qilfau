#include "main_graph.h"

					/* alloc_PR_QUEUE: alloca la struttura heap     */

void
alloc_PR_QUEUE (int n)
{

  if ((pr_queue = (ITEM_HEAP *) malloc (n * sizeof (ITEM_HEAP))) == NULL)
    printf ("alloc_PR_QUEUE: Errore allocazione memoria\n");
}


	/* init_PR_QUEUE: nella coda prioritaria vengono inseriti solo i nodi abilitati */
	/* La struttura  PR_QUEE viene inizializzata    come heap binario       */


int
init_PR_QUEUE (PGRAPH g, NODEID source, ITEM_HEAP * pr_queue)
{
  int i, j = 1, num_nodes = num_nodes_graph (g), heap_size = 0;

  pr_queue[0].node_id = source;
  pr_queue[0].path_cost = 0.0;
  route_cost_node (g, source) = 0.0;
  heap_size++;


  for (i = 0; i < num_nodes; i++)
    {
      if (node_state (g, i) == TRUE && i != source)
	{
	  pr_queue[j].node_id = i;
	  pr_queue[j].path_cost = MAX_INT;
	  heap_size++;
	  j++;
	}
    }
  return (heap_size);
}



void
build_heap (ITEM_HEAP * pr_queue, int heap_size)
{
  int i;

  for (i = (heap_size / 2); i >= 0; i--)
    heapify (pr_queue, i, heap_size);
}


		/*      heapify: viene chiamata ricorsivamente da build-heap    */

void
heapify (ITEM_HEAP * pr_queue, int start_node, int heap_size)
{
  int left, right, min, num_nodes = heap_size;

  left = (start_node * 2) + 1;	/* figlio sx del padre start_node nella strut. heap     */
  right = (start_node * 2) + 2;	/* figlio dx del padre start_node nella strut. heap     */

  if ((left < num_nodes)
      && (pr_queue[left].path_cost < pr_queue[start_node].path_cost))
    min = left;
  else
    min = start_node;

  if ((right < num_nodes)
      && (pr_queue[right].path_cost < pr_queue[min].path_cost))
    min = right;
  if (min != start_node)
    {
      swap (pr_queue, start_node, min);
      heapify (pr_queue, min, heap_size);
    }
}

					/* swap: funzione che scambi due elementi       */

void
swap (ITEM_HEAP * pr_queue, int a, int b)
{
  ITEM_HEAP tmp;

  tmp.node_id = pr_queue[a].node_id;
  tmp.path_cost = pr_queue[a].path_cost;

  pr_queue[a].node_id = pr_queue[b].node_id;
  pr_queue[a].path_cost = pr_queue[b].path_cost;

  pr_queue[b].node_id = tmp.node_id;
  pr_queue[b].path_cost = tmp.path_cost;
}


		/* heap_extract_mean: funzione che i) trasforma il vettore in un nuovo heap (ha perso
		   le propriet� dell'heap a cuasa degli aggiornamenti parziali dei costi
		   durante la costruzione dell'albero dei cammini minimi e ii) ritorna il
		   primo elemento della struttura (elemento a minima distanza dalla
		   sorgente     */

int
heap_extract_min (ITEM_HEAP * pr_queue, int heap_size)
{
  NODEID node_min_cost = pr_queue[0].node_id;

  build_heap (pr_queue, heap_size);
  node_min_cost = pr_queue[0].node_id;
  pr_queue[0].node_id = pr_queue[heap_size - 1].node_id;
  pr_queue[0].path_cost = pr_queue[heap_size - 1].path_cost;
  return (node_min_cost);
}



/* dijkstra_heap() fa lo sh_path della topologia definita in grafo, dalla
   sorgente 'source' verso tutte le destinazioni. Utilizza una coda prioritaria
	organizzata come un heap binario (pr_queue) */


void
dijkstra_heap (PGRAPH g, NODEID source, ITEM_HEAP * pr_queue)
{
  int i, heap_size;
  PLINK link;

  NODEID nearest_node_to_source;	/* nodo pi� vicino alla source  */

  reset_fields_sh_path (g);
  if (node_state (g, source) == TRUE)
    {				/* Nodo abilitato       */
      heap_size = init_PR_QUEUE (g, source, pr_queue);


      for (i = 0; i < heap_size; i++)
	{
	  nearest_node_to_source = heap_extract_min (pr_queue, heap_size - i);
	  link = first_link_out (g, nearest_node_to_source);
	  while (link != NULL)
	    {
	      if ((link_state (link) == TRUE)
		  && (route_cost_node (g, link_to (link)) >
		      (route_cost_node (g, link_from (link)) +
		       route_cost_link (link))))
		{

		  route_cost_node (g, link_to (link)) =
		    route_cost_node (g,
				     link_from (link)) +
		    route_cost_link (link);
		  ingoing_link_to_node (g, link_to (link)) = link;
		}
	      link = next_link (link);
	    }
	  update_cost (g, pr_queue, heap_size - i - 1);
	}
    }
}


/* reset_fields_sh_path: resetta i campi interessati nella costruzione 
	dello sh_path (costo del routing di ogni nodo ed il puntatore del link 
	con cui si arriva al nodo)	*/

void
reset_fields_sh_path (PGRAPH g)
{
  int i, num_nodes = num_nodes_graph (g);

  for (i = 0; i < num_nodes; i++)
    {
      route_cost_node (g, i) = MAX_INT;
      ingoing_link_to_node (g, i) = NULL;
    }
}


/* reset_all_sh_path: resetta a NULL i puntatori delle matrici matrix_path ed 
	incr_matrix_path dove erano conservate le informazioni su tutti i cammini 
	minimi trovati per la topologia analizzata e rotta (un guasto singolo). 
	Viene chiamato quando tutti gli shortest path da una sorgente sono stati 
	calcolati e si analizza una seconda sorgente	*/

void
reset_all_sh_path (PGRAPH g, BOOL bool)
{
  int from, to, i, num_nodes = num_nodes_graph (g);

  for (from = 0; from < num_nodes; from++)
    {
      for (to = 0; to < num_nodes; to++)
	{
	  if (bool == MP)
	    {
	      for (i = 0; (path_from_to (g, from, MP, to, i) != NULL); i++)
		set_path_from_to (g, from, MP, to, i, NULL);
	    }
	  else
	    {
	      for (i = 0; (path_from_to (g, from, IMP, to, i) != NULL); i++)
		set_path_from_to (g, from, IMP, to, i, NULL);
	    }
	}
    }
}


/* reset_sh_path_from_to: cancella l'albero dei cammini minimi da 'from' a 'to'
	 dalla matrice passata come parametro di ingresso	*/

void
reset_sh_path_from_to (PGRAPH g, NODEID from, NODEID to, PLINK ** mp)
{
  int i;

  for (i = 0; mp[to][i] != NULL; i++)
    mp[to][i] = NULL;
}



/* insert_conn_link: inserisce tutte le connessioni che hanno come sorgente 
	'source' (lette da matrix_path/incr_matrix_path)	*/

void
insert_conn_on_link (PGRAPH g, NODEID source, BOOL bool)
{
  int to, k, num_nodes = num_nodes_graph (g);

  if (bool == MP)
    {
      for (to = 0; to < num_nodes; to++)
	{			/* destinazioni */
	  if (to != source)
	    {
	      for (k = 0; ((path_from_to (g, source, MP, to, k)) != NULL); k++)	/* el. matrix */
		add_item (g, path_from_to (g, source, MP, to, k), source, to,
			  0.0);
	    }
	}
    }
  else
    {
      for (to = 0; to < num_nodes; to++)
	{			/* destinazioni */
	  if (to != source)
	    {
	      for (k = 0; ((path_from_to (g, source, IMP, to, k)) != NULL);
		   k++)		/* el. matrix */
		add_item (g, path_from_to (g, source, IMP, to, k), source, to,
			  0.0);
	    }
	}
    }
}


/* update_cost: aggiorna i costi associati al routing durante la costruzione 
	dell'albero dei cammini minimi	*/

void
update_cost (PGRAPH g, ITEM_HEAP * pr_queue, int heap_size)
{
  int i;
  for (i = 0; i < heap_size; i++)
    pr_queue[i].path_cost = route_cost_node (g, pr_queue[i].node_id);
}


/* insert_sh_path_in_matrix: inserisce l'albero dei cammini minimi che ha
	come 'root' il nodo 'source' nella matrice dei cammini minimi passata come 
	parametro (**mp)	*/

BOOL insert_sh_path_in_matrix (PGRAPH g, NODEID source, PLINK ** mp)
{
  int i, j, num_nodes = num_nodes_graph (g);
  PLINK link_p;

  for (i = 0; i < num_nodes; i++)
    {
      link_p = ingoing_link_to_node (g, i);
      j = 0;
      while (link_p != NULL && i != source)
	{
	  mp[i][j] = link_p;
	  j++;
	  link_p = ingoing_link_to_node (g, link_from (link_p));
	}
      if (i != source && j == 0 && node_state (g, i) == TRUE)	/* non c'e' un percorso */
	return DISCONNECTED;
    }
  return CONNECTED;
}


COST get_cost_sh_path_from_to (FILE * fp, PGRAPH g, PLINK ** mp, NODEID to)
{
  int i;
  COST cost = 0.0;

  for (i = 0; mp[to][i] != NULL; i++)
    cost += route_cost_link (mp[to][i]);
  return (cost);
}


	/* print_sh_path_from_to: prende/stampa lo sh_path da 'from' a 'to' presente
	   nella matrice passata in input **mp */


void
print_sh_path_from_to (FILE * fp, PGRAPH g, PLINK ** mp, NODEID to)
{
  int i;
  double tot_d = 0.0;

  for (i = 0; mp[to][i] != NULL; i++)
    tot_d +=
      1500.0 / ((link_capacity (mp[to][i]) - link_flow (mp[to][i]))) /
      1000000.0;
  fprintf (fp, "|%8.4f |\t", tot_d);

  for (i = 0; mp[to][i] != NULL; i++)
    fprintf (fp, "%d<-%d\t", link_to (mp[to][i]), link_from (mp[to][i]));
}

	/* print_all_sh_path_from: prende/stampa lo sh_path da 'from' a 'to' presente
	   nella matrice passata in input **mp */


void
print_all_sh_path_from (FILE * fp, PGRAPH g, PLINK ** mp, NODEID from)
{
  int j, num_nodes = num_nodes_graph (g);

  for (j = 0; j < num_nodes; j++)
    {				/* nodi destinazione    */
      if (from != j)
	{
	  fprintf (fp, "\n| %d -> %d\t|%6.2f\t", from, j,
		   get_cost_sh_path_from_to (fp, g, mp, j));
	  print_sh_path_from_to (fp, g, mp, j);
	}
    }
}


/*	find_all_sh_path: trova tutti gli shortest_path sulla topologia senza guasti,
	li memorizza nella matrice matrix_path di ogni nodo e li stampa	*/

void
find_all_sh_path (FILE * fp, PGRAPH g, ITEM_HEAP * pr_queue)
{
  int i, num_nodes = num_nodes_graph (g);

  for (i = 0; i < num_nodes; i++)
    {				/* sorgenti     */
      fprintf (fp, "\n");
      dijkstra_heap (g, i, pr_queue);
      insert_sh_path_in_matrix (g, i, g->node_set[i].matrix_path);
      insert_conn_on_link (g, i, MP);
      print_all_sh_path_from (fp, g, g->node_set[i].matrix_path, i);
    }
//reset_all_sh_path(g,MP);  
}


void
reset_flow (PGRAPH g)
{
  int i, num_nodes = num_nodes_graph (g);
  PLINK link;

  for (i = 0; i < num_nodes; i++)
    {
      link = first_link_out (g, i);
      while (link != NULL)
	{
	  link_flow (link) = 0.0;
	  link = next_link (link);
	}
    }
}

void
reset_broken_flow (PGRAPH g)
{
  int i, num_nodes = num_nodes_graph (g);
  PLINK link;

  for (i = 0; i < num_nodes; i++)
    {
      link = first_link_out (g, i);
      while (link != NULL)
	{
	  link_broken_flow (link) = 0.0;
	  link = next_link (link);
	}
    }
}


void
reset_max_flow_field (PGRAPH g)
{
  int i, num_nodes = num_nodes_graph (g);
  PLINK link;

  for (i = 0; i < num_nodes; i++)
    {
      link = first_link_out (g, i);
      while (link != NULL)
	{
	  link_broken_max_flow (link) = 0.0;
	  link = next_link (link);
	}
    }
}


/* Aggiorna i flussi sui link del grafo "g" in base agli shortest_path
	memorizzati nella matrice opportuna	*/

void
update_flow (PGRAPH g, BOOL flag_mp, _MATRIX trf_m, BOOL fault)
{

  int s, d, k, num_nodes = num_nodes_graph (g);
  PLINK link, prova;

  for (s = 0; s < num_nodes; s++)
    {
      for (d = 0; d < num_nodes; d++)
	{
	  if (s != d)
	    {
	      for (k = 0; path_from_to (g, s, flag_mp, d, k) != NULL; k++)
		{
		  prova = path_from_to (g, s, flag_mp, d, k);
		  if (fault == FALSE)	/* Senza guasti       */
		    link_flow (path_from_to (g, s, flag_mp, d, k)) +=
		      entry (trf_m, s, d);
		  else
		    link_broken_flow (path_from_to (g, s, flag_mp, d, k)) +=
		      entry (trf_m, s, d);
		}
	    }
	}
    }

  /*      Se sto considerando i guasti, devo memorizzare il massimo flusso su ogni link   */

  if (fault == TRUE)
    {
      for (s = 0; s < num_nodes; s++)
	{
	  link = first_link_out (g, s);
	  while (link != NULL)
	    {
	      if ((link_state (link) == TRUE)
		  && (link_broken_flow (link) > link_broken_max_flow (link)))
		link_broken_max_flow (link) = link_broken_flow (link);
	      link = next_link (link);
	    }
	}
    }
}

/* routing_totale:
   1)Guasti:	funzione che disabilita il nodo rotto con relativi link e 
	calcola tutti gli shortest path che hanno come sorgente nodi 
	S diversi dal nodo rotto. Tutti gli shortest_path da ogni nodo S
	vengono vengono morizzati nella matrice "matrix_path" del nodo S
   2)No Guasti:	calcola tutti gli shortest path da ogni sorgente S verso ogni
	destinazione. Gli shortest path da S trovati vengono memorizzati 
	nella matrice "matrix_path" del nodo S	*/

BOOL
total_route_flow (PGRAPH g, ITEM_HEAP * pr_queue, BOOL fault, NODEID node,
		  BOOL option_move)
{
  int i, num_nodes = num_nodes_graph (g);


  reset_all_sh_path (g, MP);
  if (fault == TRUE)
    {				/* si deve valutare la topologia con i guasti   */
      disable_node (g, node);
      for (i = 0; i < num_nodes; i++)
	{			/* nodi sorgente diversi dal nodo rotto k */
	  if (i != node)
	    {
	      dijkstra_heap (g, i, pr_queue);
	      if (insert_sh_path_in_matrix (g, i, g->node_set[i].matrix_path)
		  == DISCONNECTED)
		{
		  enable_node (g, node);
		  return DISCONNECTED;
		}
//                                      print_all_sh_path_from(stderr,g,g->node_set[i].matrix_path,i);
	    }
	}
      enable_node (g, node);
    }
  else
    {				/* topologia senza guasti       */
      for (i = 0; i < num_nodes; i++)
	{
	  dijkstra_heap (g, i, pr_queue);
	  if (insert_sh_path_in_matrix (g, i, g->node_set[i].matrix_path) ==
	      DISCONNECTED)
	    return DISCONNECTED;

//                      print_all_sh_path_from(stderr,g,g->node_set[i].matrix_path,i);
	}
    }

  /* Trovati tutti gli shortest path sul grafo ed aggiornate le matrici
     opportune che conservano tali cammini, si aggiornano i flussi        */

  if (fault == FALSE)
    reset_flow (g);
  else
    reset_broken_flow (g);
  update_flow (g, MP, trf_m, fault);
  return CONNECTED;
}

/* routing_incrementale: funzione che rompe tutti i nodi uno alla volta e 
	per ogni topologia ottenuta, reinstrada solo le connessioni che si appoggiano
	sui link non pi� abilitati a causa del guasto considerato	*/


void
routing_incrementale (FILE * fp, PGRAPH g, ITEM_HEAP * pr_queue)
{
  int k, i, j, t, num_nodes = num_nodes_graph (g);
  PLINK dis_link;
  PLINKLIST linklist;
  PITEM p;
  fprintf (fp, "\n\nRouting incrementale con topologia rotta \n");

  for (k = 0; k < num_nodes; k++)
    {				/* nodi rotti   */
      fprintf (fp, "\n\n\n******* Rompo il nodo %d ******\n", k);
      disable_node (g, k);
      reset_all_sh_path (g, IMP);

      /* link uscenti dal nodo rotto  */

      dis_link = first_link_out (g, k);
      while (dis_link != NULL)
	{
	  p = first_path_item (dis_link);
	  while (p != NULL)
	    {
	      if (path_from_to (g, item_from (p), IMP, item_to (p), 0) ==
		  NULL)
		{
		  /*      routing incrementale per la sorgente considerata non ancora
		     calcolato    */

		  dijkstra_heap (g, item_from (p), pr_queue);
		  insert_sh_path_in_matrix (g, item_from (p),
					    g->
					    node_set[item_from
						     (p)].incr_matrix_path);
		}
	      p = next_item (p);
	    }
	  dis_link = next_link (dis_link);
	}

      /* link in ingresso al nodo rotto: le connessioni le trovo solo sui
         link in out, quindi....      */

      linklist = get_linklist (g, k);
      while (linklist != NULL)
	{
	  dis_link = get_link_in (linklist);
	  p = first_path_item (dis_link);
	  while (p != NULL)
	    {
	      if (path_from_to (g, item_from (p), IMP, item_to (p), 0) ==
		  NULL)
		{		/*
				   routing incrementale per la sorgente considerata non ancora
				   calcolato        */

		  dijkstra_heap (g, item_from (p), pr_queue);
		  insert_sh_path_in_matrix (g, item_from (p),
					    g->
					    node_set[item_from
						     (p)].incr_matrix_path);
		}
	      p = next_item (p);
	    }
	  linklist = linklist_next (linklist);
	}


      /* vengono ricopiati in incr_matrix_path i cammini inalterati dalla
         rottura del nodo     */

      for (i = 0; i < num_nodes; i++)
	{			/* sorgenti     */
	  for (j = 0; j < num_nodes; j++)
	    {			/* destinazioni */
	      if (path_from_to (g, i, IMP, j, 0) == NULL && i != k && i != j
		  && j != k)
		{
		  for (t = 0; path_from_to (g, i, MP, j, t) != NULL; t++)
		    set_path_from_to (g, i, IMP, j, t, path_from_to (g, i, MP, j, t));
		}
	    }
	  print_all_sh_path_from (fp, g, g->node_set[i].incr_matrix_path, i);
	}
      enable_node (g, k);
    }
}

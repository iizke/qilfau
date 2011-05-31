#ifndef _GRAPH_
#define _GRAPH_

#define FREE_NODE -1
#define MP 0
#define IMP 1
#define MAX_NUM_NODES 100
#define MAX_NUM_LINKS 50

typedef struct _node *PNODE;	/* puntatore alla struttura _node */
typedef struct _link *PLINK;	/* puntatore alla struttura _list */
typedef struct _path_item *PITEM;	/* puntatore alla struttura PATH_ITEM */
typedef struct _graph *PGRAPH;	/* puntatore alla struttura _graph */


/*----------------------------------------------------------------------------*
Strutura del Link: Lista bilincata 
 *----------------------------------------------------------------------------*/

typedef struct _path_item
{
  NODEID node_from;
  NODEID node_to;
  TRAFFIC flow_item;
  struct _path_item *next_item;	/* lista bilincata */
  struct _path_item *pred_item;
}
PATH_ITEM;			/* identifica gli end-points di un path 
				   from->to con relativo traffico associato */
#define item_from(p) (p->node_from)
#define item_to(p) (p->node_to)
#define next_item(p) (p->next_item)
#define pred_item(p) (p->pred_item)
#define flow_item(p) (p->flow_item)


#define NULL_ITEM {-1,-1, 0.0, NULL, NULL}	/* Item nullo */

#ifdef MAIN_C
PATH_ITEM NullItem = NULL_ITEM;
#else
extern PATH_ITEM NullItem;
#endif

typedef struct _link
{
  int identif;
  NODEID node_from;		/* Identificativo numerico del nodo sorgente */
  NODEID node_to;		/* Identificativo numerico del nodo destinazione */
    COST (*economic_cost) (struct _link * p);	/* Costo monetario associato al 
						   link */
  COST routing_cost;		/* Costo associato al routing */
  struct _link *link_next;	/* Puntatore al link successivo in links_out */
  struct _link *link_pred;	/* Puntatore al link precedente in links_out */
  BOOL link_visited;		/* link visitato/no visitato dallo shpath */
  BOOL link_state;		/* link abilitato/rotto (no/si guasti sul link) */
  TRAFFIC flow_aggr;		/* Quantit� di traffico totale sul link */
  TRAFFIC flow_aggr_broken;	/* Traffico sul link in caso di rottura di un
				   nodo */
  TRAFFIC max_flow_on_link;	/* Massimo traffico che passa sul link
				   considerando tutti gli stati di 
				   funzionamento, rottura e non */
  CAPACITY capacity;		/* Capacit� sul link senza guasti */
  CAPACITY broken_capacity;	/* Capacit� sul link massima dovuta a guasti */
  PATH_ITEM *p_path;		/* Lista dei paths che si appoggiano sul link */
}
LINK;


#define next_link(p) (p->link_next)
#define pred_link(p) (p->link_pred)
#define link_from(p) (p->node_from)
#define link_to(p) (p->node_to)
#define route_cost_link(p) (p->routing_cost)
//#define economic_cost_link(p) (p->economic_cost(p))
#define economic_cost_link(p) (-1.0)
#define visited_link(p) (p->link_visited)
#define link_state(p) (p->link_state)

#define link_flow(p) (p->flow_aggr)
#define link_broken_flow(p) (p->flow_aggr_broken)
#define link_broken_max_flow(p) (p->max_flow_on_link)

#define link_capacity(p) (p->capacity)
#define link_broken_capacity(p) (p->broken_capacity)
#define first_path_item(p) (p->p_path)


//#define NULL_LINK {-1,-1,NULL,0.0,NULL,NULL,FALSE,TRUE,0.0,0.0,0.0,0.0,1,NULL}	/* LINK nullo */
#define NULL_LINK {-1,-1,-1,NULL,0.0,NULL,NULL,FALSE,TRUE,0.0,0.0,0.0,0.0,1,NULL} /* LINK nullo */

#ifdef MAIN_C
LINK NullLink = NULL_LINK;
#else
extern LINK NullLink;
#endif



/*----------------------------------------------------------------------------*
 Strutura del linklist
 *----------------------------------------------------------------------------*/
/* serve per memorizzare la lista dei link entranti in un nodo come lista */
/* si soli "PUNTATORI" a link. I link veri sono memorizzati nella lista dei */
/* link _uscenti_ first_link_out dal nodo */

typedef struct _link_list
{
  PLINK plink;			/* puntatore al link entrante */
  struct _link_list *linklist_next;	/*puntatore al prossimo in lista */
  struct _link_list *linklist_pred;	/*puntatore al precedente in lista */
}
LINKLIST;

typedef LINKLIST *PLINKLIST;
#define linklist_next(l) (l->linklist_next)
#define linklist_pred(l) (l->linklist_pred)
#define get_link_in(l) (l->plink)

#define NULL_LINKLIST {NULL,NULL,NULL}

#ifdef MAIN_C
LINKLIST NullLinkList = NULL_LINKLIST;
#else
extern LINKLIST NullLinkList;
#endif

/*----------------------------------------------------------------------------*
 Strutura del Nodo
 *----------------------------------------------------------------------------*/


typedef struct _node
{
  NODEID node_id;		/* Identificativo numerico del nodo */
  PLINK **matrix_path;		/* Matrice dinamica <dest>*<shpath> di puntatori 
				   PLINK (puntatori al link con cui arr. al nodo 
				   utilizzata dal routing non incrementale */

  PLINK **incr_matrix_path;	/* Matrice dinamica <dest>*<shpath> di puntatori 
				   PLINK (puntatori al link con cui arr. al nodo 
				   utilizzata dal routing incrementale */

  PLINK first_link_out;		/* Lista degli archi uscenti dal nodo */
  PLINKLIST first_linklist_in;
  /* Lista degli archi entranti nel nodo */
  /* sono solo puntatori a oggetti link memorizzati
     in first_link_out */

  int num_links_out;		/* Numero di links uscenti dal nodo */
  BOOL node_state;		/* nodo abilitato/rotto (no/si guasti sui nodi) */
  COST path_weight;		/* Usato dall'algoritmo di shortest path: 
				   memorizzo il peso del cammino minimo fino 
				   al nodo */
  PLINK path_link;		/* Puntatore al link con cui arrivo al nodo 
				   selezionato dall'algoritmo di shortest_path 
				   gestisco anche i link paralleli */
}
NODE;


#define node_id(g,i) (g->node_set[i].node_id)
#define node_state(g,i) (g->node_set[i].node_state)
#define node_path_weight(g,i) (g->node_set[i].path_weight)
#define num_links_out(g,i) (g->node_set[i].num_links_out)
#define first_link_out(g,i) (g->node_set[i].first_link_out)
#define get_linklist(g,n) ( g->node_set[n].first_linklist_in)
#define route_cost_node(g,i) (g->node_set[i].path_weight)
#define ingoing_link_to_node(g,i) (g->node_set[i].path_link)
#define path_from_to(g,s,b,d,k) ( (b==MP) ? (g->node_set[s].matrix_path[d][k]) : (g->node_set[s].incr_matrix_path[d][k]) )
#define set_path_from_to(g,s,b,d,k,v) ( (b==MP) ? (g->node_set[s].matrix_path[d][k]=v) : (g->node_set[s].incr_matrix_path[d][k]=v) )



/*----------------------------------------------------------------------------*
 Struttura del Grafo
 *----------------------------------------------------------------------------*/

/* Il Grafo � costituito da 5 campi + un vettore di puntatori alla
 struttura _node */

typedef struct _graph
{
  int max_num_nodes;		/* Numero massimo di nodi nel grafo */
  int max_num_links;		/* Numero massimo di link nel grafo */
  int num_nodes;		/* Numero corrente di nodi nel grafo */
  int num_links;		/* Numero corrente di link nel grafo */
  COST cost_graph;		/* Costo associato al grafo considerato */
  COST cost_broken_graph;	/* Costo associato al grafo considerato con guasti */
  NODE *node_set;		/* Vettore dinamico di strutture NODE */
}
GRAPH;

#define num_nodes_graph(g) (g->num_nodes)
#define num_links_graph(g) (g->num_links)
#define max_num_nodes_graph(g) ((g)->max_num_nodes)
#define max_num_links_graph(g) (g->max_num_links)
#define cost_graph(g) (g->cost_graph)
#define cost_broken_graph(g) (g->cost_broken_graph)
#define node_graph(g,i) (g->node_set[i])
#define increase_num_nodes(g) (g->num_nodes++)
#define decrease_num_nodes(g) (g->num_nodes--)
#define increase_num_links(g) (g->num_links++)
#define decrease_num_links(g) (g->num_links--)

#define NULL_GRAPH {0,0,0,0,0.0,0.0,NULL}

#ifdef MAIN_C
GRAPH NullGraph = NULL_GRAPH;
#else
extern GRAPH NullGraph;
#endif

#endif

#ifndef _ROUTING_
#define _ROUTING_

#include <math.h>

#define MAX_INT	INT_MAX
#define MAX_DOUBLE	DBL_MAX

typedef struct _item_heap
{
  NODEID node_id;		/* identificativo del nodo      */
  COST path_cost;		/* costo del minimo cammino temporaneo fino al nodo     */
}
ITEM_HEAP;


ITEM_HEAP *pr_queue;		/* vettore dinamico utilizzato per la gestione dell'heap
				 */
#endif

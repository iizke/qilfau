#include "main_graph.h"


/* abilita tutti i link entranti ed uscenti dal nodo	'node'	*/

void
enable_in_out_link (PGRAPH g, NODEID node)
{
  PLINK link;
  PLINKLIST linklist;

  /* link uscenti dal nodo 'node' */

  link = first_link_out (g, node);
  while (link != NULL)
    {
      link_state (link) = TRUE;	/* abilito il link */
      link = next_link (link);
    }

  /* link in ingresso nel nodo 'node'     */
  linklist = get_linklist (g, node);
  while (linklist != NULL)
    {
      link_state (get_link_in (linklist)) = TRUE;	/* abilito il link */
      linklist = linklist_next (linklist);
    }
}

/* Disabilita tutti i link entranti ed uscenti dal nodo	'node' */


void
disable_in_out_link (PGRAPH g, NODEID node)
{
  PLINK link;
  PLINKLIST linklist;

  /* link uscenti dal nodo 'node' */

  link = first_link_out (g, node);
  while (link != NULL)
    {
      link_state (link) = FALSE;	/* disabilito il link */
      link = next_link (link);
    }

  /* link in ingresso nel nodo 'node'     */
  linklist = get_linklist (g, node);
  while (linklist != NULL)
    {
      link_state (get_link_in (linklist)) = FALSE;	/* disabilito il link */
      linklist = linklist_next (linklist);
    }
}

/* Rompe un nodo 'node' nella topologia corrente memorizzata nel grafo 'g'	*/

void
disable_node (PGRAPH g, NODEID node)
{
  node_state (g, node) = FALSE;
  disable_in_out_link (g, node);
}


void
enable_node (PGRAPH g, NODEID node)
{
  node_state (g, node) = TRUE;
  enable_in_out_link (g, node);
}

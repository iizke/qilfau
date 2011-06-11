#include "main_graph.h"

/*              Funzioni di stampa per le strutture utilizzate          */

void
print_path_item (FILE * fp, PLINK p)
{
  PITEM p_path;
  for (p_path = first_path_item (p); p_path != NULL;
       p_path = next_item (p_path))
    fprintf (fp, "Rel: %d->%d Flow: %f\n", item_from (p_path),
	     item_to (p_path), flow_item (p_path));
}



void
print_list_link (int num_nodes, FILE * fp, PLINK p)	/* p:puntatore di testa della lista */
{
  PLINK p_link;
  double d, db;

  for (p_link = p; p_link != NULL; p_link = next_link (p_link))
    {
      d =
	1500.0 / ((link_capacity (p_link) - link_flow (p_link))) /
	1000000.0;
      db =
	1500.0 /
	((link_broken_capacity (p_link) - link_broken_max_flow (p_link))) /
	1000000.0;
      fprintf (fp,
	       "%d->%d\t|%12.3f\t|%12.3f\t|%12.3f\t|%12.3f\t|%12.3f\t|%12.3f\t|\n",
	       link_from (p_link), link_to (p_link), link_flow (p_link),
	       link_broken_max_flow (p_link), link_capacity (p_link),
	       link_broken_capacity (p_link), d, db);
//              print_path_item(fp, p_link);                                      
    }
}


void
print_graph (PGRAPH g, FILE * fp)
{
  int i;//, k;
  PLINK p_link;
  double d, db;
  double tot_weighted_d = 0.0, tot_weighted_db = 0.0, tot_flow = 0.0;
  double tot_d = 0.0, tot_db = 0.0, tot_link = 0.0;
  double max_flow = 0;

  fprintf (fp,
	   "\n\nNumero nodi=%d\t Cost Graph=%g\t Cost Broken Graph=%g \n\n",
	   num_nodes_graph (g), cost_graph (g), cost_broken_graph (g));

  fprintf (fp, "s = link source node\n");
  fprintf (fp, "d = link dest node\n");
  fprintf (fp, "F = Max flow [Mbps]\n");
  fprintf (fp, "Fb = Max flow on the broken graph [Mbps]\n");
  fprintf (fp, "C = Capacity of the link [Mbps]\n");
  fprintf (fp, "Cb = Capacity of the link in the broken graph [Mbps]\n");
  fprintf (fp, "D = Delay on the link [s]\n");
  fprintf (fp, "Db = Delay on the link in the broken graph [s]\n");

  fprintf (fp,
	   "\n+-------------------------------------------------------------------------------------------------------+\n");
  fprintf (fp, "| s->d|\t|F\t\t| Fb\t\t| C\t\t| Cb\t\t| D\t\t|Db\t\t|\n");
  fprintf (fp,
	   "+-------------------------------------------------------------------------------------------------------+\n");

  for (i = 0; i < num_nodes_graph (g); i++)
    {
      for (p_link = first_link_out (g, i); p_link != NULL;
	   p_link = next_link (p_link))
	{
	  d =
	    1500.0 / ((link_capacity (p_link) - link_flow (p_link))) /
	    1000000.0;
	  db =
	    1500.0 /
	    ((link_broken_capacity (p_link) - link_broken_max_flow (p_link)))
	    / 1000000.0;
	  fprintf (fp,
		   "| %d->%d\t|%12.3f\t|%12.3f\t|%12.3f\t|%12.3f\t|%12.6f\t|%12.6f\t|\n",
		   link_from (p_link), link_to (p_link), link_flow (p_link),
		   link_broken_max_flow (p_link), link_capacity (p_link),
		   link_broken_capacity (p_link), d, db);
	  tot_d += d;
	  tot_weighted_d += d * link_flow (p_link);
	  tot_weighted_db += db * link_flow (p_link);
	  tot_flow += link_flow (p_link);
	  max_flow = (max_flow < link_flow(p_link))?(link_flow(p_link)) : max_flow;
	  tot_db += db;
	  tot_link++;
	  //              print_path_item(fp, p_link);                                      
	}
    }

  fprintf (fp,
	   "+-------------------------------------------------------------------------------------------------------+\n");
  fprintf (fp,
	   "| N. link = %d\t Average delay = %8.6f\t Average Delay in the broken graph = %8.6f\t|\n",
	   (int) tot_link, tot_d / tot_link, tot_db / tot_link);
  fprintf (fp,
	   "| Tot flow = %8.3f\t Weighted Average delay = %8.6f\t Weighted Average Delay in the broken graph = %8.6f\t|\n",
	   tot_flow, tot_weighted_d / tot_flow, tot_weighted_db / tot_flow);
  fprintf (fp,
	   "+-------------------------------------------------------------------------------------------------------+\n");


  fprintf (fp, "\nNode - node route in the not failed topology\n");
  fprintf (fp, "+-----------------------------------------------+\n");
  fprintf (fp, "| s -> d \t| Cost\t| D[s] | Path list\t|\n");
  fprintf (fp, "+-----------------------------------------------+");

  reset_max_flow_field (g);
//  if (total_route_flow (g, pr_queue, FALSE, -1, NO_MOVE) != DISCONNECTED)
//    {
//      for (i = 0; i < num_nodes_graph (g); i++)
//	{
//	  print_all_sh_path_from (fp, g, g->node_set[i].matrix_path, i);
//	}
//    }
//  else
//    {
//      perror ("print_graph: grafo finale disconnesso!!\n");
//    }

  fprintf (fp, "\n+-----------------------------------------------+\n");

//  for (k = 0; k < num_nodes_graph (g); k++)
//    {
//      fprintf (fp,
//	       "\nNode - node route in the failed topology: NODE %d BROKEN\n",
//	       k);
//      fprintf (fp, "+-----------------------------------------------+\n");
//      fprintf (fp, "| s -> d \t| Cost\t| D[s] | Path list\t|\n");
//      fprintf (fp, "+-----------------------------------------------+");
//      if (total_route_flow (g, pr_queue, TRUE, k, NO_MOVE) != DISCONNECTED)
//	{
//	  for (i = 0; i < num_nodes_graph (g); i++)
//	    {
//	      print_all_sh_path_from (fp, g, g->node_set[i].matrix_path, i);
//	    }
//	}
//      else
//	{
//	  perror ("print_graph: grafo finale disconnesso!!\n");
//	}
//      fprintf (fp, "\n+-----------------------------------------------+\n");
//    }
}


void
print_test_graph (PGRAPH g)
{
  int i;
  PLINK p_link;
  PLINKLIST p_link_list;

  for (i = 0; i < num_nodes_graph (g); i++)
    {
      printf ("\n\n**** Nodo=%d\t Num Links Out=%d Abil=%d ****\n",
	      node_id (g, i), num_links_out (g, i), node_state (g, i));
      printf ("\n\nLink out dal nodo:\n");

      for (p_link = first_link_out (g, i); p_link != NULL;
	   p_link = next_link (p_link))
	{
	  printf (" %d->%d : Routing Cost=%f\t",
		  link_from (p_link), link_to (p_link),
		  route_cost_link (p_link));
	  printf (" Abil=%d\t Flow=%f\t Capacity=%f\n", link_state (p_link),
		  link_flow (p_link), link_capacity (p_link));
	}

      printf ("\n\nLink in nel nodo:\n");

      for (p_link_list = get_linklist (g, i); p_link_list != NULL;
	   p_link_list = linklist_next (p_link_list))
	{
	  printf (" %d->%d\t", link_from (get_link_in (p_link_list)),
		  link_to (get_link_in (p_link_list)));
	  printf (" Abil=%d\t Flow=%f\t Capacity=%f\n",
		  link_state (get_link_in (p_link_list)),
		  link_flow (get_link_in (p_link_list)),
		  link_capacity (get_link_in (p_link_list)));
	}
    }
}



void
read_graph (FILE * fl, PGRAPH g)
{
  int i, j, node_from, node_to, links_out;
  CAPACITY capacity;

  fscanf (fl, "%d %d\n", &(num_nodes_graph (g)), &(num_links_graph (g)));
  for (i = 0; i < num_nodes_graph (g); i++)
    {
      fscanf (fl, "%d %d", &node_from, &links_out);
      for (j = 0; j < links_out; j++)
	{
	  fscanf (fl, "%d %lf", &node_to, &capacity);
	  node_id (g, node_from) = node_from;
	  num_links_out (g, node_from) = links_out;
	  if (add_link (g, node_from, node_to, capacity) == NULL)
	    printf ("Errore nell'allocazione del nuovo link\n");
	}
    }
}

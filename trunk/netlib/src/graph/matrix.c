#include "main_graph.h"


/* Allocazione delle tre matrici: traffico, costo_setup e costo per unit� di
	  banda. */

/* Allocazione ed inizializzazione matrice di traffico  */

MATRIX alloc_matrix (MATRIX matrix, int n)
{
  int i, j;

  if ((matrix = (double **) malloc (sizeof (double *) * n)) == NULL)
    printf ("alloc_traffic_matrix: Errore allocazione memoria\n");

  for (i = 0; i < n; i++)
    {
      if ((matrix[i] = (double *) malloc (sizeof (double) * n)) == NULL)
	printf ("alloc_traffic_matrix: Errore allocazione memoria\n");
      for (j = 0; j < n; j++)
	matrix[i][j] = 0.0;
    }
  return (matrix);
}


int
read_input_matrix (int num_nodes, FILE * fp_trf,
                   FILE * fp_weight_route)
{
  int i, j, n;
  double temp;

  /* Alloca le matrici dinamiche utilizzate       */

  trf_m = alloc_matrix (trf_m, num_nodes);
  weight_route = alloc_matrix (weight_route, num_nodes);


  /*      ****************************    */
  /* Matrice di traffico in Mbit/s        */
  /*      ****************************    */

  if (fp_trf != NULL)
  {
      fscanf (fp_trf, "%d", &n);
      if (n != num_nodes)
        {
          error ("Warning: Incosistenza dei dati di ingresso: nodi diversi \n",
	         stderr);
          return (FALSE);
        }
      for (i = 0; i < num_nodes; i++)
        for (j = 0; j < num_nodes; j++)
          {
	    fscanf (fp_trf, "%lf", &temp);
	    entry (trf_m, i, j) = temp;
          }
  }
  else
  {
      for (i = 0; i < num_nodes; i++)
	{
	  for (j = 0; j < num_nodes; j++)
	    {
	      if (i != j)
		(entry (trf_m, i, j)) = 1.0;
	      else
		(entry (trf_m, i, j)) = 0;
	    }
	}
  }


  /*      *************************************************       */
  /* Pesi associati al routing per ogni coppia di link    */
  /*      *************************************************       */

  if (fp_weight_route != NULL)
    {
      fscanf (fp_weight_route, "%d", &n);
      if (n != num_nodes)
	{
	  error
	    ("Warning: Incosistenza dei dati di ingresso: nodi diversi \n",
	     stderr);
	  return (FALSE);
	}
      for (i = 0; i < num_nodes; i++)
	{
	  for (j = 0; j < num_nodes; j++)
	    {
	      fscanf (fp_weight_route, "%lf", &temp);
	      if (i != j)
		(entry (weight_route, i, j)) = temp;
	      else
		(entry (weight_route, i, j)) = 10000000000000;
	    }
	}
    }
  else
    {
      for (i = 0; i < num_nodes; i++)
	{
	  for (j = 0; j < num_nodes; j++)
	    {
	      if (i != j)
		(entry (weight_route, i, j)) = 1.0;
	      else
		(entry (weight_route, i, j)) = 10000000000000;
	    }
	}
    }
  return (TRUE);
}


void
print_matrix (MATRIX matrix, int n, FILE * fp_result)
{
  int i, j;
  TRAFFIC tot_traffic = 0.0;

  fprintf (fp_result, "\n");
  fprintf (fp_result, "\n");

  for (i = 0; i < n; i++)
    {
      for (j = 0; j < n; j++)
	{
	  fprintf (fp_result, "%8.4f ", entry (matrix, i, j));
	  tot_traffic += entry (matrix, i, j);
	}
      fprintf (fp_result, "\n");
    }
  fprintf (fp_result, "\n");

  if (matrix == trf_m)
    fprintf (fp_result, "TOTAL TRAFFIC [Mbit/s]= %6.3g \n", tot_traffic);

}

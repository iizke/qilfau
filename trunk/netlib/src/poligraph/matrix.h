#define MAXDELAY	10000

#ifndef _MATRIX_
#define _MATRIX_

typedef double **_MATRIX;
#define entry(pm,r,c)	(pm[r][c])

#endif


#ifndef _DISCRETE_CAP_MATRIX_
#define _DISCRETE_CAP_MATRIX_
typedef struct _field_dis_cap
{
  double dis_cap;
  double cost_cap;
}
FIELD_DIS_CAP;

typedef FIELD_DIS_CAP ***DISCRETE_CAP_MATRIX;
#define entry3_d_cap(pm,r,c,e)	(pm[r][c][e].dis_cap)
#define entry3_d_cost(pm,r,c,e)	(pm[r][c][e].cost_cap)
#endif

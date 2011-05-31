#ifndef _POLI_RANDOM_
#define _POLI_RANDOM_

/*
 * Module : random.c   
 */
long polirand_rnd32 (long);
double polirand_uniform (double, double, long *);
double polirand_negexp (double, long *);
int polirand_poisson (double, long *);
int polirand_geometric0 (double, long *);
int polirand_geometric1 (double, long *);
int polirand_geometric_trunc1 (double, int, long *);
int polirand_trunc_exp (double, long, long *);
double polirand_eval_gauss_sample (long *, double, double);
int polirand_randint (int, int, long *);
double polirand_weibull (double a, double b, long *);
double polirand_iperexp (double alpha, double mu_1, double mu_2, long *);
double polirand_pareto (double a, long *);
double polirand_erlang (double a, double M, long *);
double polirand_ipererl (double alpha, double m_1, double a_1, double m_2, double a_2,
		long *);


#endif

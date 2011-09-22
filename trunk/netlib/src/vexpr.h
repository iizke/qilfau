/**
 * @file vexpr.h
 * expression of variable
 * @date Created on: Aug 30, 2011
 * @author iizke
 */

#ifndef VEXPR_H_
#define VEXPR_H_

#include "def.h"
#include "list/tree.h"
#include "list/linked_list.h"

#define VEXPR_OP_PLUS       0
#define VEXPR_OP_MINUS      1
#define VEXPR_OP_MUL        2
#define VEXPR_OP_DIV        3
#define VEXPR_OP_ISCONST    4
#define VEXPR_OP_ISVAR      5
#define VEXPR_OP_GT         6
#define VEXPR_OP_LT         7
#define VEXPR_OP_ASSIGN     8
#define VEXPR_OP_INFER      9
#define VEXPR_OP_ANDO       10

#define VEXPR_STATE_NORMAL  0
#define VEXPR_STATE_WAITED  1
typedef struct vexpr_node {
  TREE_NODE tree;
  LINKED_LIST list;
  int id;
  int op_type;
  int state;
  double val;
} VEXPR_NODE;

/* use attached data structure (tree_node, or linked_list), we can change data
 * content of each structure
 */
typedef struct vexpr {
  TREE_NODE *root;
  LINKED_LIST vars;
} VEXPR;

VEXPR_NODE * vexpr_node_formular(int op_type, VEXPR_NODE* left, VEXPR_NODE *right);
VEXPR_NODE * vexpr_node_const (double val);
VEXPR_NODE * vexpr_node_var (int id, double init_val);
int vexpr_node_init (VEXPR_NODE *e);
int vexpr_assign_value(VEXPR *vexpr, VEXPR_NODE* val);
int vexpr_declare_var (VEXPR *vexpr, VEXPR_NODE *var);
VEXPR_NODE* vexpr_declare_varid (VEXPR *vexpr, int id, int init_val);
int vexpr_setup_var (VEXPR *vexpr, int id, double val);
VEXPR_NODE* vexpr_get_expr (VEXPR *vexpr);
double vexpr_calc (VEXPR* vexpr);
VEXPR_NODE* vexpr_get_var (VEXPR *vexpr, int id);
int vexpr_init (VEXPR *e);
VEXPR_NODE* vexpr_node_formular_vc (VEXPR *vexpr, int op_type, int id, int val);
VEXPR_NODE* vexpr_node_formular_cv (VEXPR *vexpr, int op_type, int val, int id);
VEXPR_NODE* vexpr_node_formular_vv (VEXPR *vexpr, int op_type, int id1, int id2);
#endif /* VEXPR_H_ */

/**
 * @file vexpr.c
 * Expression with variables
 *
 * @date Created on: Aug 30, 2011
 * @author iizke
 */

#include "vexpr.h"
#include "error.h"

static int _calc_expr_tree (TREE_NODE *t, void* no_use);

static double _calc_expr (int op_type, VEXPR_NODE *left, VEXPR_NODE *right) {
  double val;
  switch (op_type) {
  case VEXPR_OP_PLUS:
    val = left->val + right->val;
    break;
  case VEXPR_OP_MINUS:
    val = left->val - right->val;
    break;
  case VEXPR_OP_MUL:
    val = left->val * right->val;
    break;
  case VEXPR_OP_DIV:
    val = left->val / right->val;
    break;
  case VEXPR_OP_GT:
    val = left->val > right->val;
    break;
  case VEXPR_OP_LT:
    val = left->val < right->val;
    break;
  case VEXPR_OP_ASSIGN:
    if (left->op_type == VEXPR_OP_ISVAR) {
      left->val = right->val;
      val = 1;
    }
    else
      val = 0;
    break;
  case VEXPR_OP_ANDO:
    val = 1;
    break;
  case VEXPR_OP_INFER:
    if (left->val == 0) val = 0;
    else {
      right->tree.state = VEXPR_STATE_NORMAL;
      tree_do(&right->tree, NULL, _calc_expr_tree);
      right->tree.state = VEXPR_STATE_WAITED;
      val = 1;
    }
    break;
  case VEXPR_OP_EQUAL:
    val = (left->val == right->val);
    break;
  default:
    return 0;
  }
  return val;
}

static int _calc_expr_tree (TREE_NODE *t, void* no_use) {
  VEXPR_NODE * ve, *ve_left, *ve_right;

  if (!t)
    return ERR_TREE_NODE_NULL;
  ve = container_of(t, VEXPR_NODE, tree);

  if (t->left)
    ve_left = container_of(t->left, VEXPR_NODE, tree);
  if (t->right)
    ve_right = container_of(t->right, VEXPR_NODE, tree);

//  if (ve->state == VEXPR_STATE_WAITED)
//    return SUCCESS;

  if (ve->op_type != VEXPR_OP_ISCONST && ve->op_type != VEXPR_OP_ISVAR)
    ve->val = _calc_expr(ve->op_type, ve_left, ve_right);

  return SUCCESS;
}

VEXPR_NODE* vexpr_get_var (VEXPR *vexpr, int id) {
  LINKED_LIST * vp = NULL;
  linked_list_get_first(&vexpr->vars, &vp);
  while  (vp && vp != &(vexpr->vars)) {
    VEXPR_NODE *var = container_of(vp, VEXPR_NODE, list);
    if (var->id  == id && var->op_type == VEXPR_OP_ISVAR)
      return var;

    vp = vp->next;
  }
  return NULL;
}

int vexpr_setup_var(VEXPR *vexpr, int id, double val) {
  VEXPR_NODE *var = NULL;
  if ((var = vexpr_get_var(vexpr, id)) != NULL)
    var->val = val;
  else
    return ERR_VEXPR_VAR_NOEXIST;
  return SUCCESS;
}

int vexpr_declare_var (VEXPR *vexpr, VEXPR_NODE *var) {
  check_null_pointer(vexpr);
  check_null_pointer(var);
  if (var->op_type != VEXPR_OP_ISVAR)
    return ERR_VEXPR_NOT_VAR;
  if (vexpr_get_var(vexpr, var->id) != NULL)
    return ERR_VEXPR_DUP_VAR;

  linked_list_insert(&vexpr->vars, &var->list);
  return SUCCESS;
}

VEXPR_NODE* vexpr_declare_varid (VEXPR *vexpr, int id, int init_val) {
  VEXPR_NODE* v = NULL;
  if (!vexpr) return NULL;
  if ((v = vexpr_get_var(vexpr, id)) != NULL)
    return v;
  v = vexpr_node_var(id, init_val);
  vexpr_declare_var(vexpr, v);
  return v;
}

int vexpr_assign_value(VEXPR *vexpr, VEXPR_NODE* val) {
  check_null_pointer(vexpr);
  check_null_pointer(val);
  vexpr->root = &val->tree;
  return SUCCESS;
}

VEXPR_NODE* vexpr_get_expr (VEXPR *vexpr) {
  if (vexpr && vexpr->root)
    return container_of(vexpr->root, VEXPR_NODE, tree);

  return NULL;
}

double vexpr_calc (VEXPR* vexpr) {
  VEXPR_NODE *root = container_of(vexpr->root, VEXPR_NODE, tree);
  tree_do(vexpr->root, NULL, _calc_expr_tree);
  return root->val;
}

int vexpr_init (VEXPR *e) {
  e->root = NULL;
  linked_list_init(&e->vars);
  linked_list_init(&e->list);
  return SUCCESS;
}

int vexpr_new(VEXPR** vexpr) {
  (*vexpr) = malloc_gc(sizeof(VEXPR));
  if (!(*vexpr))
    return ERR_MALLOC_FAIL;
  vexpr_init((*vexpr));
  return SUCCESS;
}

int vexpr_node_init (VEXPR_NODE *e) {
  if (!e)
    return ERR_VEXPR_NULL;
  e->val = 0;
  e->id = 0;
  e->op_type = 0;
  //e->state = VEXPR_STATE_NORMAL;
  linked_list_init(&e->list);
  tree_init (&e->tree);
  return SUCCESS;
}

static int _vexpr_node_formular(VEXPR_NODE **node, int id, int op_type, VEXPR_NODE* left, VEXPR_NODE *right) {
  if (op_type != VEXPR_OP_ISVAR && op_type != VEXPR_OP_ISCONST)
  if ((left->op_type == VEXPR_OP_ISCONST) && (left->op_type == VEXPR_OP_ISCONST)) {
    *node = left;
    left->val = _calc_expr(op_type, left, right);
    gc_free(right);
    return SUCCESS;
  }

  *node = gc_malloc(sizeof (VEXPR_NODE));
  if (! (*node))
    return ERR_MALLOC_FAIL;

  vexpr_node_init(*node);

  if (left)
    (*node)->tree.left = &left->tree;

  if (right)
    (*node)->tree.right = &right->tree;

  (*node)->op_type = op_type;
  (*node)->id = id;

  if (op_type == VEXPR_OP_INFER)
    right->tree.state = VEXPR_STATE_WAITED;

  return SUCCESS;
}

VEXPR_NODE * vexpr_node_var (int id, double init_val) {
  VEXPR_NODE *var = NULL;
  _vexpr_node_formular(&var, id, VEXPR_OP_ISVAR, NULL, NULL);
  if (var)
    var->val = init_val;
  return var;
}

VEXPR_NODE* vexpr_node_const (double val) {
  VEXPR_NODE *var = NULL;
  _vexpr_node_formular(&var, 0, VEXPR_OP_ISCONST, NULL, NULL);
  var->val = val;
  return var;
}

VEXPR_NODE* vexpr_node_formular (int op_type, VEXPR_NODE* left, VEXPR_NODE *right) {
  VEXPR_NODE *f = NULL;
  if (op_type == VEXPR_OP_ISVAR)
    return NULL;
  _vexpr_node_formular(&f, 0, op_type, left, right);
  return f;
}

VEXPR_NODE* vexpr_node_formular_vc (VEXPR *vexpr, int op_type, int id, int val) {
  VEXPR_NODE *f = NULL;
  VEXPR_NODE *var = NULL;
  VEXPR_NODE *c = NULL;
  if (op_type == VEXPR_OP_ISVAR) {
    var = vexpr_declare_varid(vexpr, id, val);
    f = var;
  }
  else if (op_type == VEXPR_OP_ISCONST)
    f = vexpr_node_const(val);
  else {
    c = vexpr_node_const(val);
    var = vexpr_declare_varid(vexpr, id, 1);
    f = vexpr_node_formular(op_type, var, c);
  }
  return f;
}

VEXPR_NODE* vexpr_node_formular_cv (VEXPR *vexpr, int op_type, int val, int id) {
  VEXPR_NODE *f = NULL;
  VEXPR_NODE *var = NULL;
  VEXPR_NODE *c = NULL;
  if ((op_type == VEXPR_OP_ISVAR) || (op_type == VEXPR_OP_ISCONST))
    return NULL;

  c = vexpr_node_const(val);
  var = vexpr_declare_varid(vexpr, id, 1);
  f = vexpr_node_formular(op_type, c, var);
  return f;
}

VEXPR_NODE* vexpr_node_formular_vv (VEXPR *vexpr, int op_type, int id1, int id2) {
  VEXPR_NODE *f = NULL;
  VEXPR_NODE *v1 = NULL;
  VEXPR_NODE *v2 = NULL;
  if ((op_type == VEXPR_OP_ISVAR) || (op_type == VEXPR_OP_ISCONST))
    return NULL;

  v1 = vexpr_declare_varid(vexpr, id1, 1);
  v2 = vexpr_declare_varid(vexpr, id2, 1);
  f = vexpr_node_formular(op_type, v1, v2);
  return f;
}

int vexpr_list_setup_var (VEXPR_LIST *rules, int id, double val) {
  LINKED_LIST *next = NULL;
  check_null_pointer(rules);
  next = rules->next;
  while (next != rules) {
    VEXPR *vexpr = vexpr_from_linked_list(next);
    vexpr_setup_var(vexpr, id, val);
    next = next->next;
  }
  return SUCCESS;
}

int vexpr_list_calc_fast (VEXPR_LIST *rules) {
  LINKED_LIST *next = NULL;
  check_null_pointer(rules);
  next = rules->next;
  while (next != rules) {
    VEXPR *vexpr = vexpr_from_linked_list(next);
    vexpr_calc(vexpr);
    next = next->next;
  }
  return SUCCESS;
}

int vexpr_list_calc (VEXPR_LIST *rules, int nvars, const VEXPR_NODE *var_list) {
  LINKED_LIST *next = NULL;
  int i = 0;
  check_null_pointer(rules);
  next = rules->next;
  while (next != rules) {
    VEXPR *vexpr = vexpr_from_linked_list(next);
    for (i=0; i<nvars; i++) {
      vexpr_setup_var(vexpr, var_list[i].id, var_list[i].val);
    }
    vexpr_calc(vexpr);
    next = next->next;
  }
  return SUCCESS;
}

int vexpr_list_calc_1 (VEXPR_LIST *rules, int id, double val) {
  VEXPR_NODE var;
  var.id = id;
  var.val = val;
  return vexpr_list_calc (rules, 1, &var);
}

int vexpr_test() {
  VEXPR e;
  VEXPR_NODE *f = NULL;
  VEXPR_NODE *g = NULL;
  vexpr_init (&e);
  f = vexpr_node_formular_vc(&e, VEXPR_OP_PLUS, 1, 2);
  g = vexpr_node_formular_vc(&e, VEXPR_OP_MINUS, 1, 2);
  f = vexpr_node_formular(VEXPR_OP_MUL, f, g);
  vexpr_assign_value(&e, f);
  vexpr_setup_var(&e, 1, 11);
  vexpr_calc(&e);
  printf("result = %f\n",vexpr_calc(&e));
  return SUCCESS;
}

int vexpr_view_vars (VEXPR *vexpr) {
  check_null_pointer(vexpr);
  LINKED_LIST *next = vexpr->vars.next;
  printf("Result = %f \n", vexpr_calc(vexpr));
  while (next != &(vexpr->vars)) {
    VEXPR_NODE * var = container_of(next, VEXPR_NODE, list);
    printf("id = %d, val = %f\n", var->id, var->val);
    next = next->next;
  }
  return SUCCESS;
}

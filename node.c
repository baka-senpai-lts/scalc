#include "node.h"
#include "util.h"

#include <stdlib.h>

sc_Node *sc_construct_node(void *l, sc_NodeType l_type, sc_Operation op,
                           void *r, sc_NodeType r_type) {
  sc_Node *node = malloc(sizeof(sc_Node));

  node->l = l;
  node->l_type = l_type;
  node->op = op;
  node->r = r;
  node->r_type = r_type;

  return node;
}

sc_Node *sc_copy_node_tree(const sc_Node *node) {
  sc_Node *result = malloc(sizeof(sc_Node));

  result->l = NULL;
  result->l_type = node->l_type;
  result->r = NULL;
  result->r_type = node->r_type;
  result->op = node->op;

  switch (node->l_type) {
  case NODE_NODE:
    result->l = sc_copy_node_tree(node->l);
    break;
  case NODE_INT:
    result->l = malloc(sizeof(int));
    *(int *)result->l = *(int *)node->l;
    break;
  case NODE_FLOAT:
    result->l = malloc(sizeof(float));
    *(float *)result->l = *(float *)node->l;
    break;
  case NODE_VAR:
  case NODE_LITERAL:
    result->l = sc_alloc_strcpy(node->l);
    break;
  case NODE_NONE:
    break;
  default:
    break;
  }

  switch (node->r_type) {
  case NODE_NODE:
    result->r = sc_copy_node_tree(node->r);
    break;
  case NODE_INT:
    result->r = malloc(sizeof(int));
    *(int *)result->r = *(int *)node->r;
    break;
  case NODE_FLOAT:
    result->r = malloc(sizeof(float));
    *(float *)result->r = *(float *)node->r;
    break;
  case NODE_VAR:
  case NODE_LITERAL:
    result->r = sc_alloc_strcpy(node->r);
    break;
  case NODE_NONE:
    break;
  default:
    break;
  }

  return result;
}

void sc_free_node_tree_children(const sc_Node *node) {
  if (node->l_type == NODE_NODE && node->l != NULL) {
    sc_free_node_tree_children(node->l);
  }

  if (node->r_type == NODE_NODE && node->r != NULL) {
    sc_free_node_tree_children(node->r);
  }

  if (node->l_type != NODE_NONE && node->l != NULL) {
    free(node->l);
  }

  if (node->r_type != NODE_NONE && node->r != NULL) {
    free(node->r);
  }
}

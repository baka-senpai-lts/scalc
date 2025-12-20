#include "evaluator.h"

#include <assert.h>
#include <stdlib.h>

#include "node.h"
#include "util.h"

static int sc_node_to_int(void *val, sc_NodeType type) {
  switch (type) {
  case NODE_INT:
    return *(int *)val;
  case NODE_FLOAT:
    return (int)*(float *)val;
  case NODE_VAR:
    // TODO: fetch from context
  case NODE_LITERAL:
  case NODE_NONE:
  case NODE_NODE:
    /* assert(0 && "Passed non-end node to sc_node_to_int"); */
    return 0;
  }

  return 0;
}

static float sc_node_to_float(void *val, sc_NodeType type) {
  switch (type) {
  case NODE_INT:
    return (float)*(int *)val;
  case NODE_FLOAT:
    return *(float *)val;
  case NODE_VAR:
    // TODO: fetch from context
  case NODE_LITERAL:
  case NODE_NONE:
  case NODE_NODE:
    /* assert(0 && "Passed non-end node to sc_node_to_float"); */
    return 0;
  }

  return 0;
}

void sc_substitute_var(sc_Node *root, char *var, sc_Result sub) {
  if (root->l_type == NODE_NODE) {
    sc_substitute_var(root->l, var, sub);
  }

  if (root->r_type == NODE_NODE) {
    sc_substitute_var(root->r, var, sub);
  }

  if (root->l_type == NODE_VAR && sc_strcmp(root->l, var)) {
    free(root->l);
    root->l_type = sc_result_type_to_node_type(sub.type);

    switch (sub.type) {
    case RESULT_INT:
      root->l = malloc(sizeof(int));
      *(int *)(root->l) = *(int *)sub.result;
      break;
    case RESULT_FLOAT:
      root->l = malloc(sizeof(float));
      *(float *)(root->l) = *(float *)sub.result;
      break;
    case RESULT_LAMBDA:
      root->l = sc_copy_node_tree(sub.result);
      break;
    case RESULT_UNDEFINED:
      root->l = NULL;
      break;
    }

    root->l_type = sc_result_type_to_node_type(sub.type);
  }

  if (root->r_type == NODE_VAR && sc_strcmp(root->r, var)) {
    free(root->r);
    root->r_type = sc_result_type_to_node_type(sub.type);

    switch (sub.type) {
    case RESULT_INT:
      root->r = malloc(sizeof(int));
      *(int *)(root->r) = *(int *)sub.result;
      break;
    case RESULT_FLOAT:
      root->r = malloc(sizeof(float));
      *(float *)(root->r) = *(float *)sub.result;
      break;
    case RESULT_LAMBDA:
      root->r = sc_copy_node_tree(sub.result);
      break;
    case RESULT_UNDEFINED:
      root->r = NULL;
      break;
    }

    root->r_type = sc_result_type_to_node_type(sub.type);
  }
}

sc_IntPair sc_end_node_to_int_pair(const sc_Node *node) {
  sc_IntPair pair = {.l = sc_node_to_int(node->l, node->l_type),
                     .r = sc_node_to_int(node->r, node->r_type)};

  return pair;
}

sc_FloatPair sc_end_node_to_float_pair(const sc_Node *node) {
  sc_FloatPair pair = {.l = sc_node_to_float(node->l, node->l_type),
                       .r = sc_node_to_float(node->r, node->r_type)};

  return pair;
}

sc_ResultType sc_node_type_to_result_type(sc_NodeType n) {
  switch (n) {
  case NODE_NODE:
    return RESULT_UNDEFINED;
  case NODE_INT:
    return RESULT_INT;
  case NODE_FLOAT:
    return RESULT_FLOAT;
  default:
    break;
  }

  return RESULT_UNDEFINED;
}

sc_NodeType sc_result_type_to_node_type(sc_ResultType r) {
  switch (r) {
  case RESULT_UNDEFINED:
    return NODE_NONE;
    /* assert(0 && "RESULT_UNDEFINED passed to sc_result_type_to_node_type"); */
    break;
  case RESULT_INT:
    return NODE_INT;
  case RESULT_FLOAT:
    return NODE_FLOAT;
  default:
    break;
  }

  // Not sure what I actually should return if it failed, but this seems about
  // right I guess
  // TODO: add NODE_WTF
  return NODE_NODE;
}

sc_Result sc_allocate_result(sc_ResultType type) {
  sc_Result result = {.type = type};

  switch (type) {
  case RESULT_UNDEFINED:
    result.result = NULL; // We are not failing
    break;
  case RESULT_INT:
    result.result = malloc(sizeof(int));
    break;
  case RESULT_FLOAT:
    result.result = malloc(sizeof(float));
    break;
  case RESULT_LAMBDA:
    // Nothing to do, node must be allocated separately
    break;
  }

  return result;
}

int sc_free_result(sc_Result result) {
  if (result.type != RESULT_UNDEFINED && result.result != NULL) {
    if (result.type == RESULT_LAMBDA) {
      sc_free_node_tree_children(result.result);
    }

    free(result.result);
    return 0;
  }

  return 1;
}

sc_ResultType sc_deduce_result_type(const sc_Node *node) {
  sc_ResultType l = sc_node_type_to_result_type(node->l_type);
  sc_ResultType r = sc_node_type_to_result_type(node->r_type);

  sc_NodeType result_node_type = sc_max(node->l_type, node->r_type);

  if (node->op == OP_LAMBDA) {
    return RESULT_LAMBDA;
  }

  if (node->op == OP_NONE) {
    if (l == RESULT_UNDEFINED && node->l_type != NODE_VAR &&
        node->l_type != NODE_LITERAL && node->l_type != NODE_NONE) {
      l = sc_deduce_result_type(node->l);
    }

    return l;
  }

  if (result_node_type == NODE_NODE) {
    if (l == RESULT_UNDEFINED && node->l_type != NODE_VAR &&
        node->l_type != NODE_LITERAL && node->l_type != NODE_NONE) {
      l = sc_deduce_result_type(node->l);
    }

    if (r == RESULT_UNDEFINED && node->r_type != NODE_VAR &&
        node->r_type != NODE_LITERAL && node->r_type != NODE_NONE) {
      r = sc_deduce_result_type(node->r);
    }

    if (node->op == OP_DIVISION) {
      return sc_max(l, sc_max(r, RESULT_FLOAT));
    } else {
      return sc_max(l, r);
    }
  }

  if (node->op == OP_DIVISION) {
    // Special fucking case
    return sc_node_type_to_result_type(sc_max(result_node_type, NODE_FLOAT));
  } else {
    return sc_node_type_to_result_type(result_node_type);
  }
}

static void sc_evaluate_children(sc_Node *node) {
  if (node->l_type == NODE_NODE) {
    sc_Result tmp_result = sc_evaluate_node(node->l);

    sc_free_node_tree_children(node->l);
    free(node->l);

    node->l = tmp_result.result;
    node->l_type = sc_result_type_to_node_type(tmp_result.type);
  }

  if (node->r_type == NODE_NODE) {
    sc_Result tmp_result = sc_evaluate_node(node->r);

    sc_free_node_tree_children(node->r);
    free(node->r);

    node->r = tmp_result.result;
    node->r_type = sc_result_type_to_node_type(tmp_result.type);
  }
}

sc_Result sc_evaluate_none(sc_Node *node) {
  assert((node->op == OP_NONE) &&
         "Non-none operation node passed to sc_evaluate_none");
  sc_Result result;

  result = sc_allocate_result(sc_deduce_result_type(node));

  sc_evaluate_children(node);

  switch (result.type) {
  case RESULT_INT:
    *(int *)result.result = *(int *)node->l;
    break;
  case RESULT_FLOAT:
    *(float *)result.result = *(float *)node->l;
    break;
  case RESULT_LAMBDA:
    result.result = sc_copy_node_tree(node->l);
    break;
  case RESULT_UNDEFINED:
    break;
  }

  return result;
}

sc_Result sc_evaluate_plus(sc_Node *node) {
  assert((node->op == OP_PLUS) &&
         "Non-plus operation node passed to sc_evaluate_plus");

  sc_Result result = sc_allocate_result(sc_deduce_result_type(node));

  sc_evaluate_children(node);

  switch (result.type) {
  case RESULT_INT: {
    sc_IntPair pair = sc_end_node_to_int_pair(node);

    *(int *)result.result = pair.l + pair.r;
    break;
  }

  case RESULT_FLOAT: {
    sc_FloatPair pair = sc_end_node_to_float_pair(node);

    *(float *)result.result = pair.l + pair.r;
    break;
  }

  default:
    break;
  }

  return result;
}

sc_Result sc_evaluate_minus(sc_Node *node) {
  assert((node->op == OP_MINUS) &&
         "Non-plus operation node passed to sc_evaluate_minus");
  sc_Result result = sc_allocate_result(sc_deduce_result_type(node));

  sc_evaluate_children(node);

  switch (result.type) {
  case RESULT_INT: {
    sc_IntPair pair = sc_end_node_to_int_pair(node);

    if (node->r_type == NODE_NONE) {
      *(int *)result.result = -pair.l;
      break;
    }

    *(int *)result.result = pair.l - pair.r;
    break;
  }

  case RESULT_FLOAT: {
    sc_FloatPair pair = sc_end_node_to_float_pair(node);

    if (node->r_type == NODE_NONE) {
      *(float *)result.result = -pair.l;
      break;
    }

    *(float *)result.result = pair.l - pair.r;
    break;
  }

  default:
    break;
  }

  return result;
}

sc_Result sc_evaluate_multiplication(sc_Node *node) {
  assert((node->op == OP_MULTIPLICATION) &&
         "Non-plus operation node passed to sc_evaluate_multiplication");
  sc_Result result = sc_allocate_result(sc_deduce_result_type(node));

  sc_evaluate_children(node);

  switch (result.type) {
  case RESULT_INT: {
    sc_IntPair pair = sc_end_node_to_int_pair(node);

    if (node->r_type == NODE_NONE) {
      pair.r = 1;
    }

    *(int *)result.result = pair.l * pair.r;
    break;
  }

  case RESULT_FLOAT: {
    sc_FloatPair pair = sc_end_node_to_float_pair(node);

    if (node->r_type == NODE_NONE) {
      pair.r = 1.f;
    }

    *(float *)result.result = pair.l * pair.r;
    break;
  }

  default:
    // No idea how you can reach it, but better be safe than sorry
    /* assert(0 && "Undefined result type"); */
    break;
  }

  return result;
}

sc_Result sc_evaluate_division(sc_Node *node) {
  assert((node->op == OP_DIVISION) &&
         "Non-plus operation node passed to sc_evaluate_division");
  sc_Result result = sc_allocate_result(RESULT_FLOAT);

  sc_evaluate_children(node);

  sc_FloatPair pair = sc_end_node_to_float_pair(node);

  if (node->r_type == NODE_NONE) {
    pair.r = 1.f;
  }

  *(float *)result.result = pair.l / pair.r;

  return result;
}

sc_Result sc_evaluate_lambda(sc_Node *node) {
  assert((node->op == OP_LAMBDA) &&
         "Non-lambda operation node passed to sc_evaluate_lambda");

  sc_Result result;

  // We really don't give a fuck here
  result.type = RESULT_LAMBDA;
  result.result = sc_copy_node_tree(node);

  /* sc_free_node_tree_children(node); */
  /* free(node); */

  return result;
}

sc_Result sc_evaluate_apply(sc_Node *node) {
  assert((node->op == OP_APPLY) &&
         "Non-apply operation node passed to sc_evaluate_apply");

  const sc_Result undefined = {.type = RESULT_UNDEFINED, .result = NULL};
  sc_Result sub;
  sc_Node *lambda;
  sc_Result l_evaluated = {.type = RESULT_UNDEFINED, .result = NULL};

  // Variable thing on the right
  // Lambda on the left
  // Go through all subtrees and replace all NODE_VAR with name on lambda's
  // literal to our right. Evaluate before replacing

  // Check if left is a node
  if (node->l_type != NODE_NODE) {
    return undefined;
  }

  // Check if left already is a lambda
  if (((sc_Node *)node->l)->op == OP_LAMBDA) {
    lambda = node->l;
  } else {
    // Check if it's evaluated to lambda
    l_evaluated = sc_evaluate_node(node->l);
    if (l_evaluated.type != RESULT_LAMBDA) {
      sc_free_result(l_evaluated);
      return undefined;
    }

    lambda = l_evaluated.result;
  }

  // lambda is now guaranteed to exist

  // Check if right is a node
  if (node->r_type == NODE_NODE) {
    sub = sc_evaluate_node(node->r);
  } else {
    sub = sc_allocate_result(sc_node_type_to_result_type(node->r_type));

    switch (sub.type) {
    case RESULT_UNDEFINED:
      sc_free_result(sub);
      return undefined;
    case RESULT_INT:
      *(int *)sub.result = *(int *)node->r;
      break;
    case RESULT_FLOAT:
      *(float *)sub.result = *(float *)node->r;
      break;
    case RESULT_LAMBDA:
      sub.result = sc_copy_node_tree(node->r);

      // Freeing after evaluation
      sc_free_node_tree_children(node->r);
      free(node->r);
      break;
    }
  }

  // sub is now guaranteed to have value

  /* if (lambda->r_type == ) { */
  /*   sc_substitute_var(lambda->r, lambda->l, sub); */
  /* } */

  // Substitution copies values, so we have to clean up
  sc_Result result;

  // Already guaranteed to be a node
  sc_substitute_var(lambda, lambda->l, sub);

  if (lambda->r_type == NODE_NODE) {
    result = sc_evaluate_node(lambda->r);
  } else {
    result = sc_allocate_result(sc_node_type_to_result_type(lambda->r_type));

    switch (result.type) {
    case RESULT_UNDEFINED:
      sc_free_result(sub);
      sc_free_result(l_evaluated);
      return undefined;
    case RESULT_INT:
      *(int *)result.result = *(int *)lambda->r;
      break;
    case RESULT_FLOAT:
      *(float *)result.result = *(float *)lambda->r;
      break;
    case RESULT_LAMBDA:
      result.result = sc_copy_node_tree(lambda->r);
      break;
    }
  }

  sc_free_result(sub);
  sc_free_result(l_evaluated);

  return result;
}

sc_Result sc_evaluate_node(sc_Node *node) {
  sc_Result result;
  result.type = RESULT_UNDEFINED;

  switch (node->op) {
  case OP_NONE:
    result = sc_evaluate_none(node);
    break;
  case OP_PLUS:
    result = sc_evaluate_plus(node);
    break;
  case OP_MINUS:
    result = sc_evaluate_minus(node);
    break;
  case OP_MULTIPLICATION:
    result = sc_evaluate_multiplication(node);
    break;
  case OP_DIVISION:
    result = sc_evaluate_division(node);
    break;
  case OP_LAMBDA:
    result = sc_evaluate_lambda(node);
    break;
  case OP_APPLY:
    result = sc_evaluate_apply(node);
    break;
  }

  return result;
}

sc_Result sc_evaluate_node_safe(const sc_Node *node) {
  sc_Node *node_copy = sc_copy_node_tree(node);
  sc_Result result = sc_evaluate_node(node_copy);

  sc_free_node_tree_children(node_copy);
  free(node_copy);

  return result;
}

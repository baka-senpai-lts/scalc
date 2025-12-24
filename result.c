#include "result.h"

#include <stdlib.h>

#include "node.h"

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
  case RESULT_NODE:
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

    if (result.type == RESULT_NODE) {
      sc_free_node_tree_children(result.result);
    }

    free(result.result);
    return 0;
  }

  return 1;
}

sc_Result sc_copy_result(sc_Result original) {
  sc_Result copy = sc_allocate_result(original.type);

  switch (copy.type) {
  case RESULT_INT:
    *(int *)copy.result = *(int *)original.result;
    break;
  case RESULT_FLOAT:
    *(float *)copy.result = *(float *)original.result;
    break;
  case RESULT_NODE:
  case RESULT_LAMBDA:
    copy.result = sc_copy_node_tree(original.result);
    break;
  case RESULT_UNDEFINED:
    break;
  }

  return copy;
}

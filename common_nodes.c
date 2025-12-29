#include "common_nodes.h"

// TODO: construct them manually
#include "parser.h"
#include "result.h"
#include "util.h"

sc_Result sc_construct_true(void) {
  sc_Result result = {.result = sc_str_to_node("\\ 'x 'y (x)", NULL),
					  .type = RESULT_LAMBDA};

  return result;
}

sc_Result sc_construct_false(void) {
  sc_Result result = {.result = sc_str_to_node("\\ 'x 'y (y)", NULL),
					  .type = RESULT_LAMBDA};

  return result;
}

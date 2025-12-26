#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "result.h"
#include "context.h"
#include "node.h"

typedef struct {
  int l;
  int r;
} sc_IntPair;

typedef struct {
  float l;
  float r;
} sc_FloatPair;

sc_ResultType sc_node_type_to_result_type(sc_NodeType);
sc_NodeType sc_result_type_to_node_type(sc_ResultType);

// Takes type with highest priority
sc_ResultType sc_deduce_result_type(const sc_Node *, sc_Context **);

sc_IntPair sc_end_node_to_int_pair(const sc_Node *, sc_Context **);
sc_FloatPair sc_end_node_to_float_pair(const sc_Node *, sc_Context **);

void sc_substitute_var(sc_Node *, char *, sc_Result);

sc_Result sc_evaluate_none(sc_Node *, sc_Context **);

sc_Result sc_evaluate_set_eager(sc_Node *, sc_Context **);
sc_Result sc_evaluate_set_lazy(sc_Node *, sc_Context **);

sc_Result sc_evaluate_apply(sc_Node *, sc_Context **);

// These functions mutate node tree, PLEASE DO NOT USE IT RAW
sc_Result sc_evaluate_plus(sc_Node *, sc_Context **);
sc_Result sc_evaluate_minus(sc_Node *, sc_Context **);
sc_Result sc_evaluate_multiplication(sc_Node *, sc_Context **);
sc_Result sc_evaluate_division(sc_Node *, sc_Context **);

sc_Result sc_evaluate_node(sc_Node *, sc_Context **);

sc_Result sc_evaluate_lambda(sc_Node *);

// This thing you can use
sc_Result sc_evaluate_node_safe(const sc_Node *, sc_Context **);

#endif // EVALUATOR_H_

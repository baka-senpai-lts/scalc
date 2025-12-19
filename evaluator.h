#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "node.h"

// No I won't support complex numbers, fractions and other shit
typedef enum {
  RESULT_UNDEFINED = 255,
  RESULT_INT = 1,
  RESULT_FLOAT = 2,
  RESULT_LAMBDA = 3
} sc_ResultType;

typedef struct {
  void *result;
  sc_ResultType type;
} sc_Result;

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
sc_ResultType sc_deduce_result_type(const sc_Node *);

sc_Result sc_allocate_result(sc_ResultType);
int sc_free_result(sc_Result);

sc_IntPair sc_end_node_to_int_pair(const sc_Node *);
sc_FloatPair sc_end_node_to_float_pair(const sc_Node *);

void sc_substitute_var(sc_Node *, char *, sc_Result);

sc_Result sc_evaluate_none(sc_Node *);

// These functions mutate node tree, PLEASE DO NOT USE IT RAW
sc_Result sc_evaluate_plus(sc_Node *);
sc_Result sc_evaluate_minus(sc_Node *);
sc_Result sc_evaluate_multiplication(sc_Node *);
sc_Result sc_evaluate_division(sc_Node *);

sc_Result sc_evaluate_node(sc_Node *);

sc_Result sc_evaluate_lambda(sc_Node *);

// This thing you can use
sc_Result sc_evaluate_node_safe(const sc_Node *);

#endif // EVALUATOR_H_

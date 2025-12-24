#ifndef RESULT_H_
#define RESULT_H_

// No I won't support complex numbers, fractions and other shit
typedef enum {
  RESULT_UNDEFINED = 255,
  RESULT_INT = 1,
  RESULT_FLOAT = 2,
  RESULT_LAMBDA = 3,

  // A very special case, shouldn't appear anywhere outside of global context
  RESULT_NODE = 4
} sc_ResultType;

typedef struct {
  void *result;
  sc_ResultType type;
} sc_Result;

sc_Result sc_allocate_result(sc_ResultType);
int sc_free_result(sc_Result);

// Performs a deep copy of a result
sc_Result sc_copy_result(sc_Result);

#endif // RESULT_H_

#ifndef COMMON_NODES_H_
#define COMMON_NODES_H_

#include "result.h"

// All functions in here MUST return sc_Result with a type RESULT_NODE
// Note from future: no they don't

sc_Result sc_construct_true(void);
sc_Result sc_construct_false(void);

#endif // COMMON_NODES_H_

#include "context.h"

#include <stdlib.h>

#include "result.h"
#include "node.h"
#include "util.h"

sc_Result sc_context_get(sc_Context *ctx, const char *key) {
  const sc_Result undefined = {.result = NULL, .type = RESULT_UNDEFINED};

  if (ctx == NULL) {
    return undefined;
  }

  if (!sc_strcmp(ctx->key, key)) {
    return sc_context_get(ctx->next, key);
  }

  // Undefined doesn't allocate anything, so we aren't leaking
  sc_Result result = sc_allocate_result(ctx->value.type);

  switch (ctx->value.type) {
  case RESULT_UNDEFINED:
    return undefined;

  case RESULT_INT:
    *(int *)result.result = *(int *)ctx->value.result;
    break;

  case RESULT_FLOAT:
    *(float *)result.result = *(float *)ctx->value.result;
    break;

  case RESULT_NODE:
  case RESULT_LAMBDA:
    result.result = sc_copy_node_tree(ctx->value.result);
    break;
  }

  return result;
}

void sc_context_set(sc_Context **ctx_ptr, const char *key, sc_Result value) {
  // I know I could push it to the beginning for O(1) by modifying initial
  // pointer, but I have a slight feeling that it will end awfully
  sc_Context *ctx = *ctx_ptr;

  if (ctx != NULL) {
    if (sc_strcmp(ctx->key, key)) {
      // Overriding
      sc_free_result(ctx->value);
      goto value_copy;
    }

    sc_context_set((sc_Context **)&(ctx->next), key, value);
    return;
  }

  *ctx_ptr = malloc(sizeof(sc_Context));
  ctx = *ctx_ptr;

  ctx->next = NULL;
  ctx->key = sc_alloc_strcpy(key);

value_copy:
  ctx->value = sc_allocate_result(value.type);

  switch (value.type) {
  case RESULT_UNDEFINED:
    break;

  case RESULT_INT:
    *(int *)ctx->value.result = *(int *)value.result;
    break;

  case RESULT_FLOAT:
    *(float *)ctx->value.result = *(float *)value.result;
    break;

  case RESULT_NODE:
  case RESULT_LAMBDA:
    ctx->value.result = sc_copy_node_tree(value.result);
    break;
  }
}

void sc_context_free(sc_Context *ctx) {
	if (ctx == NULL) {
		return;
	}

	sc_context_free(ctx->next);

	sc_free_result(ctx->value);
	free(ctx->key);
	free(ctx);
}

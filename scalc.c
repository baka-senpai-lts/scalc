#include "scalc.h"

#include <stdio.h>
#include <stdlib.h>

#include "evaluator.h"
#include "node.h"
#include "parser.h"
#include "printer.h"
#include "repl.h"
#include "result.h"
#include "util.h"
#include "context.h"

int main(void) {
  // Too much for stack to hold consistently
  char *buffer = malloc(sizeof(char) * BUFSIZ);
  sc_Context *ctx = NULL;

  for (;;) {
    // Because fuck you
    sc_fill_seq_with_bytes(buffer, '\0', BUFSIZ);

    int rc = sc_getline(" > ", buffer, BUFSIZ);

    if (rc == REPL_TOO_LONG) {
      puts("#> Why the fuck did you write this long expression?\n");
      continue;
    }

    if (rc == REPL_EMPTY) {
      puts("#> EOF\n");
      break;
    }

    sc_Node *root = sc_str_to_node(buffer, NULL);

    fputs("~> ", stdout);
    sc_print_node(root);
    puts("");

    sc_Result result = sc_evaluate_node_safe(root, &ctx);

    switch (result.type) {
    case RESULT_INT:
      printf("=> %d\n", *(int *)result.result);
      break;
    case RESULT_FLOAT:
      printf("=> %f\n", (double)*(float *)result.result);
      break;
    case RESULT_STRING:
      printf("=> \"%s\"\n", (char *)result.result);
      break;
    case RESULT_LAMBDA:
      printf("=> <lambda> ");
      sc_print_node(result.result);
      puts("");
      break;
    case RESULT_NODE:
      printf("=> <node> ");
      sc_print_node(result.result);
      puts("");
      break;
    default:
      printf("=> Undefined\n");
      break;
    }
    puts("");

    sc_free_result(result);
    sc_free_node_tree_children(root);
    free(root);
  }

  sc_context_free(ctx);
  free(buffer);
  puts("If you can read this, WE DID NOT SEGFAULT");

  return 0;
}

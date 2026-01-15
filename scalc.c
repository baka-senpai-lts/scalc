#include "scalc.h"

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "file_reader.h"
#include "evaluator.h"
#include "node.h"
#include "parser.h"
#include "printer.h"
#include "repl.h"
#include "result.h"
#include "util.h"
#include "context.h"

int main(int argc, char **argv) {
  sc_Config config = sc_parse_args(argc, argv);

  // Too much for stack to hold consistently
  char *buffer = malloc(sizeof(char) * BUFSIZ);
  sc_Context *ctx = NULL;

  for (int i = 0; i < config.files_amount; i++) {
    int error = 0;

    sc_FileMap file = sc_map_text_file(config.load_files[i], &error);

    if (error != SC_OK) {
      fprintf(stderr, "Error opening file '%s': %s\n", config.load_files[i],
              sc_file_error_as_string(error));

      sc_free_config(&config);
      sc_context_free(ctx);
      free(buffer);
      exit(1);
    }

    sc_Node *file_root = sc_str_to_node(file.ptr, NULL);

    // We only need to populate context from there
    sc_free_result(sc_evaluate_node_safe(file_root, &ctx));

    sc_free_node_tree_children(file_root);
    free(file_root);

    sc_unmap_file(&file);
  }

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
    case RESULT_LITERAL:
      printf("=> <literal> %s\n", (char *)result.result);
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

  sc_free_config(&config);
  sc_context_free(ctx);
  free(buffer);
  puts("If you can read this, WE DID NOT SEGFAULT");

  return 0;
}

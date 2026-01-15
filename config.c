#include "config.h"
#include "util.h"

#include <stdlib.h>

sc_Config sc_parse_args(int argc, char **argv) {
  sc_Config config = {.files_amount = 0, .load_files = NULL};

  if (argc <= 1) {
    return config;
  }

  config.files_amount = argc - 1;
  config.load_files = malloc((long unsigned)(argc - 1) * sizeof(char *));

  for (int i = 1; i < argc; i++) {
    config.load_files[i-1] = sc_alloc_strcpy(argv[i]);
  }

  return config;
}

void sc_free_config(sc_Config *config) {
  for (int i = 0; i < config->files_amount; i++) {
    free(config->load_files[i]);
  }

  free(config->load_files);
  config->load_files = NULL;
  config->files_amount = 0;
}

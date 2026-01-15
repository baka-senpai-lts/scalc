#ifndef CONFIG_H_
#define CONFIG_H_

typedef struct {
  char **load_files;
  int files_amount;
} sc_Config;

sc_Config sc_parse_args(int, char **);

void sc_free_config(sc_Config *);

#endif // CONFIG_H_

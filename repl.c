#include "repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "node.h"
#include "parser.h"
#include "printer.h"
#include "util.h"

// I know it sounds stupid, but I do NOT believe in unistd.h to define it
// consistently everywhere
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 0
#endif
#ifndef STDIN_FILENO
#define STDIN_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

// These are all escape sequences without \033
#define CHAR_UP "[A"
#define CHAR_DOWN "[B"
#define CHAR_LEFT "[D"
#define CHAR_RIGHT "[C"

#define ESCAPE_BUFSIZ 50

static struct termios sc_original_termios;

void sc_terminal_set_raw(void) {
  tcgetattr(STDIN_FILENO, &sc_original_termios);
  atexit(sc_terminal_set_cooked);

  struct termios raw = sc_original_termios;

  raw.c_lflag &= ~((unsigned int)ECHO | (unsigned int)ICANON);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void sc_terminal_set_cooked(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &sc_original_termios);
}

static void sc_print_ast(const char *prompt, const char *buffer) {
  fputs("\033[2K", stdout);

  fputs(prompt, stdout);

  sc_Node *result = sc_str_to_node(buffer, NULL);

  sc_print_node(result);
  fflush(stdout);

  sc_free_node_tree_children(result);
  free(result);
}

int sc_getline(const char *prompt, char *buffer, int size) {
  int c = 0;

  unsigned long prompt_len = sc_strlen(prompt);

  char escape_buffer[ESCAPE_BUFSIZ];
  int current_escape = 0;

  // Initial wipe
  sc_fill_seq_with_bytes(escape_buffer, '\0', ESCAPE_BUFSIZ);

  int cursor_position = 0;
  int written_len = 0;

  bool in_escape = false;

  sc_terminal_set_raw();

  fputs(prompt, stdout);
  fflush(stdout);

  while (read(STDIN_FILENO, &c, 1) == 1 && c != 10) {
    if (in_escape) {
      escape_buffer[current_escape] = (char)c;
      current_escape++;
      // TODO: all the checks

      if (sc_strcmp(CHAR_LEFT, escape_buffer)) {
        if (cursor_position > 0) {
          fputs("\033[1D", stdout);
          fflush(stdout);
          cursor_position--;
        }

        in_escape = false;
      } else if (sc_strcmp(CHAR_RIGHT, escape_buffer)) {
        if (cursor_position < written_len) {
          fputs("\033[1C", stdout);
          fflush(stdout);
          cursor_position++;
        }

        in_escape = false;
      } else if (sc_strcmp(CHAR_UP, escape_buffer)) {
        in_escape = false;
      } else if (sc_strcmp(CHAR_DOWN, escape_buffer)) {
        in_escape = false;
      }

      if (current_escape >= 2) {
        // In case I forgot something
        in_escape = false;
      }

      continue;
    }

    if (c == '\033') {
      in_escape = true;
      current_escape = 0;
      sc_fill_seq_with_bytes(escape_buffer, '\0', ESCAPE_BUFSIZ);
      continue;
    }

    // Backspace, I really forgot what the character is for it
    if (c == 127) {
      if (cursor_position == 0) {
        continue;
      }

      char *current = buffer + cursor_position;

      sc_memmove_n(current, current - 1,
                   (unsigned long int)written_len -
                       (unsigned long int)cursor_position + 1);

      cursor_position--;

      fputs("\033[1D", stdout);
      fputs(current - 1, stdout);
      fputc(' ', stdout);

      written_len--;

      printf("\033[%dD", written_len - cursor_position + 1);

      fputc('\n', stdout);
      sc_print_ast("~> ", buffer);
      printf("\033[1A\r\033[%dC", cursor_position + (int)prompt_len);

      fflush(stdout);

      continue;
    }

    if (written_len >= size) {
      continue;
    }

    if (cursor_position < written_len) {
      sc_memmove_n(buffer + cursor_position, buffer + cursor_position + 1,
                   (unsigned long int)written_len -
                       (unsigned long int)cursor_position);

      buffer[cursor_position] = (char)c;
      cursor_position++;
      written_len++;

      int saved_position = cursor_position;

      fputs(buffer + cursor_position - 1, stdout);
      printf("\033[%dD", written_len - saved_position);

      fputc('\n', stdout);
      sc_print_ast("~> ", buffer);
      printf("\033[1A\r\033[%dC", cursor_position + (int)prompt_len);

      fflush(stdout);

      continue;
    }

    buffer[cursor_position] = (char)c;
    cursor_position++;
    written_len++;

    fputc(c, stdout);

    fputc('\n', stdout);
    sc_print_ast("~> ", buffer);
    printf("\033[1A\r\033[%dC", cursor_position + (int)prompt_len);
    fflush(stdout);
  }
  sc_terminal_set_cooked();

  fputs("\n\033[2K", stdout);

  if (written_len == 0) {
    return REPL_EMPTY;
  }

  return REPL_OK;
}

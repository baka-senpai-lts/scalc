#ifndef FILE_READER_H_
#define FILE_READER_H_

typedef struct {
  char *ptr;
  unsigned long size;
} sc_FileMap;

#define SC_OK 0

#define SC_PERMISSION_DENIED 1
#define SC_FILE_DOES_NOT_EXIST 2

// These are for lseek failures
#define SC_FILE_NOT_VALID 3
#define SC_FILE_TOO_BIG 4

#define SC_MAPPING_ERROR 5

#define SC_UNKNOWN_ERROR 999

// Puts error code into passed pointer if not NULL,
// allocates a string and fills it with file contents,
// returns NULL in .ptr on error
sc_FileMap sc_map_text_file(const char *, int *);

void sc_unmap_file(sc_FileMap *);

const char *sc_file_error_as_string(int);

#endif // FILE_READER_H_

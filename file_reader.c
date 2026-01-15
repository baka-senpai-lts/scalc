#include "file_reader.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define SET_ERROR(x)                                                           \
  if (error != NULL) {                                                         \
	*error = x;                                                                \
  }

sc_FileMap sc_map_text_file(const char *filename, int *error) {
  sc_FileMap result = {.ptr = NULL, .size = 0};

  int fd = open(filename, O_RDONLY);

  if (fd == -1) {
    switch (errno) {
	case EACCES:
	  SET_ERROR(SC_PERMISSION_DENIED);
	  break;

	case ENOENT:
	  SET_ERROR(SC_FILE_DOES_NOT_EXIST);
	  break;

    default:
      SET_ERROR(SC_UNKNOWN_ERROR);
	  break;
    }

    return result;
  }

  off_t len = lseek(fd, 0, SEEK_END);

  if (len == -1) {
	switch (errno) {
	case EOVERFLOW:
	  SET_ERROR(SC_FILE_TOO_BIG);
	  break;

	case ESPIPE:
	  SET_ERROR(SC_FILE_NOT_VALID);
	  break;

	default:
	  SET_ERROR(SC_UNKNOWN_ERROR);
	  break;
	}

	close(fd);
	return result;
  }

  void *data = mmap(0, (size_t)len, PROT_READ, MAP_PRIVATE, fd, 0);

  if (data == MAP_FAILED) {
	SET_ERROR(SC_MAPPING_ERROR);
	close(fd);
	return result;
  }

  close(fd);

  result.ptr = data;
  result.size = (size_t)len;

  SET_ERROR(SC_OK);
  return result;
}

void sc_unmap_file(sc_FileMap *file) {
  if (file->ptr == NULL || file->size == 0) {
	return;
  }

  munmap(file->ptr, file->size);
  file->ptr = NULL;
  file->size = 0;
}

const char *sc_file_error_as_string(int error) {
  switch (error) {
  case SC_OK:
	return "OK";

  case SC_FILE_DOES_NOT_EXIST:
	return "File does not exist";

  case SC_FILE_NOT_VALID:
	return "File is not valid, it might be a pipe or socket";

  case SC_FILE_TOO_BIG:
	return "File is too big";

  case SC_PERMISSION_DENIED:
	return "Permission denied";

  case SC_MAPPING_ERROR:
	return "Memory mapping error";

  case SC_UNKNOWN_ERROR:
	return "Unknown error";

  default:
	return "Unspecified error";
  }
}

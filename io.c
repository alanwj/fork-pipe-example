#include "io_helper.h"

#include <stdlib.h>
#include <unistd.h>

// The read function may read only part of what is requested.
// This function repeatedly calls read until count bytes have been read.
// Returns -1 if an error or EOF was encountered.
ssize_t ReadFull(int fd, void* buf, size_t count) {
  size_t bytes_left = count;
  while (bytes_left != 0) {
    ssize_t sz = read(fd, buf, bytes_left);
    if (sz <= 0) {
      return -1;
    }

    buf += sz;
    bytes_left -= sz;
  }
  return count;
}

// The write function may write only part of what is requested.
// This function repeatedly calls write until count bytes have been written.
// Returns -1 if on error.
ssize_t WriteFull(int fd, const void* buf, size_t count) {
  size_t bytes_left = count;
  while (bytes_left != 0) {
    ssize_t sz = write(fd, buf, bytes_left);
    if (sz <= 0) {
      return -1;
    }

    buf += sz;
    bytes_left -= sz;
  }
  return count;
}

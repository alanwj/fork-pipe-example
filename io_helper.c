#include "io_helper.h"

#include <stdlib.h>
#include <unistd.h>

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

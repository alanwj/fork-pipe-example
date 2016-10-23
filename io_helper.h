#ifndef   IO_HELPER_
#define   IO_HELPER_

#include <unistd.h>

// The read function may read only part of what is requested.
// This function repeatedly calls read until count bytes have been read.
// Returns -1 if an error or EOF was encountered.
ssize_t ReadFull(int fd, void* buf, size_t count);

// The write function may write only part of what is requested.
// This function repeatedly calls write until count bytes have been written.
// Returns -1 if on error.
ssize_t WriteFull(int fd, const void* buf, size_t count);

#endif    // IO_HELPER_

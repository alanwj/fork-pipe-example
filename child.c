#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "io_helper.h"

// Reverses the provided string.
void Reverse(char* buf, size_t len) {
  for (size_t i = 0; i < len / 2; ++i) {
    char c = buf[i];
    buf[i] = buf[len - i - 1];
    buf[len - i - 1] = c;
  }
}

int main(void) {
  int in = fileno(stdin);
  int out = fileno(stdout);

  fprintf(stderr, "child : Starting\n");
  char buf[1024];
  do {
    // Read message length.
    size_t len;
    ssize_t sz = ReadFull(in, &len, sizeof(size_t));
    if (sz != sizeof(size_t)) {
      fprintf(stderr, "child : Failed to read message size.\n");
      return 1;
    }

    if (len > sizeof(buf)) {
      fprintf(stderr, "child : Message size too long.\n");
      return 1;
    }

    // Parent tells us to stop by sending 0.
    if (len == 0) {
      fprintf(stderr, "child : Read quit message\n");
      break;
    }

    // Read message data.
    sz = ReadFull(in, buf, len);
    if (sz != (ssize_t) len) {
      fprintf(stderr, "child : Failed to read message.\n");
      return 1;
    }

    fprintf(stderr, "child : Read message     : %.*s\n", (int) len, buf);

    // Reverse and send response.
    Reverse(buf, len);
    fprintf(stderr, "child : Sending response : %.*s\n", (int) len, buf);

    sz = WriteFull(out, buf, len);
    if (sz != (ssize_t) len) {
      fprintf(stderr, "child : Failed to write message.\n");
      return 1;
    }
  } while(1);

  fprintf(stderr, "child : Exiting\n");
  return 0;
}

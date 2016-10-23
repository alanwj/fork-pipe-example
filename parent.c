#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "io_helper.h"

const char* messages[] = {
  "This is a message!",
  "This is another message.",
  "This is a third message.",
};

int main(void) {
  fprintf(stderr, "parent: Starting\n");

  // Create pipe to communicate with stdin.
  int in[2];
  if (pipe(in) == -1) {
    perror("parent: Failed to create stdin pipe");
    return 1;
  }

  // Create pipe to communicate with stdout.
  int out[2];
  if (pipe(out) == -1) {
    perror("parent: Failed to create stdout pipe");
    return 1;
  }

  pid_t pid = fork();
  if (pid == 0) {
    // We are in the child process.

    // Child shouldn't write to stdin pipe.
    close(in[1]);

    // Child shouldn't read from stdout pipe.
    close(out[0]);

    // Redirect stdin and stdout for the child process.
    if (dup2(in[0], fileno(stdin)) == -1) {
      perror("parent: Failed to redirect stdin");
      return 1;
    }
    if (dup2(out[1], fileno(stdout)) == -1) {
      perror("parent: Failed to redirect stdout");
      return 1;
    }

    // Start the child executable.
    execlp("./child", "child", NULL);

    // We should never get here.
    perror("parent: Failed to start child");
    return 1;
  } else {
    // We are in the parent process.
    if (pid == -1) {
      perror("parent: Failed to create child");
      return 1;
    }

    // Parent shouldn't read from stdin pipe.
    close(in[0]);

    // Parent shouldn't write to stdout pipe.
    close(out[1]);

    size_t message_count = sizeof(messages) / sizeof(messages[0]);
    char buf[1024];
    for (size_t i = 0; i < message_count; ++i) {
      size_t len = strlen(messages[i]);

      // Write the length.
      fprintf(stderr, "parent: Writing length   : %d\n", (int)len);
      WriteFull(in[1], &len, sizeof(len));

      // Write the data.
      fprintf(stderr, "parent: Writing data     : %.*s\n", (int)len,
              messages[i]);
      WriteFull(in[1], messages[i], len);

      // Read the response.
      ReadFull(out[0], buf, len);
      fprintf(stderr, "parent: Read response    : %.*s\n", (int)len, buf);
    }

    // Tell child to quite.
    size_t len = 0;
    WriteFull(in[1], &len, sizeof(len));

    fprintf(stderr, "parent: Waiting for child process to end.\n");
    waitpid(pid, NULL, 0);
    fprintf(stderr, "parent: Exiting\n");
  }
}

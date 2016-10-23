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

// Starts a child process.
//
// to_child_fd and from_child_fd will be set to file descriptors that may be
// used to send data to or receive data from the child process.
//
// Returns the child pid on success, or -1 on failure.
pid_t StartChild(const char* file, int* to_child_fd, int* from_child_fd) {
  // Create pipe to connect to child's stdin.
  int in[2];
  if (pipe(in) == -1) {
    perror("parent: Failed to create stdin pipe");
    return -1;
  }

  // Create pipe to connect to child's stdout.
  int out[2];
  if (pipe(out) == -1) {
    perror("parent: Failed to create stdout pipe");
    return -1;
  }

  pid_t pid = fork();
  if (pid == 0) {
    // We are in the child process.

    // Child shouldn't write to its stdin.
    close(in[1]);

    // Child shouldn't read from its stdout.
    close(out[0]);

    // Redirect stdin and stdout for the child process.
    if (dup2(in[0], fileno(stdin)) == -1) {
      perror("child[pre-exec]: Failed to redirect stdin for child");
      return -1;
    }
    if (dup2(out[1], fileno(stdout)) == -1) {
      perror("child[pre-exec]: Failed to redirect stdout for child");
      return -1;
    }

    // Start the child executable.
    execlp(file, file, NULL);

    // We should never get here.
    perror("child[pre-exec]: Failed to start child");
    return -1;
  } else {
    // We are in the parent process.
    if (pid == -1) {
      perror("parent: Failed to create child");
      return -1;
    }

    // Parent shouldn't read from child's stdin.
    close(in[0]);

    // Parent shouldn't write to child's stdout.
    close(out[1]);

    *to_child_fd = in[1];
    *from_child_fd = out[0];
    return pid;
  }
}

int main(void) {
  fprintf(stderr, "parent: Starting\n");

  int to_child_fd;
  int from_child_fd;
  pid_t pid = StartChild("./child", &to_child_fd, &from_child_fd);
  if (pid == -1) {
    return 1;
  }

  size_t message_count = sizeof(messages) / sizeof(messages[0]);
  char buf[1024];
  for (size_t i = 0; i < message_count; ++i) {
    size_t len = strlen(messages[i]);

    // Write the length.
    fprintf(stderr, "parent: Writing length   : %d\n", (int)len);
    if (WriteFull(to_child_fd, &len, sizeof(len)) == -1) {
      fprintf(stderr, "parent: Failed to write length.\n");
      return 1;
    }

    // Write the data.
    fprintf(stderr, "parent: Writing message  : %.*s\n", (int)len, messages[i]);
    if (WriteFull(to_child_fd, messages[i], len) == -1) {
      fprintf(stderr, "parent: Failed to write message.\n");
      return 1;
    }

    // Read the response.
    if (ReadFull(from_child_fd, buf, len) == -1) {
      fprintf(stderr, "parent: Failed to read response.\n");
      return 1;
    }
    fprintf(stderr, "parent: Read response    : %.*s\n", (int)len, buf);
  }

  // Tell child to quite.
  size_t len = 0;
  WriteFull(to_child_fd, &len, sizeof(len));

  fprintf(stderr, "parent: Waiting for child process to end.\n");
  waitpid(pid, NULL, 0);
  fprintf(stderr, "parent: Exiting\n");
}

# fork-pipe-example
An example of using fork and pipe to communicate with subprocesses in C.

## To build and run

    make
    ./parent
    
After running `make` two binaries will be built: `parent` and `child`.

The `parent` binary, when run, will fork and start an instance of `child`.
Communication between the processes is handled via pipes connected to stdin and stdout of the `child` process.

## Communication protocol

The `parent` sends messages to the child in the form of a size followed by a character string of that size (max 1024).

The `child` responds with a reversed message of the same size (it does not write the size first in the response).

When the `parent` is done sending messages, it sends a size `0` which causes the `child` to terminate.

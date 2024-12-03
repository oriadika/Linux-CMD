#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipefd[2];  // Array to hold the file descriptors for the pipe
    int pid;      // Process ID for fork()
    char buffer[128]; // Buffer to hold the message

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
    // CHILD PROCESS
        close(pipefd[0]);  // Close the unused read end of the pipe

        const char *message = "hello";
        write(pipefd[1], message, strlen(message) + 1); // Write to the pipe
        close(pipefd[1]);  // Close the write end after writing
        exit(EXIT_SUCCESS);
    } 
    else {
    // PARENT PROCESS
        close(pipefd[1]);  // Close the unused write end of the pipe

        read(pipefd[0], buffer, sizeof(buffer)); // Read from the pipe
        printf("Parent received: %s\n", buffer); // Print the message
        close(pipefd[0]);  // Close the read end after reading
    }

    return 0;
}

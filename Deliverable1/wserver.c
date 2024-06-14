//Doruk Aydoğan
//13547373070
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "request.h"
#include "io_helper.h"

char default_root[] = ".";

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    int threads = 2;
    int buffer_size = 5;
    
    while ((c = getopt(argc, argv, "p:t:b:")) != -1) {
        switch (c) {
        case 'p':
            port = atoi(optarg);
            break;
        case 't':
            threads = atoi(optarg);
            break;
        case 'b':
            buffer_size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "usage: ./wserver [-p <portnum>] [-t threads] [-b buffers]\n");
            exit(1);
        }
    }

    printf("Server running on port: %d, threads: %d, buffer: %d\n", port, threads, buffer_size);

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);

        pid_t pid = fork(); // Create a new process

        if (pid < 0) {
            // Fork failed
            fprintf(stderr, "Fork failed\n");
            close(conn_fd);
        } else if (pid == 0) {
            // Child Process Step
            close(listen_fd); // Close the listening socket in the child
            request_handle(conn_fd); // The request is handled by this function
            close(conn_fd); // Close the connection socket in the child after handling the request
            exit(0); // End the child process
        } else {
            // Parent Process Step
            close(conn_fd); // Close the connection socket in the parent, it's no longer needed
            int status;
            waitpid(pid, &status, 0); // Wait for the child to finish
        }
    }
    return 0;
}

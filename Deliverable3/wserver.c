//Doruk Aydoğan
//13547373070
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include "request.h"
#include "io_helper.h"

char default_root[] = ".";

void* handle_request(void* arg) {
    int conn_fd = *((int*) arg);
    free(arg);
    request_handle(conn_fd);
    close(conn_fd);
    return NULL;
}

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
        int* conn_fd = malloc(sizeof(int));
        *conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
// ptthread's are used to solve the issue for task in deliverable 3!
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_request, conn_fd);
        pthread_detach(thread_id);
    }
    return 0;
}

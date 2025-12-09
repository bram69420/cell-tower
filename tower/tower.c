#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_PORT 5555
#define BUFSIZE 1024

void handle_client(int client_fd) {
    char buf[BUFSIZE];
    ssize_t r;
    while ((r = read(client_fd, buf, BUFSIZE-1)) > 0) {
        buf[r] = '\0';
        char *p = buf;
        while (*p && (*p == '\r' || *p == '\n')) p++;
        char msg[BUFSIZE]; strncpy(msg, buf, BUFSIZE); msg[BUFSIZE-1]=0;
        if (strstr(msg, "ATTACH_REQUEST")) {
            write(client_fd, "ATTACH_ACCEPT\n", 14);
        } else if (strstr(msg, "PING")) {
            write(client_fd, "PONG\n", 5);
        } else if (strstr(msg, "DETACH")) {
            write(client_fd, "DETACH_ACK\n", 11);
            break;
        } else {
            write(client_fd, "UNKNOWN_COMMAND\n", 16);
        }
    }
    close(client_fd);
}

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;
    if (argc >= 2) port = atoi(argv[1]);

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(srv); return 1;
    }
    if (listen(srv, 5) < 0) { perror("listen"); close(srv); return 1; }

    printf("Tower simulator listening on 0.0.0.0:%d\n", port);

    while (1) {
        int client = accept(srv, NULL, NULL);
        if (client < 0) { perror("accept"); continue; }
        printf("Client connected\n");
        pid_t pid = fork();
        if (pid == 0) {
            // child
            close(srv);
            handle_client(client);
            exit(0);
        } else if (pid > 0) {
            close(client);
        } else {
            perror("fork");
            close(client);
        }
    }

    close(srv);
    return 0;
}

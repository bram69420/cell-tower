#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in srv = {0};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    inet_pton(AF_INET, ip, &srv.sin_addr);

    if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect");
        return 1;
    }
    printf("Connected to tower %s:%d\n", ip, port);

    write(sock, "ATTACH_REQUEST\n", 15);
    char buf[BUFSIZE];
    int r = read(sock, buf, sizeof(buf)-1);
    if (r > 0) {
        buf[r]=0;
        printf("[phone] recv: %s", buf);
    }

    // simple keepalive PINGs
    for (int i=0;i<3;i++) {
        write(sock, "PING\n", 5);
        int s = read(sock, buf, sizeof(buf)-1);
        if (s>0){buf[s]=0; printf("[phone] recv: %s", buf);}
        sleep(1);
    }

    // detach
    write(sock, "DETACH\n", 7);
    int s = read(sock, buf, sizeof(buf)-1);
    if (s>0){buf[s]=0; printf("[phone] recv: %s", buf);}

    close(sock);
    return 0;
}

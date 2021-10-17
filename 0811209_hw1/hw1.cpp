#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    int                 listenfd, connfd;
    socklen_t           len;
    struct sockaddr_in  servaddr, clientaddr;
    char                buff[MAXLINE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(argc > 1 ? atoi(argv[1]) : 2233);

    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    listen(listenfd, SOMAXCONN);

    for ( ; ; ) {
        len = sizeof(servaddr);
        connfd = accept(listenfd, (SA *) &clientaddr, &len);
        printf("connection from %s, on port %d\n",
            inet_ntop(AF_INET, &clientaddr.sin_addr, buff, sizeof(buff)), 
            ntohs(clientaddr.sin_port));

        Close(connfd);

        close(connfd);
    }
        
    return 0;
}
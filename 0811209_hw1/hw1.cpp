#include <sys/socket.h>
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
    servaddr.sin_port = htons(argc > 0 ? argv[1] : 23);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, SOMAXCONN);

    for ( ; ; ) {
        len = sizeof(servaddr);
        connfd = Accept(listenfd, (SA *) &servaddr, len);
        printf("connection from %s, on port %d\n",
            Inet_ntop(AF_INET, &clientaddr.sinaddr, buff, sizeof(buff)), 
            ntohs(clientaddr.sin_port));

        Close(connfd);
    }
        
    return 0;
}
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>

#define SA struct sockaddr

const int MAXLINE = 1024;

void welcome_msg(int connfd) {
    std::string msg[3] = {
        "********************************\n",
        "** Welcome to the BBS server. **\n",
        "********************************\n"
    };

    for (int i = 0; i < 3; ++i) send(connfd, msg[i].c_str(), msg[i].size(), 0);
}

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

        welcome_msg(connfd);

        close(connfd);
    }
        
    return 0;
}
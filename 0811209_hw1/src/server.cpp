#include "server.h"
#include "command.cpp"

#include <iostream>

inline void ReadCommand(int fd, void *buf)
{
    write(fd, kShellTitle.c_str(), kShellTitle.size());

    // ! must clear buffer before read
    bzero(buf, kBufSize);
    read(fd, buf, kBufSize);
}

// parse command and do action
// return :
//      true: close connection
//      false: keep reading
bool ParseCommand(std::stringstream &ss, int fd, std::vector<std::string> &cmd)
{
    std::string str;

    while (std::getline(ss, str, ' '))
        cmd.push_back(rtrim(str));

    auto func = kCommandFuntions.find(cmd.at(0));
    if (func != kCommandFuntions.end())
    {
        (*func->second)(cmd, fd);

        if (func->first == "exit")
            return true;
    }

    return false;
}

void HandleConnection(int connfd)
{
    char buf[kBufSize];
    std::stringstream ss;
    std::vector<std::string> cmd;
    cmd.reserve(3);

    WelcomeMsg(connfd);

    while (true)
    {
        cmd.clear();

        // ! clear ss before read
        ss.str("");
        ss.clear();

        ReadCommand(connfd, buf);
        ss << buf;

        if (ParseCommand(ss, connfd, cmd))
            break;
    }
}

// start a server on given address and port
//
// Example :
//      server_start(htonl(INADDR_ANY), htons(2233);
void ServerStart(in_addr_t addr, in_port_t port)
{
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, clientaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = addr;
    servaddr.sin_port = port;

    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    listen(listenfd, SOMAXCONN);

    for (;;)
    {
        connfd = accept(listenfd, (SA *)&clientaddr, &len);
        printf("connection on port %d\n", ntohs(clientaddr.sin_port));

        HandleConnection(connfd);

        close(connfd);
    }
}
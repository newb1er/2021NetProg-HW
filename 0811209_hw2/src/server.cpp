#include "server.h"

#include <iostream>

inline void PrintShellTitle(int fd)
{
    write(fd, kShellTitle.c_str(), kShellTitle.size());
}

inline void ReadCommand(int fd, void *buf)
{
    // ! must clear buffer before read
    bzero(buf, kBufSize);
    read(fd, buf, kBufSize);
}

// parse command and do action
// return :
//      true: close connection
//      false: keep reading
bool ParseCommand(std::stringstream &ss, Session *s, std::list<std::string> &cmd)
{
    Parser(ss, cmd);

    if (cmd.empty())
        return false;

    auto func = kCommandFuntions.find(cmd.front());
    if (func != kCommandFuntions.end())
    {
        (*func->second)(cmd, s);

        if (func->first == "exit")
            return true;
    }

    return false;
}

// start a server on given address and port
//
// Example :
//      Server s(htonl(INADDR_ANY), htons(2233);
//      s.setup();
//      s.start();

/**
 * @brief Construct a new Server:: Server object
 * 
 * @param addr server listening address
 * @param port server listening port
 */
Server::Server(in_addr_t addr, in_port_t port)
{
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = addr;
    servaddr.sin_port = port;

    maxconn = -1;
}

/**
 * @brief setup descriptors, socket options, etc.
 */
void Server::setup()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));

    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for (int i = 0; i < FD_SETSIZE; ++i)
        client[i] = nullptr;
}

/**
 * @brief start the server
 */
void Server::start()
{
    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    listen(listenfd, SOMAXCONN);

    for (;;)
    {
        int nready;
        Select(nready);

        if (nready == -1 && errno == EINTR)
            continue;

        IncommingReq(nready);
        ConnectedReq(nready);
    }
}

/**
 * @brief setup select bits and select
 * 
 * @param nready number of descriptors that are ready to I/O
 */
void Server::Select(int &nready)
{
    rset = allset;
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
}

/**
 * @brief handle incomming requests, and accept connections
 * 
 * @param nready number of descriptors that are ready to I/O
 */
void Server::IncommingReq(int &nready)
{
    if (nready == 0)
        return;
    if (FD_ISSET(listenfd, &rset))
    {
        int i;
        int connfd;
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);

        connfd = accept(listenfd, (SA *)&cliaddr, &len);

        for (i = 0; i < FD_SETSIZE; ++i)
        {
            if (client[i] == nullptr)
            {
                client[i] = new Session(connfd);
                break;
            }
        }
        if (i == FD_SETSIZE)
            std::cerr << "too many clients\n";

        FD_SET(connfd, &allset);

        WelcomeMsg(connfd);
        PrintShellTitle(connfd);

        maxfd = std::max(maxfd, connfd);
        maxconn = std::max(maxconn, i);
        --nready;
    }
}

/**
 * @brief handle requests from connected socket
 * 
 * @param nready number of descriptors that are ready to I/O
 */
void Server::ConnectedReq(int &nready)
{
    if (nready <= 0)
        return;

    Session *s;
    char buf[kBufSize];
    std::stringstream ss;
    std::list<std::string> cmd;

    for (int i = 0; i <= maxconn; ++i)
    {
        if ((s = client[i]) == nullptr)
            continue;

        if (FD_ISSET(s->fd, &rset))
        {
            bzero(buf, kBufSize);

            if ((read(s->fd, buf, kBufSize)) == 0)
            {
                CloseConnection(s->fd, i);
            }
            else
            {
                // ! clear ss before read
                ss.str("");
                ss.clear();

                ss << buf;

                if (ParseCommand(ss, s, cmd))
                    CloseConnection(s->fd, i);

                PrintShellTitle(s->fd);
            }

            if (--nready <= 0)
                break;
        }
    }
}

/**
 * @brief close existed socket connection
 * 
 * @param sockfd the socket descriptor to close
 * @param connidx index in "client" array
 */
void Server::CloseConnection(int &sockfd, int &connidx)
{
    FD_CLR(sockfd, &allset);
    client[connidx] = nullptr;
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}
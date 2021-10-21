#ifndef VECTOR_HEADER
#define VECTOR_HEADER

#include <vector>
#endif

#ifndef STRING_HEADER
#define STRING_HEADER

#include <cstring>
#include <string>
#include <sstream>
#endif

#ifndef SOCKET_HEADER
#define SOCKET_HEADER

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#define SA struct sockaddr

const int kBufSize = 1024;
const std::string kShellTitle = "% ";

inline void WelcomeMsg(int connfd)
{
    const std::string msg[3] = {
        "********************************\n",
        "** Welcome to the BBS server. **\n",
        "********************************\n"};

    for (int i = 0; i < 3; ++i)
        send(connfd, msg[i].c_str(), msg[i].size(), 0);
}

inline std::string &rtrim(std::string &s)
{
    if (s.empty())
        return s;

    s.erase(s.find_last_not_of(" \n\r") + 1);
    return s;
}
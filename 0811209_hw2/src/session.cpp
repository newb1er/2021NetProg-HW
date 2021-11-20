#include "stdlib.h"

namespace User
{
    class UserInfo
    {
    public:
        std::string name;
        std::string passwd;
        std::map<std::string, std::queue<std::string>> mailbox;

        UserInfo(std::string name, std::string passwd) : name(name), passwd(passwd) {}
    };

    std::map<std::string, UserInfo> users;
};

class Session
{
public:
    int fd;
    FILE *fp;
    bool isLogin = false;
    User::UserInfo *user = nullptr;

    Session(int fd) : fd(fd)
    {
        if ((fp = fdopen(fd, "r+")) < 0)
            std::cerr << "fdopen error:\nerrno: " << errno << '\n';

        setvbuf(fp, NULL, _IONBF, 0);
    }
};
#include "command.h"

namespace CurrentUser
{
    bool isLogin = false;
    std::string name = "";
};

namespace User
{
    class UserInfo
    {
    public:
        std::string passwd;
        std::forward_list<std::string> mailbox;

        UserInfo(std::string);

        std::forward_list<std::string> receiveMail()
        {
            mailbox.sort();
            return mailbox;
        }
    };

    std::map<std::string, UserInfo> users;
};

User::UserInfo::UserInfo(std::string passwd) : passwd(passwd) {}

void Register(std::vector<std::string> &arg_str, int fd)
{
    if (arg_str.size() < 3)
    {
        write(fd, ErrorMsg::kRegUsageError.c_str(), ErrorMsg::kRegUsageError.size());
        return;
    }

    // if user exists
    if (User::users.count(arg_str.at(1)))
        write(fd, ErrorMsg::kRegDupUserError.c_str(), ErrorMsg::kRegDupUserError.size());
    else
    {
        const std::string username = arg_str.at(1);
        const std::string passwd = arg_str.at(2);

        User::users.emplace(username, User::UserInfo(passwd));

        write(fd, PromptMsg::kRegSuccess.c_str(), PromptMsg::kRegSuccess.size());
    }
}

void Login(std::vector<std::string> &arg_str, int fd)
{
    if (arg_str.size() < 3)
    {
        write(fd, ErrorMsg::kLoginUsageError.c_str(), ErrorMsg::kLoginUsageError.size());
        return;
    }

    if (CurrentUser::isLogin)
    {
        write(fd, ErrorMsg::kNoLogoutError.c_str(), ErrorMsg::kNoLogoutError.size());
        return;
    }

    const std::string name = arg_str.at(1);
    const std::string passwd = arg_str.at(2);

    auto user = User::users.find(name);

    // if user not exist or password incorrect
    if (user == User::users.end() || user->second.passwd != passwd)
    {
        write(fd, ErrorMsg::kLoginFailError.c_str(), ErrorMsg::kLoginFailError.size());
        return;
    }
    else
    {
        CurrentUser::isLogin = true;
        CurrentUser::name = name;

        const std::string msg = PromptMsg::kLoginSuccess + name + ".\n";

        write(fd, msg.c_str(), msg.size());
    }
}

void Logout(std::vector<std::string> &arg_str, int fd)
{
    if (!CurrentUser::isLogin)
    {
        write(fd, ErrorMsg::kNoLoginError.c_str(), ErrorMsg::kNoLoginError.size());
        return;
    }
    else
    {
        const std::string msg = PromptMsg::kLogoutSuccess + CurrentUser::name + ".\n";
        CurrentUser::isLogin = false;

        write(fd, msg.c_str(), msg.size());
    }
}

void Whoami(std::vector<std::string> &arg_str, int fd)
{
    if (!CurrentUser::isLogin)
    {
        write(fd, ErrorMsg::kNoLoginError.c_str(), ErrorMsg::kNoLoginError.size());
        return;
    }

    const std::string msg = CurrentUser::name + '\n';
    write(fd, msg.c_str(), msg.size());
}

void ListUser(std::vector<std::string> &arg_str, int fd)
{
    std::stringstream ss;

    for (auto it : User::users)
    {
        ss << it.first << '\n';
    }

    write(fd, ss.str().c_str(), ss.str().size());
}

void Exit(std::vector<std::string> &arg_str, int fd)
{
    if (CurrentUser::isLogin == true)
    {
        std::stringstream ss;
        ss << "Bye, " << CurrentUser::name << ".\n";
        write(fd, ss.str().c_str(), ss.str().size());
    }
    return;
}

void Send(std::vector<std::string> &arg_str, int fd)
{
    return;
}

void ListMsg(std::vector<std::string> &arg_str, int fd)
{
    return;
}

void Receive(std::vector<std::string> &arg_str, int fd)
{
    return;
}
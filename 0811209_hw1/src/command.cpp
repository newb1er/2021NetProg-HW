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
        std::map<std::string, std::queue<std::string>> mailbox;

        UserInfo(std::string);

        std::string receiveMail(std::string name)
        {
            auto it = mailbox.find(name);
            if (it == mailbox.end())
            {
                return "";
            }
            else
            {
                std::string msg = (it->second).front();
                (it->second).pop();

                if ((it->second).empty())
                {
                    mailbox.erase(it);
                }
                return msg;
            }
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
    if (arg_str.size() < 3)
    {
        write(fd, ErrorMsg::kSendUsageError.c_str(), ErrorMsg::kSendUsageError.size());
        return;
    }

    // if not logged in
    if (!CurrentUser::isLogin)
    {
        write(fd, ErrorMsg::kNoLoginError.c_str(), ErrorMsg::kNoLoginError.size());
        return;
    }

    auto user = User::users.find(arg_str.at(1));

    // if user doesn't exist
    if (user == User::users.end())
        write(fd, ErrorMsg::kNoUserError.c_str(), ErrorMsg::kNoUserError.size());
    else
    {
        if ((user->second).mailbox.find(CurrentUser::name) == (user->second).mailbox.end())
        {
            (user->second).mailbox.emplace(CurrentUser::name, std::queue<std::string>());
        }

        ((user->second).mailbox.find(CurrentUser::name)->second).push(arg_str.at(2));
    }
}

void ListMsg(std::vector<std::string> &arg_str, int fd)
{
    // if not logged in
    if (!CurrentUser::isLogin)
    {
        write(fd, ErrorMsg::kNoLoginError.c_str(), ErrorMsg::kNoLoginError.size());
        return;
    }

    auto user = User::users.find(CurrentUser::name);

    // if mailbox is empty, print prompt
    if ((user->second).mailbox.empty())
    {
        write(fd, PromptMsg::kEmptyMailBox.c_str(), PromptMsg::kEmptyMailBox.size());
        return;
    }

    for (auto m : (user->second).mailbox)
    {
        std::stringstream ss;
        ss << m.second.size() << " message from " << m.first << ".\n";
        
        write(fd, ss.str().c_str(), ss.str().size());
    }
}

void Receive(std::vector<std::string> &arg_str, int fd)
{
    if (arg_str.size() < 2)
    {
        write(fd, ErrorMsg::kReceiveUsageError.c_str(), ErrorMsg::kReceiveUsageError.size());
        return;
    }

    // if not logged in
    if (!CurrentUser::isLogin)
    {
        write(fd, ErrorMsg::kNoLoginError.c_str(), ErrorMsg::kNoLoginError.size());
        return;
    }

    // if user doesn't exist
    if (!User::users.count(arg_str.at(1)))
        write(fd, ErrorMsg::kNoUserError.c_str(), ErrorMsg::kNoUserError.size());
    else
    {
        auto user = User::users.find(CurrentUser::name);
        std::string msg = (user->second).receiveMail(arg_str.at(1));
        msg += '\n';

        write(fd, msg.c_str(), msg.size());
    }
}
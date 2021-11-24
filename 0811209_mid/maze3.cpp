#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

const char *name = "maze3";
const size_t kBufSize = 10000;

struct Step
{
    std::vector<bool> visited;
    int move;
    Step *prev;

    Step(Step *prev) : prev(prev)
    {
        visited = std::vector<bool>(4, false);
        move = -1;
    }
};

Step *find_steps(std::array<int, 4> &surrounding, Step *cur, FILE *fp)
{
    Step *res = new Step(cur);

    // left
    if (surrounding.at(0) == 0 && cur->visited.at(0) == false &&
        (cur->prev == nullptr || cur->prev->move != 3))
    {
        fputs("A\n", fp);
        fputs("A\n", stdout);
        cur->move = 0;
        cur->visited.at(0) = true;
    }

    // down
    else if (surrounding.at(1) == 0 && cur->visited.at(1) == false &&
             (cur->prev == nullptr || cur->prev->move != 2))
    {
        fputs("S\n", fp);
        fputs("S\n", stdout);
        cur->move = 1;
        cur->visited.at(1) = true;
    }

    // up
    else if (surrounding.at(2) == 0 && cur->visited.at(2) == false &&
             (cur->prev == nullptr || cur->prev->move != 1))
    {
        fputs("W\n", fp);
        fputs("W\n", stdout);
        cur->move = 2;
        cur->visited.at(2) = true;
    }

    // right
    else if (surrounding.at(3) == 0 && cur->visited.at(3) == false &&
             (cur->prev == nullptr || cur->prev->move != 0))
    {
        fputs("D\n", fp);
        fputs("D\n", stdout);
        cur->move = 3;
        cur->visited.at(3) = true;
    }

    else
    {
        delete res;
        res = cur->prev;
        switch (cur->prev->move)
        {
        case 0:
            fputs("D\n", fp);
            fputs("D\n", stdout);
            break;
        case 1:
            fputs("W\n", fp);
            fputs("W\n", stdout);
            break;
        case 2:
            fputs("S\n", fp);
            fputs("S\n", stdout);
            break;
        case 3:
            fputs("A\n", fp);
            fputs("A\n", stdout);
            break;
        default:
            std::cerr << "wait.\n";
            exit(1);
        }
    }

    return res;
}

void solve(FILE *fp, char *buf)
{
    bool start = false;
    std::array<int, 4> surrounding;
    Step *cur = new Step(nullptr);

    while (fgets(buf, kBufSize, fp) != NULL)
    {
        fputs(buf, stdout);
        if (!start && buf[0] == ' ' && buf[5] == ':')
            start = true;

        if (start)
        {
            surrounding.fill({0});
            for (int i = 0; i < 2; ++i)
            {
                fgets(buf, kBufSize, fp);
                fputs(buf, stdout);
            }

            // upper
            surrounding.at(2) = buf[12] == '#' ? -1 : 0;

            fgets(buf, kBufSize, fp);
            fputs(buf, stdout);

            // left and right
            surrounding.at(0) = buf[11] == '#' ? -1 : 0;
            surrounding.at(3) = buf[13] == '#' ? -1 : 0;

            fgets(buf, kBufSize, fp);
            fputs(buf, stdout);

            // lower
            surrounding.at(1) = buf[12] == '#' ? -1 : 0;

            for (int i = 0; i < 4; ++i)
            {
                fgets(buf, kBufSize, fp);
                fputs(buf, stdout);
            }

            start = false;

            if (buf[0] == 'S')
                break;

            cur = find_steps(surrounding, cur, fp);
        }
    }

    if (fgets(buf, kBufSize, fp) == NULL)
    {
        std::cerr << "server disconnected.\n";
        return;
    }
    fputs(buf, stdout);

    while (fgets(buf, kBufSize, fp) != NULL)
        fputs(buf, stdout);
}

int main()
{
    int fd;
    socklen_t len;
    struct sockaddr_in servaddr;
    struct hostent *host;
    FILE *fp;
    char buf[kBufSize];

    host = gethostbyname(name);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    memcpy(&servaddr.sin_addr, host->h_addr, host->h_length);
    servaddr.sin_port = htons(10100);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        std::cerr << hstrerror(errno) << '\n';
        return 1;
    }

    if ((fp = fdopen(fd, "r+")) == NULL)
    {
        std::cerr << "fdopen error: " << errno << '\n';
        return 1;
    }

    setvbuf(fp, NULL, _IONBF, 0);

    solve(fp, buf);

    return 0;
}
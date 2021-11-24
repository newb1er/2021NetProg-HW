#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

const char *name = "maze2";
const size_t kBufSize = 10000;

struct Step
{
    int x;
    int y;
    std::string move;

    Step(int x, int y, std::string move) : x(x), y(y), move(move) {}
};

std::string find_steps(int s_x, int s_y, int e_x, int e_y,
                       std::array<std::array<int, 79>, 21> &map)
{
    int x, y;
    std::stack<Step> steps;
    std::string res;
    steps.push(Step(e_x, e_y, ""));

    while (!steps.empty())
    {
        x = steps.top().x;
        y = steps.top().y;

        if (x == s_x && y == s_y)
            break;

        map[y][x] = 2;

        if (x > 0 && map[y][x - 1] == 0)
            steps.push(Step(x - 1, y, "D"));
        else if (y > 0 && map[y - 1][x] == 0)
            steps.push(Step(x, y - 1, "S"));
        else if (x < 78 && map[y][x + 1] == 0)
            steps.push(Step(x + 1, y, "A"));
        else if (y < 20 && map[y + 1][x] == 0)
            steps.push(Step(x, y + 1, "W"));
        else
        {
            steps.pop();
            map[y][x] = 1;
        }
    }

    while (!steps.empty())
    {
        res += steps.top().move;
        steps.pop();
    }
    res.append("\n");

    return res;
}

void solve(FILE *fp, char *buf)
{
    bool start = false;
    std::array<std::array<int, 79>, 21> map;
    int s_x, s_y, e_x, e_y;
    int cur_y = 0;
    std::stack<Step> steps;
    std::string ans;

    while (fgets(buf, kBufSize, fp) != NULL)
    {
        fputs(buf, stdout);
        if (!start && buf[0] == '#')
            start = true;

        // end input
        if (start && buf[0] == '\n')
            break;

        if (start)
        {
            for (int x = 0; x < 79; ++x)
            {
                if (buf[x] == '*')
                {
                    s_x = x;
                    s_y = cur_y;
                }
                else if (x != 0 && buf[x] == 'E')
                {
                    e_x = x;
                    e_y = cur_y;
                }

                if (buf[x] == '#')
                    map[cur_y][x] = 1;
                else
                    map[cur_y][x] = 0;
            }

            ++cur_y;
        }

        // bzero(buf, kBufSize);
    }

    if (fgets(buf, kBufSize, fp) == NULL)
    {
        std::cerr << "server disconnected.\n";
        return;
    }
    fputs(buf, stdout);

    ans = find_steps(s_x, s_y, e_x, e_y, map);
    fputs(ans.c_str(), fp);
    fputs(ans.c_str(), stdout);

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
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

const char *name = "maze1";
const size_t kBufSize = 1024;

void steps(int s_x, int s_y, int e_x, int e_y, char *buf)
{
    char *ptr = buf;
    int d_x = e_x - s_x;
    int d_y = e_y - s_y;

    while (d_x)
    {
        if (d_x < 0)
        {
            *(ptr++) = 'A';
            ++d_x;
        }
        else
        {
            *(ptr++) = 'D';
            --d_x;
        }
    }

    while (d_y)
    {
        if (d_y < 0)
        {
            *(ptr++) = 'W';
            ++d_y;
        }
        else
        {
            *(ptr++) = 'S';
            --d_y;
        }
    }

    *ptr = '\n';
}

void solve(FILE *fp, char *buf)
{
    bool start = false;
    int s_x, s_y, e_x, e_y;
    int cur_y = 0;
    std::array<std::array<int, 7>, 11> map;

    while (fgets(buf, kBufSize, fp) != NULL)
    {
        if (!start && buf[0] == '#')
            start = true;

        if (start)
        {
            for (int x = 0; x < 11; ++x)
            {
                if (buf[x] == '*')
                {
                    s_x = x;
                    s_y = cur_y;
                }
                if (buf[x] == 'E')
                {
                    e_x = x;
                    e_y = cur_y;
                }
            }
            ++cur_y;
        }

        fputs(buf, stdout);

        if (buf[0] == '[')
            break;
    }

    bzero(buf, kBufSize);
    steps(s_x, s_y, e_x, e_y, buf);

    fputs(buf, stdout);
    fputs(buf, fp);

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
#include "src/server.cpp"

int main(int argc, char **argv)
{
    ServerStart(htonl(INADDR_ANY), htons(argc > 1 ? atoi(argv[1]) : 2233));

    return 0;
}
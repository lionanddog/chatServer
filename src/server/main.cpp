#include <signal.h>
#include <iostream>
#include "ChatServer.h"
#include "ChatService.h"

void resetHandler(int)
{
    ChatService::getInstance()->dealServerCloseException();
    exit(0);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "输入参数不对,请以形式程序 ip port" << std::endl;
    }
    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress address(argv[1], atoi(argv[2]));
    ChatServer server(&loop, address, "ChatServer");
    server.start();
    loop.loop();
    return 0;
}
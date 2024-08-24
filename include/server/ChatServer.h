/*
    聊天服务类
*/
#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

using namespace muduo::net;
using namespace muduo;


class ChatServer
{
public:
    // 构造函数
    ChatServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& nameArg);
    // 启动服务
    void start();
private:
    // 处理连接和断开
    void onConnect(const TcpConnectionPtr& conn);
    // 处理消息的接受发送
    void onMsg(const TcpConnectionPtr& conn, Buffer* buff, Timestamp time);
private:
    TcpServer m_server; //< TCP服务连接
    EventLoop* m_loop;    //< 事件循环
};

#endif
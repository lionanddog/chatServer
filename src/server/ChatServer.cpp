#include "ChatServer.h"
#include "ChatService.h"


ChatServer::ChatServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& nameArg):m_server(loop, listenAddr, nameArg), m_loop(loop)
{
    // 设置连接和断开时的回调
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnect, this, std::placeholders::_1));
    // 设置收到消息是的回调
    m_server.setMessageCallback(std::bind(&ChatServer::onMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置事件循环线程数
    // 一个IO线程，三个子subReactor
    m_server.setThreadNum(4);
}

void ChatServer::start()
{
    m_server.start();
}

void ChatServer::onConnect(const TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        ChatService::getInstance()->dealCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMsg(const TcpConnectionPtr& conn, Buffer* buff,Timestamp time)
{
    std::string strInfo = buff->retrieveAllAsString();
    json js = json::parse(strInfo);
    int nMsgId = js["msgid"].get<int>();
    // 获取处理handler
    auto handler = ChatService::getInstance()->getHandler(nMsgId);
    handler(conn, js, time);
}
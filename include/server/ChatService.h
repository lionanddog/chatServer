/*
*   单例模式实现业务层功能,通过handler和网络层解耦
*/
#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include <functional>
#include <muduo/net/TcpServer.h>
#include <unordered_map>
#include <mutex>
#include "json.hpp"
#include "UserModel.h"
#include "OfflineMessageModel.h"
#include "GroupModel.h"
#include "FriendModel.h"
#include "redis.hpp"

using json = nlohmann::json;
using namespace muduo::net;
using namespace muduo;
using Handler = std::function<void(const TcpConnectionPtr& conn, json js, Timestamp time)>;

class ChatService
{
public:
    static ChatService* getInstance();
    Handler getHandler(int msgid);
    // 处理客户端异常关闭
    void dealCloseException(const TcpConnectionPtr& conn);
    // 处理服务器异常关闭
    void dealServerCloseException();
private:
    ChatService();
    // 登录处理
    void _loginHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 注册处理
    void _onRegHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 点对点通信处理
    void _onOneChatHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 创建群组
    void _onAddGroupHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 给群组添加成员
    void _onAddUserToGroupHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 给群组发送消息
    void _onChatToGroupHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 添加好友
    void _onAddFriendHandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 退出
    void _onLoginOutandler(const TcpConnectionPtr& conn, json js, Timestamp time);
    // 处理redis推送
    void _onReplyRedis(int id, string messages);
private:
    std::unordered_map<int, Handler> m_mapMsgType2Handler;
    std::unordered_map<int, TcpConnectionPtr> m_mapID2Conn;
    std::mutex m_mutex;
    UserModel m_userModel;
    OfflineMessageModel m_offlineModel;
    GroupModel m_groupModel;
    FriendModel m_friendModel;
    Redis redis;
};

#endif
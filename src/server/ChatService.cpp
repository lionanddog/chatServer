#include <muduo/base/Logging.h>
#include <vector>
#include "ChatService.h"
#include "Public.h"

ChatService* ChatService::getInstance()
{
    static ChatService chatService;
    return &chatService;
}

Handler ChatService::getHandler(int msgid)
{
    if (m_mapMsgType2Handler.find(msgid) == m_mapMsgType2Handler.end())
    {
        return [=](const TcpConnectionPtr& conn, json js, Timestamp time){
            LOG_INFO << "" ;
        };
    }
    return m_mapMsgType2Handler[msgid];
}

ChatService::ChatService()
{
    m_mapMsgType2Handler.insert({LOG_MSG, std::bind(&ChatService::_loginHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({REG_MSG, std::bind(&ChatService::_onRegHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({ONE_CHAT_MSG, std::bind(&ChatService::_onOneChatHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({ADD_GROUP_MSG, std::bind(&ChatService::_onAddGroupHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({ADD_User_MSG, std::bind(&ChatService::_onAddUserToGroupHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({Chat_Group_MSG, std::bind(&ChatService::_onChatToGroupHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({ADD_Friend_MSG, std::bind(&ChatService::_onAddFriendHandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    m_mapMsgType2Handler.insert({LoginOut, std::bind(&ChatService::_onLoginOutandler, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3)});
    if (redis.connect())
    {
        redis.init_notify_handler(std::bind(&ChatService::_onReplyRedis, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void ChatService::dealCloseException(const TcpConnectionPtr& conn)
{
    std::lock_guard guard(m_mutex);
    User user;
    for(auto it = m_mapID2Conn.begin(); it != m_mapID2Conn.end(); it++)
    {
        if (it->second == conn)
        {
            m_mapID2Conn.erase(it->first);
            redis.unsubscribe(it->first);
            user.setId(it->first);
            break;
        }
    }
    user.setState("offline");
    m_userModel.update(user);
}

void ChatService::_onLoginOutandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    std::lock_guard guard(m_mutex);
    int userid = js["id"].get<int>();
    User user;
    auto it = m_mapID2Conn.find(userid);
    if (it != m_mapID2Conn.end())
    {
        m_mapID2Conn.erase(it->first);
        redis.unsubscribe(it->first);
    }
    user.setId(userid);
    user.setState("offline");
    m_userModel.update(user);
}

void ChatService::dealServerCloseException()
{
    m_userModel.reserOffline();
}

void ChatService::_loginHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    int id = js["id"];
    std::string pwd = js["password"];
    User user = m_userModel.query(id);
    json response;
    if (user.getId() != -1 && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            response["msgid"] = LOG_MSG_ACK;
            response["errno"] = 1;
            response["errMsg"] = "用户已登录";
        }
        else
        {
            user.setState("online");
            if (m_userModel.update(user))
            {
                {
                    std::lock_guard guard(m_mutex);
                    m_mapID2Conn.insert({user.getId(), conn});
                }
                redis.subscribe(user.getId());
                std::vector<std::string> vec = m_offlineModel.query(user.getId());
                if (!vec.empty())
                {
                    response["offlineMessage"] = vec;
                    m_offlineModel.remove(user.getId());
                }
                std::vector<Group> vecGroup = m_groupModel.query(user.getId());
                std::vector<User> vecUser = m_friendModel.query(user.getId());
                std::vector<std::string> vecInfo;
                if (!vecGroup.empty())
                {
                    for (Group& group : vecGroup)
                    {
                        json js;
                        std::vector<std::string> vecUser;
                        js["groupId"] = group.getId();
                        js["groupName"] = group.getName();
                        js["groupDesc"] = group.getDesc();
                        for (GroupUser& user : group.getVecGroupUser())
                        {
                            json js;
                            js["userId"] = user.getId();
                            js["userName"] = user.getName();
                            js["userState"] = user.getState();
                            js["userRole"] = user.getRole();
                            vecUser.push_back(js.dump());
                        }
                        js["users"] = vecUser;
                        vecInfo.push_back(js.dump());
                    }
                    response["groups"] = vecInfo;
                }
                
                if (!vecUser.empty())
                {
                    std::vector<std::string> vecUserInfo;
                    for (User& user : vecUser)
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        vecUserInfo.push_back(js.dump());
                    }
                    response["friends"] = vecUserInfo;
                }
                response["msgid"] = LOG_MSG_ACK;
                response["errno"] = 0;
                response["name"] = user.getName();
                response["id"] = id;
                response["password"] = pwd;
            }
            else
            {
                response["msgid"] = LOG_MSG_ACK;
                response["errno"] = 1;
                response["errMsg"] = "用户状态更新失败";
            }
        }
    }
    else
    {
        response["msgid"] = LOG_MSG_ACK;
        response["errno"] = 1;
        response["errMsg"] = "输入的用户账号或密码错误";
    }
    conn->send(response.dump());
}

void ChatService::_onRegHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    std::string strName = js["name"];
    std::string strPwd = js["password"];

    User user;
    user.setName(strName);
    user.setPwd(strPwd);
    user.setState("offline");

    bool bState = m_userModel.insert(user);
    json response;
    // 注册成功
    if (bState)
    {
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getId();
        response["errno"] = 0;
    }
    else // 注册失败
    {
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["errMsg"] = "注册失败";
    }
    conn->send(response.dump());
}

void ChatService::_onOneChatHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    int toid = js["toid"];
    std::string name = js["fromname"];

    std::lock_guard guard(m_mutex);
    auto it = m_mapID2Conn.find(toid);
    if (it != m_mapID2Conn.end())
    {
        it->second->send(js.dump());
        return;
    }
    User user = m_userModel.query(toid);
    if (user.getState() == "online")
    {
        redis.publish(toid, js.dump());
        return;
    }
    m_offlineModel.insert(toid, js.dump());
}

void ChatService::_onAddGroupHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    std::string strName = js["groupName"];
    std::string strDesc = js["groupDesc"];
    int userid = js["id"].get<int>();
    Group group;
    group.setId(-1);
    group.setName(strName);
    group.setDesc(strDesc);
    m_groupModel.createGroup(group);
    if (group.getId() != -1)
    {
        m_groupModel.addGroup(group.getId(), userid, "creator");
    }
}

void ChatService::_onAddUserToGroupHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    int id = js["useid"].get<int>();
    int groupid = js["groupid"].get<int>();
    m_groupModel.addGroup(groupid, id, "normal");
}

void ChatService::_onChatToGroupHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    int id = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<GroupUser> vec = m_groupModel.queryByGroupID(groupid, id);
    for(GroupUser& groupUser : vec)
    {
        std::lock_guard guard(m_mutex);
        auto it = m_mapID2Conn.find(groupUser.getId());
        if (it == m_mapID2Conn.end())
        {
            User user = m_userModel.query(groupUser.getId());
            if (user.getState() == "online")
            {
                redis.publish(groupUser.getId(), js.dump());
                return;
            }
            m_offlineModel.insert(groupUser.getId(), js.dump());
        }
        else
        {
            it->second->send(js.dump());
        }
    }
}

void ChatService::_onAddFriendHandler(const TcpConnectionPtr& conn, json js, Timestamp time)
{
    int id = js["id"].get<int>();
    int friendID = js["friendid"].get<int>();
    m_friendModel.insert(id, friendID);
}

void ChatService::_onReplyRedis(int id, string messages)
{
    {
        std::lock_guard guard(m_mutex);
        auto it = m_mapID2Conn.find(id);
        if (it != m_mapID2Conn.end())
        {
            it->second->send(messages);
            return;
        }
    }
    m_offlineModel.insert(id, messages);
}
#include "User.h"
#include "Group.h"
#include "json.hpp"
#include "Public.h"
#include <iostream>
#include <string>
#include <functional>
#include <sys/socket.h>
#include <unordered_map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <semaphore.h>
#include <atomic>
using json = nlohmann::json;
User g_currentUser;
std::atomic_bool isLogout = true;
sem_t rwSem;

std::unordered_map<std::string, std::string> commandMap = {
    {"help", "显示支持的所有格式命令"},
    {"chat","一对一聊天,格式:chat:friendid:message"},
    {"addfrined","添加好友,格式:addfriend:friendid"},
    {"creategroup","创建群组,格式:creategroup:groupname:groupdesc"},
    {"addgroup","加入群组,格式:addgroup:groupid"},
    {"groupchat","群组聊天,格式:groupchat:groupid:message"},
    {"loginout","退出"}
};

void help(int clientid = 0, std::string str = "")
{
    for(auto it = commandMap.begin(); it != commandMap.end(); it++)
    {
        std::cout << "command:" << it->first << "    " << "说明:" << it->second << std::endl;
    }
}

void chat(int clientid, std::string message)
{
    int index = message.find(":");
    if (index == -1)
    {
        std::cout << "输入命令格式有误" << std::endl;
    }
    else
    {
        int friendid = atoi(message.substr(0, index).c_str());
        std::string strMessage = message.substr(index + 1);
        json js;
        js["toid"] = friendid;
        js["fromname"] = g_currentUser.getName();
        js["message"] = strMessage;
        js["msgid"] = ONE_CHAT_MSG;
        int len = send(clientid, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
        if (len == -1)
        {
            std::cout << "发送消息失败" << std::endl;
        }
    }
}

void addfrined(int clientid,std::string message)
{
    int friendid = atoi(message.c_str());
    json js;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friendid;
    js["msgid"] = ADD_Friend_MSG;
    int len = send(clientid, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
    if (len == -1)
    {
        std::cout << "发送消息失败" << std::endl;
    }
}

void creategroup(int clientid,std::string message)
{
    int index = message.find(":");
    if (index == -1)
    {
        std::cout << "输入命令格式有误" << std::endl;
    }
    else
    {
        std::string name = message.substr(0, index).c_str();
        std::string desc = message.substr(index + 1);
        json js;
        js["groupName"] = name;
        js["id"] = g_currentUser.getId();
        js["groupDesc"] = desc;
        js["msgid"] = ADD_GROUP_MSG;
        int len = send(clientid, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
        if (len == -1)
        {
            std::cout << "发送消息失败" << std::endl;
        }
    }
}

void addgroup(int clientid,std::string message)
{
    int groupid = atoi(message.c_str());
    json js;
    js["useid"] = g_currentUser.getId();
    js["groupid"] = groupid;
    js["msgid"] = ADD_User_MSG;
    int len = send(clientid, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
    if (len == -1)
    {
        std::cout << "发送消息失败" << std::endl;
    }
}

void groupchat(int clientid,std::string message)
{
    int index = message.find(":");
    if (index == -1)
    {
        std::cout << "输入命令格式有误" << std::endl;
    }
    else
    {
        int groupid = atoi(message.substr(0, index).c_str());
        std::string strMessage = message.substr(index + 1);
        json js;
        js["groupid"] = groupid;
        js["name"] = g_currentUser.getName();
        js["id"] = g_currentUser.getId();
        js["message"] = strMessage;
        js["msgid"] = Chat_Group_MSG;
        int len = send(clientid, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
        if (len == -1)
        {
            std::cout << "发送消息失败" << std::endl;
        }
    }
}

void loginout(int clientid,std::string message)
{
    json js;
    js["id"] = g_currentUser.getId();
    js["msgid"] = LoginOut;
    int len = send(clientid, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
    if (len == -1)
    {
        std::cout << "发送消息失败" << std::endl;
    }
    else
    {
        isLogout = true;
    }
}

std::unordered_map<std::string, std::function<void (int, std::string)>> commandHandler = {
    {"help",help},
    {"chat",chat},
    {"addfrined",addfrined},
    {"creategroup",creategroup},
    {"addgroup",addgroup},
    {"groupchat",groupchat},
    {"loginout",loginout}
};

void showWelcome()
{
    std::cout << "**********************************" << std::endl;
    std::cout << "    欢迎使用聊天系统，系统功能如下   " << std::endl;
    std::cout << "             1.注册                " << std::endl;
    std::cout << "             2.登录                " << std::endl;
    std::cout << "             3.退出                " << std::endl;
    std::cout << "**********************************"  << std::endl;
}

void doLogin(json js)
{
    std::cout << "登录成功" << std::endl;
    g_currentUser.setId(js["id"]);
    g_currentUser.setName(js["name"]);
    g_currentUser.setPwd(js["password"]);
    g_currentUser.setState("online");
    std::cout << "-----离线消息------" << std::endl;
    if (js.contains("offlineMessage"))
    {
        std::vector<std::string> vec = js["offlineMessage"];
        for (std::string str : vec)
        {
            json message = json::parse(str);
            if (message["msgid"] == ONE_CHAT_MSG)
            {
                std::cout << "消息来自于:" << message["fromname"] << "   消息内容为:" << message["message"] << std::endl;
            }
            else
            {
                std::cout << "消息来自于群:" << message["groupid"] << "    发送者为:" << message["name"]
                << "   消息内容为:" << message["message"] << std::endl;
            }
        }
    }
    std::cout << "-----friends list -----" << std::endl;
    if (js.contains("friends"))
    {
        std::vector<std::string> vec = js["friends"];
        for (std::string str : vec)
        {
            json user = json::parse(str);          
            std::cout << "姓名:" << user["name"] << "   id:" << user["id"] << "   状态" <<  user["state"] << std::endl;
        }
    }
    std::cout << "-----groups list -----" << std::endl;
    if (js.contains("groups"))
    {
        std::vector<std::string> vec = js["groups"];
        for (std::string str : vec)
        {
            json group = json::parse(str);          
            std::cout << "群名:" << group["groupName"] << "   id:" << group["groupId"] << "    群描述:" << group["groupDesc"] << std::endl;
        }
    }
    isLogout = false;
    sem_post(&rwSem);
}

void doReg(json js)
{
     if (js["errno"] == 0)
    {
        std::cout << "注册成功, id为:"  << js["id"] << std::endl;
    }
    else
    {
        std::cout << js["errMsg"] << std::endl;
    }
}

void readTask(int client)
{
    for(;;)
    {
        char buffer[1024]= {0};
        int len = recv(client, buffer, 1024, 0);
        if (len != -1)
        {
            json js = json::parse(buffer);
            if (js["msgid"] == ONE_CHAT_MSG)
            {
                std::cout << "一对一聊天,发送者名称:" << js["fromname"] <<  "    消息:" <<  js["message"]  << std::endl;
                continue;
            }
            else if (js["msgid"] == Chat_Group_MSG)
            {
                std::cout << "群组聊天,发送者名称:" << js["name"] << "发送群组:" << js["groupid"] <<  "    消息:" <<  js["message"]  << std::endl;
                continue;
            }
            else if (js["msgid"] == LOG_MSG_ACK)
            {
                if (js["errno"] == 0)
                {
                    doLogin(js);
                }
                else
                {
                    std::cout << js["errMsg"] << std::endl;
                    isLogout = true;
                    sem_post(&rwSem);
                }
            }
            else if (js["msgid"] == REG_MSG_ACK)
            {
                doReg(js);
            }
        }
    }
}

void parseCommand(int client)
{
    while(!isLogout)
    {
        char charMessage[50] = {0};
        std::cin.getline(charMessage, 50);
        std::string strMessage = charMessage;
        int index = strMessage.find(":");
        std::string command;
        if (index == -1)
        {
            command = strMessage;
        }
        else
        {
            command = strMessage.substr(0, index).c_str();
        }
        if (commandHandler.find(command) != commandHandler.end())
        {
            commandHandler[command](client, strMessage.substr(index + 1));
        }
        else
        {
            std::cout << "无法解析的命令" << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "输入的参数不对,请重新输入,类似程序名 ip 端口" << std::endl;
        return 0;
    }
    std::string strIp = argv[1];
    int port = atoi(argv[2]);
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        std::cout << "连接创建失败" << std::endl;
        return 0;
    }
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(strIp.c_str());
    if (connect(client, (sockaddr*)&address, sizeof(address)) == -1)
    {
        std::cout << "连接服务器失败" << std::endl;
        close(client);
        return 0;
    }
    sem_init(&rwSem, 0 ,0);
    std::thread th(readTask, client);
    th.detach();
    for (;;)
    {
        showWelcome();
        std::cout << "请输入你的选择:" << std::endl;
        int choice;
        std::cin >> choice;
        std::cin.get();
        switch(choice)
        {
            case 1:
            {
                char name[50] = {0};
                char password[50] = {0};
                std::cout << "请输入名称:" << std::endl;
                std::cin.getline(name, 50);
                std::cout << "请输入密码:" << std::endl;
                std::cin.getline(password, 50);
                json js;
                js["name"] = name;
                js["password"] = password;
                js["msgid"] = REG_MSG;
                int len = send(client, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
                if (len == -1)
                {
                    std::cout << "发送消息失败" << std::endl;
                    continue;
                }
                break;
            }
            case 2:
            {
                int id;
                char password[50] = {0};
                std::cout << "请输入账号:" << std::endl;
                std::cin >> id;
                std::cin.get();
                std::cout << "请输入密码:" << std::endl;
                std::cin.getline(password, 50);
                json js;
                js["id"] = id;
                js["password"] = password;
                js["msgid"] = LOG_MSG;
                int len = send(client, js.dump().c_str(), strlen(js.dump().c_str()) + 1, 0);
                if (len == -1)
                {
                    std::cout << "发送消息失败" << std::endl;
                    continue;
                }
                sem_wait(&rwSem);
                if (isLogout)
                {
                    break;
                }
                help();
                parseCommand(client);
                break;
            }
            case 3:
            {
                close(client);
                exit(0);
            }
        }
    }
}



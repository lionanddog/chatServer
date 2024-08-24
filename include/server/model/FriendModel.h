#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H
#include <vector>
#include <string>
#include <User.h>
// 好友数据库操作类
class FriendModel
{
public:
    // 查询好友
    std::vector<User> query(int id);
    // 添加好友
    bool insert(int id, int friendid);
};


#endif
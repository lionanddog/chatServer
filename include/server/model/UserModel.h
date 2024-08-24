#ifndef USERMODEL_H
#define USERMODEL_H

#include "User.h"

class UserModel
{
public:
    UserModel();
    ~UserModel();
    
    // User表的增加方法
    bool insert(User& user);

    // User表根据id查询
    User query(int id);

    // User表更改在线状态
    bool update(User& user);

    // User表重置在线状态
    bool reserOffline();

};

#endif
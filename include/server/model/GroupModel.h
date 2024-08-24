#ifndef GROUPMODEL_H
#define GROUPMODEL_H
#include <string.h>
#include "Group.h"

class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group& group);
    // 向群组添加用户
    bool addGroup(int groupid, int userid, std::string role);
    // 查询群组
    std::vector<Group> query(int userid);
    // 查询在当前群组中的所有人
    std::vector<GroupUser> queryByGroupID(int groupid, int userid);
};

#endif
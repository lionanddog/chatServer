#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include "GroupUser.h"

// 群组映射类
class Group
{
public:
    // 获取属性
    int getId();
    std::string getName();
    std::string getDesc();
    std::vector<GroupUser>& getVecGroupUser();

    // 设置属性
    void setId(int id);
    void setName(std::string name);
    void setDesc(std::string desc);

private:
    int m_id;
    std::string m_name;
    std::string m_desc;
    std::vector<GroupUser> vecGroupUser;
};
#endif
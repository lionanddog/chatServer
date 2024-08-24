#ifndef GROUPUSER_H
#define GROUPUSER_H
#include "User.h"

class GroupUser : public User
{
public:
    std::string getRole() {return m_strRole;};
    void setRole(std::string strRole) {m_strRole = strRole;};
private:
    std::string m_strRole;
};


#endif
#include "Group.h"

int Group::getId()
{
    return m_id;
}

std::string Group::getName()
{
    return m_name;
}

std::string Group::getDesc()
{
    return m_desc;
}

std::vector<GroupUser>& Group::getVecGroupUser()
{
    return vecGroupUser;
}

void Group::setId(int id)
{
    m_id = id;
}

void Group::setName(std::string name)
{
    m_name = name;
}

void Group::setDesc(std::string desc)
{
    m_desc = desc;
}
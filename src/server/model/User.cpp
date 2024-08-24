#include "User.h"

User::User(int id, std::string name, std::string pwd, std::string state)
    :m_id(id), m_name(name), m_pwd(pwd), m_state(state)
{
    
}

User::~User()
{

}

std::string User::getName()
{
    return m_name;
}

int User::getId()
{
    return m_id;
}

std::string User::getPwd()
{
    return m_pwd;
}

std::string User::getState()
{
    return m_state;
}

void User::setId(int id)
{
    m_id = id;
}

void User::setName(std::string name)
{
    m_name = name;
}

void User::setPwd(std::string pwd)
{
    m_pwd = pwd;
}
void User::setState(std::string state)
{
    m_state = state;
}
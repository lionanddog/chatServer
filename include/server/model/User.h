/*
    用户类,包含用户信息
*/
#ifndef USER_H
#define USER_H
#include <string>
class User
{
public:
    User(int id = -1, std::string name = "", std::string pwd = "", std::string state = "offline");
    ~User();

    // 获取用户属性
    std::string getName();
    int getId();
    std::string getPwd();
    std::string getState();

    // 设置用户属性
    void setId(int id);
    void setName(std::string name);
    void setPwd(std::string pwd);
    void setState(std::string state);
private:
    int m_id;            //< id主键
    std::string m_name;  //< 名称
    std::string m_pwd;   //< 密码
    std::string m_state; //< 状态:是否在线
};

#endif
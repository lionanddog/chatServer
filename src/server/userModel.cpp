#include "UserModel.h"
#include "db.h"


UserModel::UserModel()
{

}

UserModel::~UserModel()
{

}

bool UserModel::insert(User& user)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into user(name, password, state) values('%s', '%s', '%s')", 
    user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MySQLIMP mysql;
    if (mysql.connect())
    {
        if (mysql.update(strSql))
        {
            user.setId(mysql_insert_id(mysql.getConn()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select name, password, state from user where id = %d", id);
    MySQLIMP mysql;
    if (mysql.connect())
    {
        MYSQL_RES* result = mysql.query(strSql);
        if (result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            User user;
            user.setId(id);
            user.setName(row[0]);
            user.setPwd(row[1]);
            user.setState(row[2]);
            mysql_free_result(result);
            return user;
        }
    }
    return User();
}

// User表更改在线状态
bool UserModel::update(User& user)
{
    char strSql[1024] = {0};
    sprintf(strSql, "update user set state='%s' where id = %d", user.getState().c_str(), user.getId());
    MySQLIMP mysql;
    if (mysql.connect())
    {
        if (mysql.update(strSql))
        {
            return true;
        }
    }
    return false;
}

bool UserModel::reserOffline()
{
    char strSql[1024] = {0};
    sprintf(strSql, "update user set state='offline' where state='online'");
    MySQLIMP mysql;
    if (mysql.connect())
    {
        if (mysql.update(strSql))
        {
            return true;
        }
    }
    return false;
}
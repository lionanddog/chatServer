#include "FriendModel.h"
#include "db.h"


std::vector<User> FriendModel::query(int id)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select a.id, a.name, a.state from user a inner join friend b on b.userid=%d and a.id=b.friendid", id);
    std::vector<User> vec;
    MySQLIMP mysql;
    if (mysql.connect())
    {
        MYSQL_RES* result = mysql.query(strSql);
        if (result != nullptr)
        {
            MYSQL_ROW row = nullptr;
            while ((row = mysql_fetch_row(result)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
        }
    }
    return vec;
}

bool FriendModel::insert(int id, int friendid)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into friend(userid, friendid) values(%d, %d)", id, friendid);
    MySQLIMP mysql;
    if (mysql.connect() && mysql.update(strSql))
    {
        return true;
    }
    return false;
}
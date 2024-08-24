#include "OfflineMessageModel.h"
#include "db.h"


 OfflineMessageModel::OfflineMessageModel()
 {

 }

 OfflineMessageModel::~OfflineMessageModel()
 {

 }

std::vector<std::string> OfflineMessageModel::query(int id)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select message from offlinemessage where userid=%d", id);
    std::vector<std::string> vec;
    MySQLIMP mysql;
    if (mysql.connect())
    {
        MYSQL_RES* result =  mysql.query(strSql);
        if (result != nullptr)
        {
            MYSQL_ROW row = nullptr;
            while ((row = mysql_fetch_row(result)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(result);
        }
    }
    return vec;
}

bool OfflineMessageModel::insert(int id, std::string strMessage)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into offlinemessage(userid, message) values(%d, '%s')", id, strMessage.c_str());
    MySQLIMP mysql;
    if (mysql.connect() && mysql.update(strSql))
    {
        return true;
    }
    return false;
}

bool OfflineMessageModel::remove(int id)
{
    char strSql[1024] = {0};
    sprintf(strSql, "delete from offlinemessage where userid = %d", id);
    MySQLIMP mysql;
    if (mysql.connect() && mysql.update(strSql))
    {
        return true;
    }
    return false;
}
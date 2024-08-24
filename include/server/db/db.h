#ifndef DB_H
#define DB_H
#include <mysql/mysql.h>
#include <string>

// 数据库操作类
class MySQLIMP
{
public:
    // 初始化数据库连接
    MySQLIMP();

    // 释放数据库连接资源
    ~MySQLIMP();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(std::string sql);

    // 查询操作
    MYSQL_RES* query(std::string sql);

    // 获取数据库连接
    MYSQL * getConn();
private:
    MYSQL *m_conn;
};

#endif
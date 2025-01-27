#include "db.h"
#include <muduo/base/Logging.h>

// 数据库配置信息
static std::string server = "127.0.0.1";
static std::string user = "root";
static std::string password = "123456";
static std::string dbname = "chat";

// 创建数据库连接
MySQLIMP::MySQLIMP()
{
    m_conn = mysql_init(nullptr);
}

// 释放数据库连接资源
MySQLIMP::~MySQLIMP()
{
    if (m_conn != nullptr)
    mysql_close(m_conn);
}

// 连接数据库
bool MySQLIMP::connect()
{
    MYSQL *p = mysql_real_connect(m_conn, server.c_str(), user.c_str(),
    password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(m_conn, "set names gbk");
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
        << "连接成功!";
    }
    else
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
        << "连接失败!";
    }
    return p;
}

// 更新操作
bool MySQLIMP::update(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
        << sql << "更新失败!";
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES* MySQLIMP::query(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
        << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(m_conn);
}

MYSQL *MySQLIMP::getConn()
{
    return m_conn;
}

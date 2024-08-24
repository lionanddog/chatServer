#include "GroupModel.h"
#include "db.h"

bool GroupModel::createGroup(Group& group)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());
    MySQLIMP mysql;
    if (mysql.connect() && mysql.update(strSql))
    {
        group.setId(mysql_insert_id(mysql.getConn()));
        return true;
    }
    return false;
}

bool GroupModel::addGroup(int groupid, int userid, std::string role)
{
    char strSql[1024] = {0};
    sprintf(strSql, "insert into groupuser(groupid, userid, grouprole) values(%d, %d, '%s')", groupid, userid, role.c_str());
    MySQLIMP mysql;
    if (mysql.connect() && mysql.update(strSql))
    {
        return true;
    }
    return false;
}

std::vector<Group> GroupModel::query(int userid)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join  groupuser b \
    on b.userid = %d and a.id = b.groupid", userid);
    MySQLIMP mysql;
    std::vector<Group> vec;
    if (mysql.connect())
    {
        MYSQL_RES* result = mysql.query(strSql);
        if (result != nullptr)
        {
            MYSQL_ROW row = nullptr;
            while ((row = mysql_fetch_row(result)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                vec.push_back(group);
            }
        }
        mysql_free_result(result);
        for (Group& group : vec)
        {
            memset(strSql,0,1024);
            sprintf(strSql, "select a.id, a.name, a.state, b.grouprole from user a inner join  groupuser b \
            on b.groupid = %d and a.id = b.userid", group.getId());
            MYSQL_RES* result = mysql.query(strSql);
            if (result != nullptr)
            {
                MYSQL_ROW row = nullptr;
                while ((row = mysql_fetch_row(result)) != nullptr)
                {
                    GroupUser groupUser;
                    groupUser.setId(atoi(row[0]));
                    groupUser.setName(row[1]);
                    groupUser.setState(row[2]);
                    groupUser.setRole(row[3]);
                    group.getVecGroupUser().push_back(groupUser);
                }
            }
            mysql_free_result(result);
        }
    }
    return vec;
}

std::vector<GroupUser> GroupModel::queryByGroupID(int groupid, int userid)
{
    char strSql[1024] = {0};
    sprintf(strSql, "select a.id, a.name, a.state, b.grouprole from user a inner join groupuser b \
    on b.groupid=%d and b.userid != %d and a.id=b.userid", groupid, userid);
    std::vector<GroupUser> vec;
    MySQLIMP mysql;
    if (mysql.connect())
    {
        MYSQL_RES* result = mysql.query(strSql);
        if (result != nullptr)
        {
            MYSQL_ROW row = nullptr;
            while ((row = mysql_fetch_row(result)) != nullptr)
            {
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setRole(row[3]);
                vec.push_back(groupUser);
            }
        }
    }
    return vec;
}
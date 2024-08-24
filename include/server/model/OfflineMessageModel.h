/*
    offlinemessage表操作类
*/
#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include <vector>
#include <string>
class OfflineMessageModel
{
public:
    OfflineMessageModel();
    ~OfflineMessageModel();

    // 根据id查询离线信息
    std::vector<std::string> query(int id);

    // 根据id和message插入离线数据
    bool insert(int id, std::string strMessage);

    // 根据id删除离线数据
    bool remove(int id);

};


#endif
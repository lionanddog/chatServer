/*
    客户端服务端公共定义
 */

enum MsgType
{
    LOG_MSG = 1,   //< 登录
    LOG_MSG_ACK,   //< 登录确认
    REG_MSG,       //< 注册
    REG_MSG_ACK,   //< 注册回复
    ONE_CHAT_MSG,  //< 点对点聊天
    ADD_GROUP_MSG, //< 创建群组
    ADD_User_MSG,  //< 向群组中添加用户
    Chat_Group_MSG,//< 向群聊中发送消息
    ADD_Friend_MSG,//< 添加好友
    LoginOut       //< 退出
};
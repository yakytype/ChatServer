#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <mutex>
#include <functional>
#include <unordered_map>
// #include <memory>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "usermodel.hpp"
#include "redis.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "json.hpp"
#include "user.hpp"
using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;
//聊天服务器业务类 单例模式
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService *instance();
    //处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //加入群聊
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //群聊功能
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    void handleRedisSubscribeMessage(int userid, string msg);
    //服务器异常退出后，业务重置方法
    void reset();
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
private:
    ChatService();

    //存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msghandlerMap;

    //存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    //定义互斥锁 保证userconnmap的线程安全
    mutex _connMutex;

    //数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    //redis操作对象
    Redis _redis;
};

#endif
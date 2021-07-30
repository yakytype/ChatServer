#include "chatserver.hpp"
#include "chatservice.hpp"
#include <functional>
#include <string>
#include "json.hpp"
#include <iostream>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg)
            :_server(loop, listenAddr, nameArg),
            _loop(loop)
{
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    //客户端用户断开链接
    if(!conn->connected()){
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    //数据的反序列化
    json js = json::parse(buf);
    //达到的目的：完全解耦网络模块和业务模块的代码
    //通过js["msgid"] 获取业务处理器handler->conn js time...
    MsgHandler msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    //回调消息对应绑定好的事件处理器 来执行相应的业务处理
    msgHandler(conn, js, time);
}
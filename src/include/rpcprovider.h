#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/callback.h>
#include <thread>
#include <functional>
#include <string>
#include <unordered_map>

// 框架提供的，专门负责发布rpc服务的网络对象类
class RpcProvider
{
public:
    //  框架为外部提供的，可以发布rpc方法的函数接口
    void NotifyService(::google::protobuf::Service *service);

    // 启动Rpc服务节点，开始提供Rpc远程网络调用服务
    void Run();

private:
    // 新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr &);
    // 已经建立连接的读写事件回调
    void OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);
    // Closure回调操作，用于序列化Rpc的响应和使用网络发送
    void SendRpcResponse(const muduo::net::TcpConnection &, ::google::protobuf::Message *);

private:
    // 组合EventLoop
    muduo::net::EventLoop m_eventLoop_;

    // 服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;                                                    // 服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap; // 保存服务方法
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap_;
};
#pragma once
#include "google/protobuf/service.h"

// 框架提供的，专门负责发布rpc服务的网络对象类
class RpcProvider
{
public:
    //  框架为外部提供的，可以发布rpc方法的函数接口
    void NotifyService(::google::protobuf::Service *service);
    
    // 启动Rpc服务节点，开始提供Rpc远程网络调用服务
    void Run();
};
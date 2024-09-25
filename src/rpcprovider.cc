#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <thread>
#include <functional>

void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserver_ip");
    uint16_t port = stoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserver_port"));
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop_, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, 
                                                                std::placeholders::_1, 
                                                                std::placeholders::_2, 
                                                                std::placeholders::_3));

    //  设置处理线程为当前服务器最大线程
    server.setThreadNum(std::thread::hardware_concurrency());

    std::cout << "RpcProvider statrt service at ip: " << ip << " port: " << port << std::endl;

    // 启动网络服务
    server.start();
    m_eventLoop_.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &)
{
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp)
{
}

#include "rpcprovider.h"
#include "mprpcapplication.h"


void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
    ServiceInfo service_info;

    // 获取服务对象的描述
    const google::protobuf::ServiceDescriptor* p_serviceDesc = service->GetDescriptor();
    // 获取服务名
    std::string service_name = p_serviceDesc->name();
    // 获取服务对象service方法的数量
    int methodCnt = p_serviceDesc->method_count();

    // std::cout << service_name << std::endl;

    // 遍历来获取service中的method
    for(int i = 0; i < methodCnt; i++)
    {
        // 获取了服务对象执行下标的服务方法的描述 (抽象描述)
        const google::protobuf::MethodDescriptor* p_methodDesc = p_serviceDesc->method(i);
        std::string method_name = p_methodDesc->name();

        // std::cout << method_name << std::endl;
        service_info.m_methodMap.insert({method_name, p_methodDesc});
    }
    service_info.m_service;
    m_serviceMap_.insert({service_name, service_info});
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

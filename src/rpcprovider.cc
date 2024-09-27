#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
    ServiceInfo service_info;

    // 获取服务对象的描述
    const google::protobuf::ServiceDescriptor *p_serviceDesc = service->GetDescriptor();
    // 获取服务名
    std::string service_name = p_serviceDesc->name();
    // 获取服务对象service方法的数量
    int methodCnt = p_serviceDesc->method_count();

    // std::cout << service_name << std::endl;

    // 遍历来获取service中的method
    for (int i = 0; i < methodCnt; i++)
    {
        // 获取了服务对象执行下标的服务方法的描述 (抽象描述)
        const google::protobuf::MethodDescriptor *p_methodDesc = p_serviceDesc->method(i);
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

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client的连接断开
        conn->shutdown(); // 关闭socket文件描述符
    }
}

/*
    定义消息协议为:  header_size(4 bytes) + header_str + args_str
    header_str = service_name + method_name + args_size(4bytes,防止粘包)
*/
// 从消息内容中解析出服务名、服务方法、参数
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp)
{
    // 网络上接收的远程rpc调用请求的字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字符流, 反序列化数据得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);

    mprpc::RpcHeader rpcheader;
    std::string service_name; // 服务名
    std::string method_name;  // 方法名
    uint32_t args_size;       // 参数长度
    if (rpcheader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpcheader.service_name();
        method_name = rpcheader.method_name();
        args_size = rpcheader.args_size();
    }
    else
    {
        // 数据头反序列化失败
        std::cout << "rpc_header_str" << rpc_header_str << " parse error !" << std::endl;
        return;
    }

    // 解析参数内容
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "====================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "====================================================" << std::endl;

    // 获取service对象和其中的method
    auto it = m_serviceMap_.find(service_name);
    if (it == m_serviceMap_.end())
    {
        // 请求的方法不存在
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        // 请求的方法不存在
        std::cout << service_name << " : " << method_name << " is not exist!" << std::endl;
        return;
    }

    ::google::protobuf::Service *service = it->second.m_service;
    const ::google::protobuf::MethodDescriptor *method = mit->second;

    // 生成rpc方法调用的request和响应response参数
    ::google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error ! content : " << args_str << std::endl;
        return;
    }
    ::google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                                                                    const muduo::net::TcpConnectionPtr &,
                                                                    google::protobuf::Message *>(this,
                                                                                                &RpcProvider::SendRpcResponse,
                                                                                                conn, response);
    // 在框架上根据远端的rpc调用请求，调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, ::google::protobuf::Message *response)
{
    // 将response内容序列化
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        // 序列化成功，则将rpc方法执行结果发送给调用方
        conn->send(response_str);
    }
    else
    {
        // 序列化失败
        std::cout << "serialize response_str error !" << std::endl;
    }

    conn->shutdown();       // 模拟http短链接服务，由rpcprovider主动断开链接
}

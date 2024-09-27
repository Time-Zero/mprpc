#include "mprpcchannel.h"
#include <string>

/*
rpc调用协议为:
header_size(4 bytes) + service_name + method_name + args_size + args
header_size = service_name_len + method_name_len + args_size
*/
void MprpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
                              ::google::protobuf::RpcController *controller,
                              const ::google::protobuf::Message *request,
                              ::google::protobuf::Message *response,
                              ::google::protobuf::Closure *done)
{
    const ::google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        std::cout << "serialize request error !" << std::endl;
        return;
    }

    // 完成rpc请求的请求头
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpc_header.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        std::cout << "serialize rpc header error !" << std::endl;
        return;
    }

    // 组织待发送的rpc请求的字符串
    std::string rpc_send_str;
    rpc_send_str.insert(0, std::string((char *)&header_size, 4)); // 前四个字节存放请求头长度
    rpc_send_str += rpc_header_str;                               // 请求头信息
    rpc_send_str += args_str;                                     // 参数

    // 打印调试信息
    std::cout << "====================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "====================================================" << std::endl;

    // TODO: 模块化发送功能，并且使用智能指针并定义自定义删除器维护socket连接
    // 使用tcp，完成rpc方法的远程调用
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        std::cout << "create socket error, errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserver_ip");
    uint16_t port = stoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserver_port"));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc节点
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "connect error ! error: " << errno << std::endl;
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // 发送rpc报文
    if (send(client_fd, rpc_send_str.c_str(), rpc_send_str.size(), 0) == -1)
    {
        std::cout << "send error! errno: " << errno << std::endl;
        close(client_fd);
        return;
    }

    // 接受rpc请求的响应内容
    char recv_buf[2048] = {0}; // 2k大小的缓冲区
    uint32_t recv_size = 0;
    if ((recv_size = recv(client_fd, recv_buf, sizeof(recv_buf), 0)) == -1)
    {
        std::cout << "recv error! errno: " << errno << std::endl;
        close(client_fd);
        return;
    }

    // 反序列化
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        std::cout << "parse error! response_str: " << recv_buf << std::endl;
        close(client_fd);
        return;
    }

    close(client_fd);
}

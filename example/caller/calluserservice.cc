#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

/*
    rpc的caller实例代码
*/

int main(int argc, char **argv)
{
    // 初始化框架
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;       // rpc 请求
    request.set_name("zhang san");      
    request.set_pwd("123456");
    fixbug::LoginResponse response;     // rpc响应

    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，读取调用的结果
    if(response.result().errcode() == 0)
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;
    }

    return 0;
}
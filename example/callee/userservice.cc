#include <iostream>
#include <string>
#include "../user.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"

/*
    这是一个把本地服务发布为一个Rpc服务的标准动作
    我们这里有一个本地的Login业务
    我们通过继承对应的Rpc类，重写服务，在重写的服务中调用本地业务
    这样就实现了将本地业务发布为一个Rpc服务
*/

class UserService : public fixbug::UserServiceRpc
{
public:
    // 这是实际的本地业务
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Regsiter" << std::endl;
        std::cout << "id: " << id << " name: " << name << " pwd: " << pwd << std::endl;
        return false;
    }

    // Rpc服务提供者应该响应的服务
    void Login(::google::protobuf::RpcController *controller,
                       const ::fixbug::LoginRequest *request,
                       ::fixbug::LoginResponse *response,
                       ::google::protobuf::Closure *done) override
    {
        // 网络层负责数据传输，而应用层只关注数据内容
        std::string name = request->name();         
        std::string pwd = request->pwd();

        // Rpc服务调用对应的本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入
        response->mutable_result()->set_errcode(0);
        response->set_success(login_result);

        // 执行回调操作
        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd= request->pwd();

        bool register_result = Register(id, name ,pwd);

        response->mutable_result()->set_errcode(1);
        response->mutable_result()->set_errmsg(" do register failed !");

        done->Run();
    }
};

int main(int argc, char** argv)
{
    // 调用框架的初始化操作 provider -i config.conf
    MprpcApplication::Init(argc, argv);
    
    RpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();

    return 0;
}
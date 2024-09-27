#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"

int main(int argc, char **argv)
{
    // 初始化框架
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendsListRequest request;       // rpc 请求
    request.set_userid(2);  
    fixbug::GetFriendsListResponse response;     // rpc响应
    MprpcController controller;

    stub.GetFriendsList(&controller, &request, &response, nullptr);

    // 一次rpc调用完成，读取调用的结果
    if(controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if(response.result().errcode() == 0)
    {
        std::cout << "rpc getfriendlist response! " << std::endl;
        uint32_t friend_size = response.friends_size();
        for(int i = 0 ; i < friend_size; i++){
            std::cout << "index: " << (i + 1) << " name: " << response.friends(i) << std::endl;
        }
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
    }
}
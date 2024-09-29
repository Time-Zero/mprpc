#include <cstdio>
#include <iostream>
#include <string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"
#include <vector>
#include "logger.h"

class FriendService : public fixbug::FriendServiceRpc {
public:
    std::vector<std::string> GetFriendList(uint32_t userid) {
        std::cout << "do GetFriendsList Service! userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("张三");
        vec.push_back("李四");
        vec.push_back("王五");
        return vec;
    }

    void GetFriendsList(::google::protobuf::RpcController *controller,
                        const ::fixbug::GetFriendsListRequest *request,
                        ::fixbug::GetFriendsListResponse *response,
                        ::google::protobuf::Closure *done) override {
        uint32_t userid = request->userid();
        std::vector<std::string> vec = GetFriendList(userid);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");

        for (auto &it : vec) {
            std::string *p = response->add_friends();
            *p = it;
        }

        done->Run();
    }
};

int main(int argc, char **argv) {
    LOG_INFO("first log message !");
    LOG_ERROR("%s,%s,%d", __FILE__, __FUNCTION__, __LINE__);

    // 调用框架的初始化操作 provider -i config.conf
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new FriendService());

    provider.Run();

    return 0;
}
#include "test.pb.h"
#include <iostream>
#include <string>

// int main(){
//     fixbug::LoginRequest req;
//     req.set_name("张三");
//     req.set_pwd("123456");

//     // 序列化 
//     std::string send_str;
//     if(req.SerializeToString(&send_str)){
//         std::cout << send_str.c_str() << std::endl;
//     }

//     // 反序列化
//     fixbug::LoginRequest reqB;
//     if(reqB.ParseFromString(send_str)){
//         std::cout << reqB.name() << std::endl;
//         std::cout << reqB.pwd() << std::endl;
//     }



//     return 0;
// }

int main(){
    fixbug::GetFriendListsResponse rsp;
    fixbug::ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    
    fixbug::User *user1 = rsp.add_friend_list();
    user1->set_age(20);
    user1->set_name("zhang san");
    user1->set_sex(fixbug::User::Sex::User_Sex_Man);

    std::cout << rsp.friend_list_size() << std::endl;

    return 0;
}
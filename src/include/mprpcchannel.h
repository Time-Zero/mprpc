#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/callback.h>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include "mprpcapplication.h"
#include "mprpccontroller.h"

class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    // 所有通过stub代理对象调用的rpc方法，都走到这里，统一做rpc方法调用的数据序列化和网络发送
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                        ::google::protobuf::RpcController* controller, const ::google::protobuf::Message* request,
                        ::google::protobuf::Message* response, ::google::protobuf::Closure* done) override;

private:

};
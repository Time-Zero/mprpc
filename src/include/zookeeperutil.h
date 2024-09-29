#pragma once

#include <semaphore.h>
#include <string>
#include <zookeeper/zookeeper.h>


// 封装的zk客户端类
class ZkClient {
public:
    ZkClient();
    ~ZkClient();

    // zkclient启动连接zkserver
    void Start();
    // 再zkserver上根据指定的path创建znode节点, 默认创建永久性节点
    void Create(const char *path, const char *data, int datalen, int state = 0);
    void Create(const std::string& path, const std::string& data, int datalen, int state = 0);
    // 根据参数指定的znode路径，返回znode节点的值
    std::string GetData(const char *path);

private:
    // zk的客户端句柄
    zhandle_t *m_zhandle_;
};
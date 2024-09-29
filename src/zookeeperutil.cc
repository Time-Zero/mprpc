#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include "logger.h"
#include <cstdlib>
#include <semaphore.h>
#include <string>
#include <zookeeper/zookeeper.h>

// 全局的watcher观察器 zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watchCtx)
{
    if(type == ZOO_SESSION_EVENT)                           // 和会话有关的消息
    {
        if(state == ZOO_CONNECTED_STATE)                    // zkclient和zkserver连接成功
        {
            sem_t *sem = (sem_t*)zoo_get_context(zh);       //  从指定的句柄上获取我们设置的信号量
            sem_post(sem);                                  // 资源+1
        }
    }
}

ZkClient::ZkClient() : m_zhandle_(nullptr)
{

}

ZkClient::~ZkClient()
{
    if(m_zhandle_ != nullptr)
    {
        zookeeper_close(m_zhandle_);
    }
}


void ZkClient::Start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeper_ip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeper_port");
    std::string connstr = host + ":" + port;

    /*
    zookeeper_mt:多线程版本 多线程就是指的客户端三线程
    zookeeper的API客户端提供了三个线程
    API调用线程
    网络I/O线程 pthread_creat + poll
    watcher回调线程
    */
    m_zhandle_ = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(m_zhandle_ == nullptr)
    {
        LOG_ERROR("zookeeper_init error!");
        exit(EXIT_FAILURE);
    }

    // zookeeper创建和server之间的session是一个异步的操作，所以我们需要创建一个信号量，当创建成功的信号被传递过来后，再往下执行
    sem_t sem;
    sem_init(&sem, 0 ,0);
    zoo_set_context(m_zhandle_, &sem);

    sem_wait(&sem);
    LOG_INFO("zookeeper_init success!");
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);

    // 判断path的znode节点是否存在，如果存在就不会重复创建
    int flag = zoo_exists(m_zhandle_, path, 0, nullptr);
    if(flag == ZNONODE)
    {
        flag = zoo_create(m_zhandle_, path, data, datalen, 
        &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if(flag == ZOK)
        {
            LOG_INFO("znode create success. path: %s", path);
        }
        else 
        {
            LOG_ERROR("flag: %d", flag);
            LOG_ERROR("znode create error. path: %s", path);
            exit(EXIT_FAILURE);
        }
    }
}

void ZkClient::Create(const std::string& path, const std::string& data, int datalen, int state)
{
    Create(path.c_str(), data.c_str(), datalen, state);
}

std::string ZkClient::GetData(const char *path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle_, path, 0, buffer, &bufferlen, nullptr);
    if(flag == ZOK)
    {
        return std::string(buffer);
    }
    else 
    {
        LOG_ERROR("get znode error. path: %s", path);
        return "";
    }
}
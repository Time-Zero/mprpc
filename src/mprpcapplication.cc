#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

void MprpcApplication::Init(int argc, char **argv)
{
    // 启动时没有指定参数
    if(argc < 2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    // 判断指定参数是否符合要求
    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        }
    }

    // 读取参数并且初始化
    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserver_ip: " << m_config.Load("rpcserver_ip") << std::endl;
    // std::cout << "rpcserver_port: " << m_config.Load("rpcserver_port") << std::endl;
    // std::cout << "zookeeper_ip: " << m_config.Load("zookeeper_ip") << std::endl;
    // std::cout << "zookeeper_port: " << m_config.Load("zookeeper_port") << std::endl;
}

MprpcApplication &MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig &MprpcApplication::GetConfig()
{
    return m_config;
}

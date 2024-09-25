#pragma once

// mprpc框架的初始化类
class MprpcApplication
{
public:    
    // 初始化
    static void Init(int argc, char ** argv);
    
    // 单例对象获取
    static MprpcApplication& GetInstance();

private:
    MprpcApplication(){};
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&& ) = delete;
};
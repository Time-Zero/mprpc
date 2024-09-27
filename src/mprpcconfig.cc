#include "mprpcconfig.h"
#include <iostream>
#include <string>

void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr)
    {
        std::cout << config_file << " is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 1.处理注释   2.正确的配置项  3.去掉开头多余的空格
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, sizeof(buf), pf);

        std::string read_buf(buf);
        Trim(read_buf);

        // 如果是注释或者是空行
        if (read_buf[0] == '#' || read_buf.empty())
            continue;

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1) // 不是合法的配置项
            continue;

        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        int endidx = read_buf.find('\n', idx);    // '='位置之后找换行符
        value = read_buf.substr(idx + 1, endidx - idx - 1); // -2是为了排除文本中换行符的内容
        Trim(value);
        m_configMap_.insert({key, value});
    }

}

std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_configMap_.find(key);
    if (it == m_configMap_.end())
    {
        return "";
    }

    return it->second;
}

void MprpcConfig::Trim(std::string &src_buf)
{

    // 去掉前面多余的空格
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }

    // 去掉配置文件后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}

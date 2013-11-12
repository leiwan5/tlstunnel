#ifndef CONFIG_H
#define CONFIG_H

#include "pch.h"

class Config
{
public:
    Config();

    std::string host;
    int port;

    std::string dst_host;
    int dst_port;

    void* data;
};

#endif // CONFIG_H

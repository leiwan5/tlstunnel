#include "server.h"
#include "config.h"

int main(int argc, char** argv)
{
    Config config;

    for(int i = 1; i < argc; ++i) {
        if(!strcmp("-p", argv[i]) && i+1 < argc)
            config.port = atoi(argv[i+1]);
        else if(!strcmp("-dp", argv[i]) && i+1 < argc)
            config.dst_port = atoi(argv[i+1]);
        else if(!strcmp("-h", argv[i]) && i+1 < argc)
            config.host = argv[i+1];
        else if(!strcmp("-dh", argv[i]) && i+1 < argc)
            config.dst_host = argv[i+1];
    }

    Server server(config);

    return server.start();
}


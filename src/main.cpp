#include <iostream>
#include "grpc_server.h"

int main(int argc, char** argv) {
    int port = 50051;
    int memorySizeMB = 10;
    RunServer(port, memorySizeMB);
    std::cout << "Memory Manager iniciado correctamente" << std::endl;
    return 0;
}

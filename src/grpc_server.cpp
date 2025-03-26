#include "grpc_server.h"
#include "memory_manager.h"

#include "memory_manager.pb.h"
#include "memory_manager.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>

class MemoryManagerServiceImpl final : public memory_manager::MemoryManager::Service {
private:
    MemoryManager manager;

public:
    MemoryManagerServiceImpl(int sizeMB) : manager(sizeMB) {}

    grpc::Status Create(grpc::ServerContext*,
                        const memory_manager::CreateRequest* request,
                        memory_manager::CreateResponse* reply) override {
        int id = manager.createBlock(request->size());
        reply->set_id(id);
        return grpc::Status::OK;
    }

    grpc::Status Set(grpc::ServerContext*,
                     const memory_manager::SetRequest* request,
                     memory_manager::Empty*) override {
        manager.setBlock(request->id(), request->value());
        return grpc::Status::OK;
    }

    grpc::Status Get(grpc::ServerContext*,
                     const memory_manager::GetRequest* request,
                     memory_manager::GetResponse* reply) override {
        reply->set_value(manager.getBlock(request->id()));
        return grpc::Status::OK;
    }

    grpc::Status IncreaseRefCount(grpc::ServerContext*,
                                  const memory_manager::RefCountRequest* request,
                                  memory_manager::Empty*) override {
        manager.increaseRefCount(request->id());
        return grpc::Status::OK;
    }

    grpc::Status DecreaseRefCount(grpc::ServerContext*,
                                  const memory_manager::RefCountRequest* request,
                                  memory_manager::Empty*) override {
        manager.decreaseRefCount(request->id());
        return grpc::Status::OK;
    }
};

void RunServer(int port, int memorySizeMB) {
    std::string address = "0.0.0.0:" + std::to_string(port);
    MemoryManagerServiceImpl service(memorySizeMB);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    std::cout << "Memory Manager listening on " << address << std::endl;
    server->Wait();
}

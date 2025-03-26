#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <fstream>
#include <chrono>
#include <mutex>
#include <vector>

#define MEMORY_SIZE_MB 10
#define MEMORY_SIZE (MEMORY_SIZE_MB * 1024 * 1024) // Convertir MB a bytes

struct MemoryBlock {
    int id;
    void* address;
    int size;
    int refCount;
    bool isFree;
};

class MemoryManager {
private:
    void* memoryPool;  // Único malloc() permitido
    std::vector<MemoryBlock> allocations;
    std::mutex mutex;
    int nextId;

public:
    MemoryManager(int sizeMB);
    ~MemoryManager();
    
    int createBlock(int size);
    void setBlock(int id, const std::string& value);
    std::string getBlock(int id);
    void increaseRefCount(int id);
    void decreaseRefCount(int id);
    void defragmentMemory();
    void dumpMemoryState();
};

#endif

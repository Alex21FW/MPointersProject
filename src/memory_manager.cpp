#include "memory_manager.h"
#include <cstring>

MemoryManager::MemoryManager(int sizeMB) : nextId(1) {
    memoryPool = malloc(sizeMB * 1024 * 1024);  // Única reserva de memoria permitida
    allocations.push_back({-1, memoryPool, sizeMB * 1024 * 1024, 0, true});
}

MemoryManager::~MemoryManager() {
    free(memoryPool);
}

int MemoryManager::createBlock(int size) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& block : allocations) {
        if (block.isFree && block.size >= size) {
            int id = nextId++;
            block.id = id;
            block.isFree = false;
            block.refCount = 1;
            allocations.push_back({-1, (char*)block.address + size, block.size - size, 0, true});
            block.size = size;
            return id;
        }
    }
    return -1;  // No hay espacio disponible
}

void MemoryManager::setBlock(int id, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& block : allocations) {
        if (block.id == id) {
            memcpy(block.address, value.c_str(), std::min(block.size, (int)value.size()));
            dumpMemoryState();
            return;
        }
    }
}

std::string MemoryManager::getBlock(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& block : allocations) {
        if (block.id == id) {
            return std::string((char*)block.address, block.size);
        }
    }
    return "";
}

void MemoryManager::increaseRefCount(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& block : allocations) {
        if (block.id == id) {
            block.refCount++;
            return;
        }
    }
}

void MemoryManager::decreaseRefCount(int id) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& block : allocations) {
        if (block.id == id) {
            block.refCount--;
            if (block.refCount <= 0) {
                block.isFree = true;
                defragmentMemory();
            }
            return;
        }
    }
}

void MemoryManager::defragmentMemory() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (size_t i = 0; i < allocations.size() - 1; i++) {
        if (allocations[i].isFree && allocations[i + 1].isFree) {
            allocations[i].size += allocations[i + 1].size;
            allocations.erase(allocations.begin() + i + 1);
            i--;
        }
    }
}

void MemoryManager::dumpMemoryState() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    std::ofstream dumpFile("memory_dump_" + std::to_string(timestamp) + ".txt");
    for (const auto& block : allocations) {
        dumpFile << "ID: " << block.id << ", RefCount: " << block.refCount << ", Free: " << block.isFree << "\n";
    }
    dumpFile.close();
}

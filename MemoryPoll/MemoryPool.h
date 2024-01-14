#include<cstddef>
#include<vector>
#include<cstdlib>
#include<iostream>
#include<mutex>

struct MemoryPoolBlock
{
public:
    MemoryPoolBlock* next;
};

class MemoryPool
{
public:
    MemoryPool(size_t block_size,size_t block_count);
    ~MemoryPool();

    void* allocate();
    void deallocate(void* ptr);

private:
    MemoryPoolBlock* free_list;
    size_t block_size;
    std::vector<MemoryPoolBlock*>allocated_blocks;
    std::mutex pool_mutex;

    void allocateBlock();
};

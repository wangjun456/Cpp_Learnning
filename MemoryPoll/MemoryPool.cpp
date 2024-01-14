#include"MemoryPool.h"

MemoryPool::MemoryPool(size_t block_size,size_t block_count):free_list(nullptr),block_size(block_size)
{
    std::lock_guard<std::mutex> lock(pool_mutex);
    for(size_t i = 0;i<block_count;i++)
    {
        allocateBlock();
    }
}

MemoryPool::~MemoryPool()
{
    std::lock_guard<std::mutex> lock(pool_mutex);
    for(MemoryPoolBlock* block : allocated_blocks)
    {
        delete(block);
    }
}

void MemoryPool::allocateBlock()
{
    MemoryPoolBlock* new_black = static_cast<MemoryPoolBlock*>(::operator new(block_size));
    if(!new_black)
    {
        throw std::bad_alloc();
    }
    new_black->next = free_list;
    free_list = new_black;
    allocated_blocks.push_back(new_black);

}

void* MemoryPool::allocate()
{
    std::lock_guard<std::mutex> lock(pool_mutex);
    if(!free_list)
    {
        allocateBlock();
    }
    MemoryPoolBlock* block = free_list;
    free_list = block->next;
    return block;
}

void MemoryPool::deallocate(void* ptr)
{
   if(!ptr)
   {
       return;
   }

    std::lock_guard<std::mutex> lock(pool_mutex);
    MemoryPoolBlock* block = static_cast<MemoryPoolBlock*>(ptr);
    block->next = free_list;
    free_list = block;
}

int main()
{
    MemoryPool pool(sizeof(int),100);

    int* p1 = static_cast<int*>(pool.allocate());
    *p1 = 1;
    int* p2 = static_cast<int*>(pool.allocate());
    *p2 = 2;
    pool.deallocate(p1);
    pool.deallocate(p2);

    // Allocate and deallocate more blocks
    for(int i = 0; i < 10; i++)
    {
        int* p = static_cast<int*>(pool.allocate());
        *p = i*3;
    }
    return 0;
}


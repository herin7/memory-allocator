// Think of the computer's memory as a giant, empty warehouse. The memory allocator is the warehouse manager.
// When a program needs to store something (like a variable or an image), it asks the manager for space
#include <unistd.h>
#include <iostream>
// (void*) means Pure Memory add. not a pointer which can be derefrenced ! 
struct Block
{
    size_t size; 
    bool isFree;
    Block *next; 
}
#define META_SIZE sizeof(Block)
Block *entry_point = nullptr;

size_t helpAlign(size_t req)
{
    if (x % 8 == 0)
        return x;
    return x + (8 - (x % 8)); // converting into (lowerbound multiple of 8)
}
Block *request_space(Block *last, size_t size)
{
    Block *b = (Block *)brk(0);
    request = sbrk(size + META_SIZE);
    if (*request == -1)
    {
        cout << "Memory allocation Failed\n";
        return nullptr;
    }
    b.size = size;
    b.next = next;
    b.isfree = false;
    if (last)
    {
        last.next = b;
    }
    return b;
}

// malloc
Block *my_malloc(size_t req)
{
    req = helpAlign(req);
    if (entry_point == nullptr)
    {
        block = request_space(nullptr, req);
        if (!block)
            return nullptr;
        entry_point = block;
    }
    else
    {
        while (last->next)
        {
            last = last->next;
        };
        block = request_space(last, req);
        if (!block)
            return nullptr;
        else
        {
            block->free = false;
        }
    }

    return (void*)(block + 1);
}
// local variable => Call stack
// Global variable => Heap =>because we need to access it anytime
int main()
{
    
}

// Minimal Version of malloc with debug logs
// sbrk(0) returns pointer to top of the memory heap
// sbrk(n) returns  (void*)-1 in failure case (While requesting for n memory space)
#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
std::mutex brk_lock;
using namespace std;

struct Block
{
    size_t size;
    bool free;
    Block *next;
};

#define META_SIZE sizeof(Block)

thread_local Block *head = nullptr;
void heap_stats()
{
    size_t used = 0, free = 0;
    Block *cur = head;
    while (cur)
    {
        if (cur->free)
            free += cur->size;
        else
            used += cur->size;
        cur = cur->next;
    }
    cout << "used=" << used << " free=" << free << endl;
}

size_t align8(size_t x)
{
    size_t y = (x + 7) & ~7;
    cout << "align " << x << " -> " << y << endl;
    return y;
}

Block *request_space(Block *last, size_t size)
{
    std::lock_guard<std::mutex> lock(brk_lock);
    Block *block = (Block *)sbrk(0);
    if (sbrk(size + META_SIZE) == (void *)-1)
        return nullptr;

    block->size = size;
    block->free = false;
    block->next = nullptr;

    if (last)
        last->next = block;

    cout << "new block @" << block << " size " << size << endl;
    return block;
}

Block *find_free_block(size_t size)
{
    Block *cur = head;
    while (cur)
    {
        if (cur->free && cur->size >= size)
        {
            cout << "reuse block @" << cur << endl;
            return cur;
        }
        cur = cur->next;
    }
    return nullptr;
}

void *my_malloc(size_t size)
{
    if (size == 0)
        return nullptr;

    size = align8(size);
    Block *block;

    if (!head)
    {
        block = request_space(nullptr, size);
        head = block;
    }
    else
    {
        block = find_free_block(size);
        if (!block)
        {
            Block *last = head;
            while (last->next)
                last = last->next;
            block = request_space(last, size);
        }
        else
        {
            block->free = false;
        }
    }

    cout << "malloc -> " << (block + 1) << endl;
    return (void *)(block + 1);
}

void my_free(void *ptr)
{
    if (!ptr)
        return;

    Block *block = (Block *)ptr - 1;
    block->free = true;

    cout << "free @" << ptr << endl;

    if (block->next && block->next->free)
    {
        block->size += META_SIZE + block->next->size;
        block->next = block->next->next;
        cout << "merge @" << block << endl;
    }
}

void worker(int id)
{
    char *a = (char *)my_malloc(sizeof(char));
    *a = 'A' + id;
    cout << "thread " << id
         << " alloc @" << (void *)a
         << " value " << *a << endl;
    my_free(a);
    int *b = (int *)my_malloc(sizeof(int));
    *b = id * 100;
    cout << "thread " << id
         << " realloc @" << (void *)b
         << " value " << *b << endl;
}

int main()
{
    cout << "=== Single-threaded ===" << endl;
    char *a = (char *)my_malloc(sizeof(char));
    *a = 'A';
    cout << "main alloc @" << (void *)a << " value " << *a << endl;
    my_free(a);
    int *b = (int *)my_malloc(sizeof(int));
    *b = 42;
    cout << "main realloc @" << (void *)b << " value " << *b << endl;
    cout << "\n=== Multi-threaded ===" << endl;
    thread t1(worker, 1);
    thread t2(worker, 2);
    t1.join();
    t2.join();
}

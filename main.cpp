#include <iostream>
#include <unistd.h>

struct Block {
    size_t size;
    bool free;
    Block* next;
};

#define META_SIZE sizeof(Block)

Block* entry_point = nullptr;

size_t align8(size_t x) {
    if (x % 8 == 0) return x;
    return x + (8 - (x % 8));
}

Block* request_space(Block* last, size_t size) {
    Block* block = (Block*)sbrk(0);
    void* request = sbrk(size + META_SIZE);

    if (request == (void*)-1) {
        return nullptr;
    }

    block->size = size;
    block->free = false;
    block->next = nullptr;

    if (last) {
        last->next = block;
    }

    return block;
}

Block* find_free_block(size_t size) {
    Block* curr = entry_point;
    while (curr) {
        if (curr->free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return nullptr;
}

void* my_malloc(size_t size) {
    if (size == 0) return nullptr;

    size = align8(size);
    Block* block;

    if (!entry_point) {
        block = request_space(nullptr, size);
        if (!block) return nullptr;
        entry_point = block;
    } else {
        block = find_free_block(size);
        if (!block) {
            Block* last = entry_point;
            while (last->next) last = last->next;
            block = request_space(last, size);
            if (!block) return nullptr;
        } else {
            block->free = false;
        }
    }

    return (void*)(block + 1);
}

void my_free(void* ptr) {
    if (!ptr) return;

    Block* block = (Block*)ptr - 1;
    block->free = true;

    if (block->next && block->next->free) {
        block->size += META_SIZE + block->next->size;
        block->next = block->next->next;
    }
}

int main() {
    int* x = (int*)my_malloc(sizeof(int));
    *x = 42;

    my_free(x);

    int* y = (int*)my_malloc(sizeof(int));
    *y = 100;

    std::cout << *y << std::endl;
}

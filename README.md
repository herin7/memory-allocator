# Custom C++ Memory Allocator
A robust, thread-safe implementation of a memory allocator built from scratch in C++. This project demonstrates low-level memory management techniques, including heap manipulation via `sbrk`, large page allocation via `mmap`, and thread-local free lists.

> **⚠️ COMPATIBILITY WARNING:** > This program relies on Unix-specific system calls (`sbrk`, `mmap`, `unistd.h`) and **will only work on Linux or WSL (Windows Subsystem for Linux)**. It will not compile on native Windows (MSVC) or macOS without significant modification.

## 🚀 Key Features

* **Hybrid Allocation Strategy:**
    * **Small Objects:** allocated on the heap using `sbrk`.
    * **Large Objects (>128KB):** allocated directly via `mmap` to reduce heap fragmentation.
* **Thread Safety:** Utilizes `thread_local` storage for free lists to minimize lock contention and a `std::mutex` for thread-safe heap expansion.
* **Memory Efficiency:**
    * **Block Splitting:** Large free blocks are split to fit the exact requested size, preserving the remainder for future use.
    * **Coalescing:** Automatically merges adjacent free blocks (both forward and backward) to prevent fragmentation.
* **Alignment:** Ensures all memory allocations are 8-byte aligned for CPU efficiency.
* **Debug Instrumentation:** Built-in verbose logging to trace allocation, freeing, and merging operations in real-time.

## 🛠️ Architecture

The allocator manages memory as a linked list of blocks. Each block contains a metadata header followed by the user payload.

### The Block Structure
Every allocation is prefixed with a metadata header (`sizeof(Block)`) that tracks:
* `size`: The usable size of the payload.
* `free`: Boolean flag indicating availability.
* `is_mmap`: Flag to distinguish between heap and mmap allocations.
* `next`: Pointer to the next block in the list.

### Allocation Logic (`my_malloc`)
1.  **Alignment:** Adjusts requested size to the nearest multiple of 8.
2.  **Threshold Check:** If size > 128KB, allocates specifically via `mmap`.
3.  **Free List Search:** Scans the thread-local free list for a suitable block.
4.  **Splitting:** If a found block is significantly larger than requested, it is split into two.
5.  **Heap Extension:** If no block is found, requests more memory from the OS via `sbrk`.

### Deallocation Logic (`my_free`)
1.  **Resolution:** specific pointer arithmetic is used to locate the metadata header.
2.  **Unmapping:** If the block was `mmap`'ed, it is returned to the OS immediately.
3.  **Coalescing:** If the block is on the heap, it merges with adjacent free neighbors (Forward and Backward) to create larger contiguous free blocks.

## 📦 How to Build and Run

Since the code uses `<thread>` and `<mutex>`, you must link the pthread library.

### Prerequisites
* GCC or Clang compiler
* Linux/Unix environment (due to `unistd.h`, `sbrk`, `mmap`)

### Compilation
```bash
g++ -o allocator main.cpp -pthread
```

### Execution
```Bash
./allocator
```
### 🔍 Usage Example
The main function provided includes a test suite that runs both single-threaded and multi-threaded scenarios.
C++

// Allocate an integer
int* ptr = (int*)my_malloc(sizeof(int));
*ptr = 100;

// Free the memory
my_free(ptr);

### ⚠️ Implementation Notes
1. **Backward Coalescing**: The current implementation uses an O(N) search (find_prev_block) to merge with previous blocks. In a production environment, a Doubly Linked List would optimize this to O(1).

2. **Thread Local Storage**: Each thread maintains its own free list (thread_local). This avoids locking during the search phase but means memory freed in one thread cannot be reused by another thread.

This project is open-source and available for educational purposes.
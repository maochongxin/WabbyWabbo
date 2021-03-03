// Copyright (c) 2021 Maochongxin. All rights reserved.

#ifndef UTIL_ARENA_H_
#define UTIL_ARENA_H_

#include <vector>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "atomic_pointer.h"

namespace kvdb {

class Arena {
public:
  Arena();
  ~Arena();
	
  char* Allocate(size_t bytes);
	
  char* AllocateAligned(size_t bytes);

  size_t MemoryUsage() const {
 		return reinterpret_cast<uintptr_t>(memory_usage_.NoBarrier_Load());
  }

  Arena(const Arena&) = delete;
  void operator=(const Arena&) = delete;
private:
	char* AllocateFallback(size_t bytes);
	char* AllocateNewBlock(size_t block_btypes);

	char* alloc_ptr_;
	size_t alloc_bytes_remaning_;

	std::vector<char*> blocks_;

	AtomicPointer memory_usage_;
};

inline char* Arena::Allocate(size_t bytes) {
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaning_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaning_ -= bytes;
    return result;
  }
  return AllocateFallback(bytes);
}


} // end of namespace kvdb

#endif

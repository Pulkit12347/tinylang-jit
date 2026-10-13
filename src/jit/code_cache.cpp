#include "jit/code_cache.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

namespace tinylang::jit {

namespace {
constexpr std::size_t kInitialCapacity = 256 * 1024;
}

CodeCache::CodeCache() {
  capacity_ = kInitialCapacity;
  base_ = static_cast<uint8_t*>(mmap(nullptr, capacity_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0));
  if (base_ == MAP_FAILED) {
    throw std::runtime_error("mmap code cache failed");
  }
}

CodeCache::~CodeCache() {
  if (base_ && base_ != MAP_FAILED) {
    munmap(base_, capacity_);
  }
}

uint8_t* CodeCache::allocate(std::size_t size) {
  if (finalized_) {
    throw std::runtime_error("cannot allocate after finalize");
  }
  if (used_ + size > capacity_) {
    throw std::runtime_error("code cache OOM");
  }
  uint8_t* ptr = base_ + used_;
  used_ += size;
  return ptr;
}

void* CodeCache::publish(const void* src, std::size_t size) {
  if (size == 0) return nullptr;
  long page = sysconf(_SC_PAGESIZE);
  if (page <= 0) page = 4096;
  std::size_t alloc = (size + static_cast<std::size_t>(page) - 1) &
                      ~(static_cast<std::size_t>(page) - 1);
  void* mem = mmap(nullptr, alloc, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if (mem == MAP_FAILED) {
    throw std::runtime_error("mmap JIT region failed");
  }
  std::memcpy(mem, src, size);
  if (mprotect(mem, alloc, PROT_READ | PROT_EXEC) != 0) {
    munmap(mem, alloc);
    throw std::runtime_error("mprotect JIT RX failed");
  }
#if defined(__aarch64__)
  __builtin___clear_cache(static_cast<char*>(mem), static_cast<char*>(mem) + size);
#endif
  return mem;
}

void CodeCache::finalize() {
  if (finalized_) return;
  if (used_ == 0) return;
  if (mprotect(base_, capacity_, PROT_READ | PROT_EXEC) != 0) {
    throw std::runtime_error("mprotect RX failed");
  }
  finalized_ = true;

  // Invalidate icache on ARM
#if defined(__aarch64__)
  for (std::size_t off = 0; off < used_; off += 64) {
    __builtin___clear_cache(reinterpret_cast<char*>(base_ + off),
                            reinterpret_cast<char*>(base_ + off + 64));
  }
#endif
}

CodeCache& global_code_cache() {
  static CodeCache cache;
  return cache;
}

}  // namespace tinylang::jit

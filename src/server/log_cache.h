#pragma once

#include <string>
#include <vector>
#include <deque>
#include <atomic>

namespace {
  size_t kMaxCacheSize = 1000000;
}

class LogCache {
 public:
  LogCache(): cache_size_(0), start_seq_(0) {}

  void Put(const size_t key, const std::string& value);
  size_t Get(const size_t seq_num, std::vector<std::string>* logs);

 private:
  std::deque<std::string> log_cache_;
  std::deque<std::string>::iterator iter_;
  std::atomic<size_t> cache_size_;
  std::atomic<size_t> start_seq_; 
};
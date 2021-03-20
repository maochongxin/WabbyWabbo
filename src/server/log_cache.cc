#include "log_cache.h"

void LogCache::Put(const size_t key, const std::string& value) {
  log_cache_.push_back(value);
  cache_size_ += value.size();
  start_seq_ = key - log_cache_.size() + 1;
  iter_ = log_cache_.begin();

  while (cache_size_ > kMaxCacheSize) {
    if (iter_ != log_cache_.end()) {
      cache_size_ -= iter_->size();
      ++iter_;
      log_cache_.pop_front();
      ++start_seq_;
    } else {
      break;
    }
  }
}

size_t LogCache::Get(const size_t seq_num, std::vector<std::string>* logs) {
  if (!logs) { return -1; }
  if (seq_num > start_seq_ + log_cache_.size() - 1) { return -1; }
  logs->clear();
  if (seq_num < start_seq_) {
    for (iter_ = log_cache_.begin(); iter_ != log_cache_.end(); ++iter_) {
      logs->push_back(*iter_);
    }
    return start_seq_;
  }

  iter_ = log_cache_.begin();
  size_t start_index = seq_num - start_seq_;
  if (start_index < log_cache_.size()) {
    iter_ += start_index;
    while (iter_ < log_cache_.end()) {
      logs->push_back(*iter_);
      ++iter_;
    }
  }
  return seq_num;
}
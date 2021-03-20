#pragma once


#include <string>

namespace db {
class DB;
}

class KVDB {
 public:
  KVDB(const std::string& string& db_path, const size_t id);
  virtual ~KVDB();

 public:
  virtual bool Put(const std::string& key, const std::string& value);
  virtual bool Get(const std::string& key, std::string* value);
  virtual bool Delete(const std::string& key);

  inline bool FindKey(const std::string& key);
  inline size_t GetId();
  size_t GetSeq();
  inline void SetSeq(size_t sqe_num);
  inline bool Stop();
  inline void SetStop(bool stop);
  inline void SeekToLogLast();
  inline db::DB::Iterator* NewIterator();
  inline db::DB::Iterator* NewKvIterator();

 private:
  bool InsertIntoLog(const std::string& key, const std::string& value);

  db::DB* data_db_;
  db::DB* log_db_;
  LogCache* log_cache_;
  std::atomic<size_t> seq_num_;
  size_t id_;
  std::atomic<bool> stop_;

};
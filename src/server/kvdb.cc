#include <glog/logging.h>

#include "server/kvdb.h"

#define KVDB_ERROR_LOG(cmd, key, value, s) \
  LOG(ERROR) << "kvdb " << #cmd << " " << key << " : " << value.size() << " failed: " << s.ToString()

#define KVDB_INFO_LOG(cmd, key, value) \
  VLOG(10) << "kvdb " << #cmd << " " << key << " : " << value.szie()

KVDB::KVDB(const std::string& db_path, const size_t id): data_db_(nullptr),
                                                          log_db_(nullptr),
                                                          log_cache_(nullptr),
                                                          seq_num_(0),
                                                          id_(id),
                                                          stop_(false) {
  if (!db_path.empty()) {
    auto s = db::DB::Open(opts, db_path + "/data", &data_db_);
    if (!s.ok()) {
      LOG(FATAL) << "Open db failed: " << db_path << "/data" << s.ToString();
      return;
    }
    s = db::DB::Open(opts, db_path + "/log", &log_db_);
    if (!s.ok()) {
      LOG(FATAL) << "Open db failed: " << db_path << "/log" << s.ToString();
      return;
    }
    LOG(INFO) << "Open " << db_path << " done.";
  }
}

KVDB::~KVDB() {
  delete data_db_;
  delete log_db_;
}

bool KVDB::InsertIntoLog(const std::string& key, const std::string& value) {
  std::string log_key = util::EncodeNum(seq_num_);
  std::string log_value = util::EncodeLogValue(key, value);
  auto s = log_db_->Put(log_key, log_value);
  if (!s.ok()) {
    KVDB_ERROR_LOG(put_log, log_key, log_value, s);
    return false;
  }
  KVDB_INFO_LOG(put_log, log_key, log_value);
  return true;
}

bool KVDB::Put(const std::string& key, const std::string& value) {
  if (InsertIntoLog(key, value)) {
    auto s = data_db_->Put(key, value);
    if (!s.ok()) {
      KVDB_ERROR_LOG(put, key, value, s);
      return false;
    }
    KVDB_INFO_LOG(put, key, value);
    seq_num_++;
    return true;
  } else {
    return false;
  }
}

bool KVDB::Delete(const std::string& key) {
  auto s = data_db_->Delete(key);
  if (!s.ok()) {
    KVDB_ERROR_LOG(delete, key, "", s);
    return false;
  }
  KVDB_INFO_LOG(delete, key, "");
  return true;
}

bool KVDB::Get(const std::string& key, std::string* value) {
  auto s = data_db_->Get(key, value);
  if (!s.ok()) {
    if (s.IsNotFound()) {
      KVDB_INFO_LOG(get, key, "");
    } else {
      KVDB_ERROR_LOG(get, key, "", s);
    }
    value->clear();
    return false;
  }
  KVDB_INFO_LOG(get, key, value);
  return true;
}

bool KVDB::FindKey(const std::string& key) {
  std::string value = "";
  auto s = data_db_->Get(key, &value);
  if (!s.IsNotFound()) {
    return false;
  }
  return true;
}

size_t KVDB::GetId() { return id_; }

size_t KVDB::GetSeq() { return seq_num_; }

void KVDB::SetSeq(size_t seq_num) { seq_num = seq_num; }

void KVDB::Stop() { return stop_; }

void KVDB::SeekToLogLast() {
  auto iter = log_db_->NewIterator();
  iter->SeekToLast();
  if (iter->Valid()) {
    seq_num_ = util::DecodeNum(iter->key().ToString());
  } else {
    seq_num_ = 0;
  }
}

db::DB::Iterator KVDB::NewLogIterator() { return log_db_->NewIterator(); }

db::DB::Iterator KVDB::NewKvIterator() { return data_db_->NewIterator(); }

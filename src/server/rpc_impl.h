#pragma once

#include "protos/server.pb.h"
#include "kvdb.h"

#include <string>
#include <grpc/grpc.h>
#include <glog/logging.h>

class SerivceImpl : public ::grpc::wabbywabbo::dbserver::DbService {
 public:
  SerivceImpl() {}
  ~SerivceImpl() {}

  void UpdateDB(std::map<int32_t, KVDB*> db_list) {
    for (const auto& db : db_list) {
      db_list_[db.first] = db.second;
    }
  }

  void SyncMasterShard(const ::grpc::wabbywabbo::dbserver::GetMasterRequest* request,
                      ::grpc::wabbywabbo::dbserver::GetMasterResponse* response,
                      ::google::protobuf::Closure* done,
                      const std::time_t revc_time) {
    for (const auto& db : db_list_) {
      if (db.second != nullptr) {
        response->add_db_id(db.first);
      }
    }
    done->Run();
  }

  void SyncGetShardData(const ::grpc::wabbywabbo::dbserver::GetShardDataRequest* request,
                        ::grpc::wabbywabbo::dbserver::GetShardDataResponse* response,
                        ::google::protobuf::Closure* done,
                        const std::time_t recv_time) {
    if (db_list_.count(request->record().shard()) == 0 || db_list_[request->record().shard()] == nullptr) {
      response->set_status(::grpc::wabbywabbo::dbserver::Status::ERROR_SHARD_INVALID);
      LOG(INFO) << "shard invalid";
      done->Run();
      return;
    }
    auto record = response->mutable_record();
    record->set_key(request->record().key());
    auto it = db_list_[request->record().shard()]->NewKvIterator();
    std::string value;
    it->Get(request->record().key(), &value);
    if (!value.empty()) {
      record->set_value(value);
      response->set_status(::grpc::wabbywabbo::dbserver::Status::OK);
    } else {
      response->set_status(::grpc::wabbywabbo::dbserver::Status::ERROR_NOT_FOUND);
    }

    done->Run();
  }

  void SyncSetString(const ::grpc::wabbywabbo::dbserver::SetShardDataRequest* request,
                    ::grpc::wabbywabbo::dbserver::SetShardDataResponse* response,
                    ::google::protobuf::Closure* done,
                    std::time_t recv_time) {
    if (db_list_.count(request->record().shard()) == 0 || db_list_[request->record().shard()] == nullptr) {
      response->set_status(::grpc::wabbywabbo::dbserver::Status::ERROR_SHARD_INVALID);
      LOG(INFO) << "shard invalid";
      done->Run();
      return;
    }
    auto record = request->record();
    auto it = db_list_[record.shard()]->NewKvIterator();
    it->Set(record.key(), record.value());
    done->Run();
  }


  void GetMasterShard(::google::protobuf::RpcController* controller,
                      const ::grpc::wabbywabbo::dbserver::GetMasterRequest* request,
                      ::grpc::wabbywabbo::dbserver::GetMasterResponse* response,
                      ::google::protobuf::Closure* done) {
    std::time_t recv_time = std::time(0);
    SyncMasterShard(request, response, done, recv_time);
  }

  void Get(::google::protobuf::RpcController* controller,
          const ::grpc::wabbywabbo::dbserver::GetShardDataRequest* request,
          ::grpc::wabbywabbo::dbserver::GetShardDataResponse* response,
          ::google::protobuf::Closure* done) {
    std::time_t recv_time = std::time(0);
    SyncGetShardData(request, response, done, recv_time);
  }

  void SetString(::google::protobuf::RpcController* controller,
          const ::grpc::wabbywabbo::dbserver::SetShardDataRequest* request,
          ::grpc::wabbywabbo::dbserver::SetShardDataResponse* response,
          ::google::protobuf::Closure* done) {
    std::time_t recv_time = std::time(0);
    SyncSetString(request, response, done, recv_time);
  }

 private:
  std::map<int32_t, KVDB*> db_list_;

};

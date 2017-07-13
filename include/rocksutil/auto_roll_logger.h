//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// Logger implementation that can be shared by all environments
// where enough posix functionality is available.

#pragma once
#include <list>
#include <string>

#include "rport/port.h"
#include "rport/util_logger.h"
#include "rocksutil/mutexlock.h"

namespace rocksutil {

std::string InfoLogFileName(const std::string& log_path); 

// Rolls the log file by size and/or time
class AutoRollLogger : public Logger {
 public:
  AutoRollLogger(Env* env, const std::string& log_path,
                 size_t log_max_size, size_t log_file_time_to_roll,
                 const InfoLogLevel log_level = InfoLogLevel::INFO_LEVEL,
                 uint64_t flush_every_seconds = 5)
      : Logger(flush_every_seconds, log_level),
        log_path_(log_path),
        env_(env),
        status_(Status::OK()),
        kMaxLogFileSize(log_max_size),
        kLogFileTimeToRoll(log_file_time_to_roll),
        cached_now(static_cast<uint64_t>(env_->NowMicros() * 1e-6)),
        ctime_(cached_now),
        cached_now_access_count(0),
        call_NowMicros_every_N_records_(100),
        mutex_() {
    log_fname_ = InfoLogFileName(log_path_);
    RollLogFile();
    ResetLogger();
  }

  using Logger::Logv;
  void Logv(const char* format, va_list ap) override;

  // Write a header entry to the log. All header information will be written
  // again every time the log rolls over.
  virtual void LogHeader(const char* format, va_list ap) override;

  // check if the logger has encountered any problem.
  Status GetStatus() {
    return status_;
  }

  size_t GetLogFileSize() const override {
    std::shared_ptr<Logger> logger;
    {
      MutexLock l(&mutex_);
      // pin down the current logger_ instance before releasing the mutex.
      logger = logger_;
    }
    return logger->GetLogFileSize();
  }

  void Flush() override {
    std::shared_ptr<Logger> logger;
    {
      MutexLock l(&mutex_);
      // pin down the current logger_ instance before releasing the mutex.
      logger = logger_;
    }
    if (logger) {
      logger->Flush();
    }
  }

  virtual ~AutoRollLogger() {
  }

  void SetCallNowMicrosEveryNRecords(uint64_t call_NowMicros_every_N_records) {
    call_NowMicros_every_N_records_ = call_NowMicros_every_N_records;
  }

  // Expose the log file path for testing purpose
  std::string TEST_log_fname() const {
    return log_fname_;
  }

 private:
  bool LogExpired();
  Status ResetLogger();
  void RollLogFile();
  // Log message to logger without rolling
  void LogInternal(const char* format, ...);
  // Serialize the va_list to a string
  std::string ValistToString(const char* format, va_list args) const;
  // Write the logs marked as headers to the new log file
  void WriteHeaderInfo();

  std::string log_fname_; // Current active info log's file name.
  std::string log_path_;
  Env* env_;
  std::shared_ptr<Logger> logger_;
  // current status of the logger
  Status status_;
  const size_t kMaxLogFileSize;
  const size_t kLogFileTimeToRoll;
  // header information
  std::list<std::string> headers_;
  // to avoid frequent env->NowMicros() calls, we cached the current time
  uint64_t cached_now;
  uint64_t ctime_;
  uint64_t cached_now_access_count;
  uint64_t call_NowMicros_every_N_records_;
  mutable port::Mutex mutex_;
};

Status CreateLogger(const std::string& log_path,
         std::shared_ptr<Logger>* logger, Env* env,
         size_t log_max_size = 0, size_t log_file_time_to_roll = 0,
         const InfoLogLevel log_level = InfoLogLevel::INFO_LEVEL,
         uint64_t flush_every_seconds = 5);

}  // namespace rocksutil

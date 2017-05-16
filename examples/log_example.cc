#include "rocksutil/auto_roll_logger.h"

#include <iostream>

using namespace rocksutil;

/*
 *  Log Level:
 *    InfoLogLevel::DEBUG_LEVEL;
 *    InfoLogLevel::INFO_LEVEL;
 *    InfoLogLevel::WARN_LEVEL;
 *    InfoLogLevel::ERROR_LEVEL;
 *    InfoLogLevel::FATAL_LEVEL;
 *    InfoLogLevel::HEADER_LEVEL;
 *
 *  Log Function:
 *    Debug(log, ...); or LOG(InfoLogLevel::DEBUG_LEVEL, log, ...);
 *    Info(log, ...); or LOG(InfoLogLevel::DEBUG_INFO, log, ...);
 *    Warn(log, ...); or LOG(InfoLogLevel::DEBUG_WARN, log, ...);
 *    Error(log, ...); or LOG(InfoLogLevel::DEBUG_ERROR, log, ...);
 *    Fatal(log, ...); or LOG(InfoLogLevel::DEBUG_FATAL, log, ...);
 *    Header(log, ...); or LOG(InfoLogLevel::DEBUG_HEADER, log, ...);
 *
 *  Usage:
 *    Status CreateLogger(const std::string log_path, std::shared_ptr<Logger>* log,
 *            size_t log_max_size = 0 [never roll],
 *            size_t log_file_time_to_roll = 0 [never roll],
 *            const InfoLogLevel = InfoLogLevel::INFO_LEVEL);
 */


std::atomic<bool> exit_flag(false);

void WriteLog(std::shared_ptr<Logger>log, int thread_num) {
  while(exit_flag.load(std::memory_order_acquire) == false) {
    Info(log, "Thread %d write a log\n", thread_num);
  }
}

int main() {

  std::shared_ptr<Logger> log;
  // 1. Default logger: never roll log file, log_level = InfoLogLevel::INFO_LEVEL;
  Status s = CreateLogger("./log_path", &log);

  // 2. logger: roll to next file per 5s, log_level = InfoLogLevel::WARN_LEVEL;
  // Status s = CreateLogger("tmp", &log, 0, 5, InfoLogLevel::WARN_LEVEL);
  
  // 3. logger roll to next file per 100M, log_level = InfoLogLevel::ERROR_LEVEL;
  // Status s = CreateLogger("tmp", &log, 1024*1024*100, 0, InfoLogLevel::WARN_LEVEL);
  std::cout << s.ToString() << std::endl;

  Header(log, "-------------------------------------------------------------------");
  Header(log, "|                                                                 |");
  Header(log, "| This is the header of log, it will be printed in the header of  |");
  Header(log, "| every log file.                                                 |");
  Header(log, "|                                                                 |");
  Header(log, "-------------------------------------------------------------------");

  std::thread t1 = std::thread(WriteLog, log, 1);
  std::thread t2 = std::thread(WriteLog, log, 2);

  getchar();
  exit_flag.store(true, std::memory_order_release);

  t1.join();
  t2.join();

  return 0;
}

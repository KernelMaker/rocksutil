#include "rocksutil/log_writer.h"
#include "rocksutil/log_reader.h"
#include "rocksutil/file_reader_writer.h"
#include "rocksutil/mutexlock.h"
#include "rocksutil/slice.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

/*
 * Usage:
 *  1. Writer(unique_ptr<WritableFileWriter>&& dest,
 *        uint64_t log_number = 0,
 *        bool recycle_log_files = false);
 *      
 *     Status AddRecord(const Slice& slice);
 *
 *  2. Reader(unique_ptr<SequentialFileReader>&& file,
 *        Reporter* reporter, bool checksum, uint64_t initial_offset,
 *        uint64_t log_num = 0);
 *
 *     bool ReadRecord(Slice* record, std::string* scratch,
 *                WALRecoveryMode wal_recovery_mode =
 *                    WALRecoveryMode::kTolerateCorruptedTailRecords);
 *
 */
using namespace rocksutil;

Env* env = Env::Default();
EnvOptions env_options;
port::Mutex mutex;
port::CondVar cv(&mutex);
std::atomic<bool> should_exit(false);
uint64_t writer_offset = 0;
uint64_t reader_offset = 0;

void WriterFunc() {
/*
 * 1. Create a log::Writer: WritableFile -> WritableFileWriter ->
 *        log::Writer
 *    NOTICE: log::Writer would truncate the walfile to 0;
 */
  unique_ptr<WritableFile> writable_file;
  Status s = NewWritableFile(env, "./wal_file", &writable_file, env_options);
  if (!s.ok()) {
    std::cout << "NewWritableFile Error: " << s.ToString() << std::endl;
    return;
  }
  unique_ptr<WritableFileWriter> writer(
               new WritableFileWriter(std::move(writable_file), env_options));
  log::Writer log_writer(std::move(writer));

  uint64_t i = 0;
  while (!should_exit) {
    {
      MutexLock l(&mutex);
    s = log_writer.AddRecord("WAL_" + std::to_string(i));
    if (!s.ok()) {
      std::cout << "AddRecord Error: " << s.ToString() << std::endl;
    } else {
      i++;
      writer_offset = log_writer.file()->GetFileSize();
      cv.SignalAll();
    }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void ReaderFunc() {
/*
 * 2. Create a log::Reader: SequentialFile -> SequentialFileReader ->
 *      log::Reader
 */
  unique_ptr<SequentialFile> sequential_file;
  Status s;
  s = NewSequentialFile(env, "./wal_file", &sequential_file, env_options);
  if (!s.ok()) {
    std::cout << "NewSequentialFile Error: " << s.ToString() << std::endl;
    return;
  }
  unique_ptr<SequentialFileReader> sequential_reader(
               new SequentialFileReader(std::move(sequential_file)));
  
  // if you want to get the inner error in ReadRecord,
  // construct a log::Reader::LogReporter and pass its
  // address to the log::Reader Construction, otherwise,
  // just pass nullptr;
  log::Reader::LogReporter reporter;
  reporter.status = &s;

  log::Reader log_reader(std::move(sequential_reader), &reporter,
            true, 0);
  std::string scratch;
  Slice record;
//  std::cout << log_reader.IsEOF() << std::endl;
  bool ret = false;
  while (!should_exit) {
    ret = log_reader.ReadRecord(&record, &scratch);
    if (ret) {
      std::cout << std::string(record.data(), record.size()) << std::endl;
    } else {
      if (s.ok()) {
        mutex.Lock();
        reader_offset = log_reader.EndOfBufferOffset();
        while (writer_offset == reader_offset) {
          cv.Wait();
          if (should_exit) {
            mutex.Unlock();
            return;
          }
        }
        mutex.Unlock();
        log_reader.UnmarkEOF();
      } else {
        std::cout << "log_reader error: " << s.ToString() << std::endl;
        return;
      }
    }
  }
}

int main() {
  std::thread writer(WriterFunc);
  std::thread reader(ReaderFunc);
  getchar();
  should_exit = true;
  cv.SignalAll();
  writer.join();
  reader.join();
  return 0;
}

#include "rocksutil/log_writer.h"
#include "rocksutil/log_reader.h"
#include "rocksutil/file_reader_writer.h"

#include <iostream>

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

int main() {
  Env* env = Env::Default();
  EnvOptions env_options;
/*
 * 1. Create a log::Writer: WritableFile -> WritableFileWriter ->
 *        log::Writer
 *    NOTICE: log::Writer would truncate the walfile to 0;
 */
  unique_ptr<WritableFile> writable_file;
  Status s = NewWritableFile(env, "./wal_file", &writable_file, env_options);
  if (!s.ok()) {
    std::cout << "NewWritableFile Error: " << s.ToString() << std::endl;
  }
  unique_ptr<WritableFileWriter> writer(
               new WritableFileWriter(std::move(writable_file), env_options));
  log::Writer log_writer(std::move(writer));
  // Write 100 records;
  for (int i = 0; i < 100; i++) {
    s = log_writer.AddRecord("WAL_" + std::to_string(i));
    if (!s.ok()) {
      std::cout << "AddRecord Error: " << s.ToString() << std::endl;
      return -1;
    }
  }
/*
 * 2. Create a log::Reader: SequentialFile -> SequentialFileReader ->
 *      log::Reader
 */
  unique_ptr<SequentialFile> sequential_file;
  s = NewSequentialFile(env, "./wal_file", &sequential_file, env_options);
  if (!s.ok()) {
    std::cout << "NewSequentialFile Error: " << s.ToString() << std::endl;
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
  std::cout << log_reader.IsEOF() << std::endl;
  while (log_reader.ReadRecord(&record, &scratch) && s.ok()) {
    std::cout << std::string(record.data(), record.size()) <<
      " " << log_reader.LastRecordOffset() << " " <<
      log_reader.EndOfBufferOffset() << std::endl;
  }
  std::cout << log_reader.IsEOF() << " " << s.ToString() << std::endl;
  return 0;
}

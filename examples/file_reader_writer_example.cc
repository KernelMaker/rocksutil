#include "rocksutil/file_reader_writer.h"

#include <iostream>
/*
 * Usage:
 * 1. WritableFileWriter
 * 2. RandomAccessFileReader
 * 3. SequenialFileReader
 * 4. ReadaheadRandomAccessFile
 *
 * use EnvOptions to tune the writer & reader:
 *
 *   If true, then allow caching of data in environment buffers
 *    bool use_os_buffer = true;
 *
 *   If true, then use mmap to read data
 *    bool use_mmap_reads = false;
 *
 *   If true, then use mmap to write data
 *    bool use_mmap_writes = true;
 *
 *   If true, then use O_DIRECT for reading data
 *    bool use_direct_reads = false;
 *
 *   If true, then use O_DIRECT for writing data
 *    bool use_direct_writes = false;
 *
 *   If false, fallocate() calls are bypassed
 *    bool allow_fallocate = true;
 *
 *   If true, set the FD_CLOEXEC on open fd.
 *    bool set_fd_cloexec = true;
 *
 *   Allows OS to incrementally sync files to disk while they are being
 *   written, in the background. Issue one request for every bytes_per_sync
 *   written. 0 turns it off.
 *   Default: 0
 *    uint64_t bytes_per_sync = 0;
 *
 *   If true, we will preallocate the file with FALLOC_FL_KEEP_SIZE flag, which
 *   means that file size won't change as part of preallocation.
 *   If false, preallocation will also change the file size. This option will
 *   improve the performance in workloads where you sync the data on every
 *   write. By default, we set it to true for MANIFEST writes and false for
 *   WAL writes
 *    bool fallocate_with_keep_size = true;
 *
 *    size_t writable_file_max_buffer_size = 1024 * 1024;
 *
 */
using namespace rocksutil;

int main() {
  Env* env = Env::Default();
  EnvOptions env_options;
/*
 * 1. Write [0 - 10000] (int32_t) to tmp_file sequentially 
 */
  // 1-1. Create a WritableFile and use it to Create a WritableFileWriter 
  unique_ptr<WritableFile> writable_file;
  Status s = NewWritableFile(env, "./tmp_file", &writable_file, env_options);
  if (!s.ok()) {
    std::cout << "NewWritableFile Error: " << s.ToString() << std::endl;
  }
  unique_ptr<WritableFileWriter> writer(
               new WritableFileWriter(std::move(writable_file), env_options)); 

  // 1-2. Append sequentially 
  for (int32_t i = 0; i < 10000; i++) {
    s = writer->Append(Slice((char*)&i, sizeof(i)));
    if (!s.ok()) {
      std::cout << "Append Error: " << s.ToString() << std::endl;
    }
  }
  uint64_t file_size = writer->GetFileSize();
  std::cout << "[WritableFileWriter]" << std::endl;
  std::cout << "file_size: " << file_size << std::endl;

  // 1-3. Flush buffered data to OS buffer 
  s = writer->Flush();
  if (!s.ok()) {
    std::cout << "Flush Error: " << s.ToString() << std::endl;
  }

  // 1-4. Sync to disk 
  s = writer->Sync(false);
  if (!s.ok()) {
    std::cout << "Sync Error: " << s.ToString() << std::endl;
  }

  // 1-5. Close the WritableFile 
  s = writer->Close();
  if (!s.ok()) {
    std::cout << "Close Error: " << s.ToString() << std::endl;
  }

/*
 * 2. [Random] Read the 68th num in the tmp_file file, it tends to be 68; 
 */
  // 2-1. Create RandomAccessFile and use it to create a RandomAccessFileReader
  unique_ptr<RandomAccessFile> random_access_file;
  s = NewRandomAccessFile(env, "./tmp_file", &random_access_file, env_options);
  if (!s.ok()) {
    std::cout << "NewRandomAccessFile Error: " << s.ToString() << std::endl;
  }
  unique_ptr<RandomAccessFileReader> random_access_reader(
               new RandomAccessFileReader(std::move(random_access_file)));
  // 2-2. Read the 68th num
  char backing_store[1024];
  Slice result;
  s = random_access_reader->Read(68 * sizeof(int32_t), sizeof(int32_t), &result, backing_store);
  if (!s.ok()) {
    std::cout << "[Random]Read Error: " << s.ToString() << std::endl;
  }
  int32_t num;
  memcpy(&num, result.data(), sizeof(int32_t));
  std::cout << std::endl << "[RandomAccessFileReader]" << std::endl;
  std::cout << num << std::endl;

/*
 * 3. [Sequential] Read the [69-168]th num in the tmp_file file, they tend to be [68-167]
 */
  // 3-1. Create SequentialFile and use it to create a SequenialFileReader
  unique_ptr<SequentialFile> sequential_file;
  s = NewSequentialFile(env, "./tmp_file", &sequential_file, env_options);
  if (!s.ok()) {
    std::cout << "NewSequentialFile Error: " << s.ToString() << std::endl;
  }
  unique_ptr<SequentialFileReader> sequential_reader(
               new SequentialFileReader(std::move(sequential_file)));

  // 3-2. Skip to the 69th num
  s = sequential_reader->Skip(68 * sizeof(int32_t));
  if (!s.ok()) {
    std::cout << "Skip Error: " << s.ToString() << std::endl;
  }
  // 3-3. Sequential Read
  std::cout << std::endl << "[SequentialFileReader]" << std::endl;
  for (int i = 0; i < 100; i++) {
    s = sequential_reader->Read(sizeof(int32_t), &result, backing_store);
    if (!s.ok()) {
      std::cout << "[Sequential]Read Error: " << s.ToString() << std::endl;
      break;
    }
    memcpy(&num, result.data(), sizeof(int32_t));
    std::cout << num  << " ";
  }
  std::cout << std::endl;

/*
 * 4. [Read Ahead Random] Read the 668th & 688th num in the tmp_file file, it tends to be 668 & 688;
 */
  // 4-1. Create a RandomAccessFile and use it to create a [ReadAhead]RandomAccessFile
  s = NewRandomAccessFile(env, "./tmp_file", &random_access_file, env_options);
  if (!s.ok()) {
    std::cout << "NewRandomAccessFile Error: " << s.ToString() << std::endl;
  }
  unique_ptr<RandomAccessFile> readahead_random_access_file(
      NewReadaheadRandomAccessFile(std::move(random_access_file), 1024));
  s = readahead_random_access_file->Read(668 * sizeof(int32_t), sizeof(int32_t), &result, backing_store);
  if (!s.ok()) {
    std::cout << "[RandAhead]Read Error: " << s.ToString() << std::endl;
  }
  memcpy(&num, result.data(), sizeof(int32_t));
  std::cout << std::endl << "[ReadaheadRandomAccessFile]" << std::endl;
  std::cout << num << " ";
  s = readahead_random_access_file->Read(688 * sizeof(int32_t), sizeof(int32_t), &result, backing_store);
  if (!s.ok()) {
    std::cout << "[RandAhead]Read Error: " << s.ToString() << std::endl;
  }
  memcpy(&num, result.data(), sizeof(int32_t));
  std::cout << num << std::endl;

  return 0;
}

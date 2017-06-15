# rocksutil
rocksutil is a ToolKit for C++ developer，It has a lot of useful tools to make coding more faster and easier. It supports LINUX and MACOS platforms

## tools

### 1. Env

Easy way to operate files, directories, threads, times, etc

Here is an [EXAMPLE](https://github.com/KernelMaker/rocksutil/blob/master/examples/thread_pool_example.cc)

Find more usages [Here](https://github.com/KernelMaker/rocksutil/blob/master/include/rocksutil/env.h)

|Files & Directories           |
| --- |
|Env::NewDirectory             |
|Env::FileExists               |
|Env::GetChildren              |
|Env::GetChildrenFileAttributes|
|Env::DeleteFile               |
|Env::CreateDir                |
|Env::CreateDirIfMissing       |
|Env::GetFileSize              |
|Env::GetFileModificationTime  |
|Env::RenameFile               |
|Env::LinkFile                 |
|Env::LockFile                 |
|Env::UnlockFile               |
|Env::GetAbsolutePath          |

---
|Thread & ThreadPool              |
| --- |
|Env::Schedule                    |
|Env::UnSchedule                  |
|Env::StartThread                 |
|Env::WaitForJoin                 |
|Env::GetThreadPoolQueueLen       |
|Env::SetBackgroundThreads        |
|Env::IncBackgroundThreadsIfNeeded|
|Env::LowerThreadPoolIOPriority   |
|Env::GetThreadID                 |

---
|Time & System            |
| --- |
|Env::NowMicros           |
|Env::NowNanos            |
|Env::SleepForMicroseconds|
|Env::GetHostName         |
|Env::GetCurrentTime      |

### 2. Log

Easy way to use Log

Here is an [EXAMPLE](https://github.com/KernelMaker/rocksutil/blob/master/examples/log_example.cc)

### 3. ThreadLocal

Easy way to use thread-specific data

Here is an [EXAMPLE](https://github.com/KernelMaker/rocksutil/blob/master/examples/thread_pool_example.cc)

### 4. MutexLock

Easy way to use Lock, RWLock, SpinLock

Here is an [EXAMPLE](https://github.com/KernelMaker/rocksutil/blob/master/examples/mutexlock_example.cc)

### 5. File Writer & Reader

Easy way to read & write file in random or sequential mode, and use EnvOptions to manipulate
the writer & reader to use [mmap, direct_io, buffer_read_write...]

Here is an [EXAMPLE](https://github.com/KernelMaker/rocksutil/blob/master/examples/file_reader_writer_example.cc)

### 6. Others
Find more info in

[Slice](https://github.com/KernelMaker/rocksutil/blob/master/include/rocksutil/slice.h)

[Coding](https://github.com/KernelMaker/rocksutil/blob/master/include/rocksutil/coding.h)

[Status](https://github.com/KernelMaker/rocksutil/blob/master/include/rocksutil/status.h)

## Where is it from?
This ToolKit is mainly extracted from [rocksdb](https://github.com/facebook/rocksdb), I remove some specific features, change some and make it more
**UNIVERSAL**


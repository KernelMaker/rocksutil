#include "rocksutil/mutexlock.h"

#include <iostream>

using namespace rocksutil;

int main() {
/*
 *  1. MutexLock 
 */
  port::Mutex mutex;
  {
    MutexLock l(&mutex);
    std::cout << "In MutexLock Scope" << std::endl;
  }
/*
 *  2. ReadLock & ReadUnlock 
 */
  port::RWMutex rw_mutex;
  {
    ReadLock l(&rw_mutex);
    std::cout << "In RWMutex ReadLock Scope" << std::endl;
  }
  {
    rw_mutex.ReadLock();
    ReadUnlock l(&rw_mutex);
    std::cout << "In RWMutex ReadLock Scope" << std::endl;
  }
/*
 *  3. WriteLock 
 */
  {
    WriteLock l(&rw_mutex);
    std::cout << "In RWMutex WriteLock Scope" << std::endl;
  }
/*
 *  4. SpinMutex 
 */
  SpinMutex spin_mutex;
  spin_mutex.lock();
  std::cout << "SpinMutex, lock" << std::endl;
  bool ret = spin_mutex.try_lock();
  std::cout << "SpinMutex, try_lock return " << ret << std::endl;
  spin_mutex.unlock();
  std::cout << "SpinMutex, unlock" << std::endl;

  return 0;
}

#include "rocksutil/thread_local.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace rocksutil;

/*
 *  Usage:
 *
 *    1) UnrefHandle:
 *      Cleanup function that will be called for a stored thread local
 *      pointer (if not NULL) when one of the following happens:
 *      (1) a thread terminates
 *      (2) a ThreadLocalPtr is destroyed
 *
 *    2) void* Get():
 *      Return the current pointer stored in thread local
 *
 *    3) void Reset(void* ptr):
 *      Set a new pointer value to the thread local storage.
 *
 *    4) void* Swap(void* ptr):
 *      Atomically swap the supplied ptr and return the previous value
 *
 *    5) bool CompareAndSwap(void* ptr, void*& expected):
 *      Atomically compare the stored value with expected. Set the new
 *      pointer value to thread local only if the comparison is true.
 *      Otherwise, expected returns the stored value.
 *      Return true on success, false on failure
 *
 *    6) void Scrape(autovector<void*>* ptrs, void* const replacement):
 *      Reset all thread local data to replacement, and return non-nullptr
 *      data for all existing threads
 */

class LocalData {
 public:
  LocalData(int first) {
    array = new int[2];
    array[0] = first;
  }
  ~LocalData() {
    std::cout << "Destruct LocalData with First = " << array[0] << std::endl;
    delete[] array;
  }
  int GetFirst() {
    return array[0];
  }
  void SetFirst(int num) {
    array[0] = num;
  }
 private:
  int* array;
};
void UnrefHandle(void* ptr) {
  delete static_cast<LocalData*>(ptr);
}

void VisitLocalData(ThreadLocalPtr* thread_local_ptr, int first) {
  LocalData *p = new LocalData(first);

  void* old_ptr = thread_local_ptr->Swap(p);
  std::cout << "thread " << std::this_thread::get_id() <<
    " Swap, old_ptr = " << old_ptr << std::endl;

  void* cur_ptr = thread_local_ptr->Get();
  std::cout << "thread " << std::this_thread::get_id() <<
    " Get, GetFirst = " << static_cast<LocalData*>(cur_ptr)->GetFirst() <<
    std::endl;
}

int main() {
  ThreadLocalPtr* thread_local_ptr = new ThreadLocalPtr(&UnrefHandle);
  std::thread t1 = std::thread(VisitLocalData, thread_local_ptr, 6);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::thread t2 = std::thread(VisitLocalData, thread_local_ptr, 8);

  t1.join();
  t2.join();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "Bye" << std::endl;

  return 0;
}

#include "rocksutil/env.h"

#include <iostream>
#include <chrono>
#include <thread>

using namespace rocksutil;
/*
 *  Usage: 
 *   
 * Arrange to run "(*function)(arg)" once in a background thread, in
 * the thread pool specified by pri. By default, jobs go to the 'LOW'
 * priority thread pool.
 *
 * 1.
 * "function" may run in an unspecified thread.  Multiple functions
 * added to the same Env may run concurrently in different threads.
 * I.e., the caller may not assume that background work items are
 * serialized.
 * When the UnSchedule function is called, the unschedFunction
 * registered at the time of Schedule is invoked with arg as a parameter.
 * void Schedule(void (*function)(void* arg), void* arg,
 *                       Priority pri = LOW, void* tag = nullptr,
 *                       void (*unschedFunction)(void* arg) = 0);
 *
 * 2.
 * Arrange to remove jobs for given arg from the queue_ if they are not
 * already scheduled. Caller is expected to have exclusive lock on arg.
 * int UnSchedule(void* arg, Priority pri);
 *
 * 3.
 * Start a new thread, invoking "function(arg)" within the new thread.
 * When "function(arg)" returns, the thread will be destroyed.
 * void StartThread(void (*function)(void* arg), void* arg);
 *
 * 4.
 * Wait for all threads started by StartThread to terminate.
 * void WaitForJoin();
 *
 * 5.
 * Get thread pool queue length for specific thrad pool.
 * virtual unsigned int GetThreadPoolQueueLen(Priority pri = LOW); 
 *
 * 6.
 * The number of background worker threads of a specific thread pool
 * for this environment. 'LOW' is the default pool.
 * default number: 1
 * void SetBackgroundThreads(int number, Priority pri = LOW);
 *
 * 7.
 * Enlarge number of background worker threads of a specific thread pool
 * for this environment if it is smaller than specified. 'LOW' is the default
 * pool.
 * void IncBackgroundThreadsIfNeeded(int number, Priority pri);
 * 
 * 8.
 * Lower IO priority for threads from the specified pool.
 * void LowerThreadPoolIOPriority(Priority pool = LOW); 
 */

struct Msg {
  int num;
  std::string str;
};

void HighPrioFunc(void* arg) {
  std::cout << "High Priority[Fast] thread " << std::this_thread::get_id() << 
    " proccess:" << (static_cast<Msg*>(arg))->str << std::endl;
}

void HighPrioSlowFunc(void* arg) {
  std::cout << "High Priority[Slow] thread " << std::this_thread::get_id() << 
    " proccess:" << (static_cast<Msg*>(arg))->num << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void LowPrioFunc(void* arg) {
  std::cout << "Low Priority thread " << std::this_thread::get_id() << 
    " proccess:" << (static_cast<Msg*>(arg))->num << std::endl;
}

int main() {
  Env* env = Env::Default();
  Msg msg = {6, "hello"};
/*
 *  1. Lower the LOW thread priority
 */
  env->LowerThreadPoolIOPriority(Env::Priority::LOW);
/*
 *  2. Schedule
 */
  env->Schedule(&HighPrioFunc, &msg, Env::Priority::HIGH);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  env->Schedule(&LowPrioFunc, &msg, Env::Priority::LOW);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
/*
 *  3. Increase the number of HIGH priority background threads
 *     to 2 TEMPORIRARILY, If you want to Change it PERMANENTLY,
 *     use SetBackgroundThreads(int number, Priority pri);
 */
  env->IncBackgroundThreadsIfNeeded(2, Env::Priority::HIGH);
/*
 *  4. continue Scheduling
 */
  for (int i = 0; i < 50; i++) {
    env->Schedule(&HighPrioSlowFunc, &msg, Env::Priority::HIGH);
    env->Schedule(&HighPrioFunc, &msg, Env::Priority::HIGH);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  // GetThreadPoolQueueLen
  std::cout << "Before Unschedule Queue size: " << env->GetThreadPoolQueueLen(
      Env::Priority::HIGH) << std::endl;
/*
 *  5. should Unschedule
 */
  env->UnSchedule(nullptr, Env::Priority::HIGH);
  std::cout << "After Unschedule Queue size: " << env->GetThreadPoolQueueLen(
      Env::Priority::HIGH) << std::endl;
/*
 *  6. should WaitForJoin 
 */
  env->WaitForJoin();
/*
 *  sleep for a while to wait for all background threads exiting 
 */
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  return 0;
}

#include "rocksdb/env.h"
//#include "rocksdb/status.h"
#include "util/build_version.h"
#include <iostream>

int main() {
  rocksdb::Env* env = rocksdb::Env::Default();
  rocksdb::Status s = env->CreateDirIfMissing("tmp");
  std::cout << s.ToString() << std::endl;
  std::cout << rocksdb_build_git_sha << std::endl;
  return 0;
}

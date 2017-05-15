#include "rocksutil/env.h"
#include "util/build_version.h"
#include <iostream>

int main() {
  rocksutil::Env* env = rocksutil::Env::Default();
  rocksutil::Status s = env->CreateDirIfMissing("tmp");
  std::cout << s.ToString() << std::endl;
  std::cout << rocksutil_build_git_sha << std::endl;
  return 0;
}

#include "rocksdb/env.h"
#include "rocksdb/status.h"

int main() {
  rocksdb::Env* env = rocksdb::Env::Default();
  return 0;
}

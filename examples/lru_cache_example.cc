#include "rocksutil/cache.h"

#include <iostream>

using namespace rocksutil;

  /* Usage:
   * 
   * 1. 
   * Create a new cache with a fixed size capacity. The cache is sharded
   * to 2^num_shard_bits shards, by hash of the key. The total capacity
   * is divided and evenly assigned to each shard. If strict_capacity_limit
   * is set, insert to the cache will fail when cache is full. User can also
   * set percentage of the cache reserves for high priority entries via
   * high_pri_pool_pct.
   *
   * std::shared_ptr<Cache> NewLRUCache(size_t capacity,
   *                                 int num_shard_bits = 6,
   *                                 bool strict_capacity_limit = false,
   *                                 double high_pri_pool_ratio = 0.0);
   *
   * 2. 
   * Insert a mapping from key->value into the cache and assign it
   * the specified charge against the total cache capacity.
   * If strict_capacity_limit is true and cache reaches its full capacity,
   * return Status::Incomplete.
   * 
   * If handle is not nullptr, returns a handle that corresponds to the
   * mapping. The caller must call this->Release(handle) when the returned
   * mapping is no longer needed. In case of error caller is responsible to
   * cleanup the value (i.e. calling "deleter").
   * 
   * If handle is nullptr, it is as if Release is called immediately after
   * insert. In case of error value will be cleanup.
   * 
   * When the inserted entry is no longer needed, the key and
   * value will be passed to "deleter".
   * 
   * virtual Status Insert(const Slice& key, void* value, size_t charge,
   *                       void (*deleter)(const Slice& key, void* value),
   *                       Handle** handle = nullptr,
   *                       Priority priority = Priority::LOW);
   *
   * 3. 
   * If the cache has no mapping for "key", returns nullptr.
   * 
   * Else return a handle that corresponds to the mapping.  The caller
   * must call this->Release(handle) when the returned mapping is no
   * longer needed.
   *
   * virtual Handle* Lookup(const Slice& key);
   *
   * 4. 
   * Release a mapping returned by a previous Lookup().
   * REQUIRES: handle must not have been released yet.
   * REQUIRES: handle must have been returned by a method on *this.
   *
   * virtual void Release(Handle* handle);
   *
   * 5. 
   * Return the value encapsulated in a handle returned by a
   * successful Lookup().
   * REQUIRES: handle must not have been released yet.
   * REQUIRES: handle must have been returned by a method on *this.
   *
   * virtual void* Value(Handle* handle);
   *
   * More Usage in include/rocksutil/cache.h
   */

class Entity {
 public:
  Entity(int n) : num_(n) {
  }
  ~Entity() {
    std::cout << "Destruct Entity " << num_ << std::endl;
  }
  int num() {
    return num_;
  }
 private:
  int num_;
};

void Deleter(const Slice& key, void* value) {
  delete static_cast<Entity*>(value);
}

int main() {


  std::shared_ptr<Cache> lru_cache = NewLRUCache(64);
  Entity* ptr = nullptr;
  Status s;
  Cache::Handle* handles[43];
  for (int i = 0; i < 86; i++) {
    ptr = new Entity(i);
    if (i % 2 == 0) {
      s = lru_cache->Insert(std::to_string(i), ptr, 1,
          &Deleter, &(handles[i/2]));
    } else {
      s = lru_cache->Insert(std::to_string(i), ptr, 1,
          &Deleter);
    }
    std::cout << "Insert key: " << std::to_string(i) << ", return: "
      << s.ToString() << " " << lru_cache->GetCapacity() << " "
      << lru_cache->GetUsage() << " " << lru_cache->GetPinnedUsage() << std::endl;
  }
  getchar();

  for (int i = 0; i < 43; i++) {
    lru_cache->Release(handles[i]);
  }
  getchar();

  Cache::Handle* handle = nullptr;
  for (int i = 0; i < 86; i++) {
    handle = lru_cache->Lookup(std::to_string(i));
    if (handle) {
      std::cout << "Hit, key: " << std::to_string(i) << ", value: "
        << static_cast<Entity*>(lru_cache->Value(handle))->num() << std::endl;
      lru_cache->Release(handle);
    } else {
      std::cout << "Miss key: " << std::to_string(i) << std::endl;
    }
  }
  getchar();

  return 0;
}

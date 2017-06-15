LIB_SOURCES = \
	util/coding.cc \
	util/env.cc \
	util/env_posix.cc \
	util/io_posix.cc \
	util/random.cc \
	util/status.cc \
	util/status_message.cc \
	util/string_util.cc \
	util/thread_local.cc \
	util/threadpool_imp.cc \
	util/build_version.cc \
	util/auto_roll_logger.cc \
	util/sharded_cache.cc \
	util/lru_cache.cc \
	util/hash.cc \
	port/port_posix.cc \
	util/file_reader_writer.cc

EXAMPLE_SOURCE = \
	examples/log_example.cc \
	examples/thread_local_example.cc \
	examples/mutexlock_example.cc \
	examples/thread_pool_example.cc

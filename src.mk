LIB_SOURCES = \
	rutil/coding.cc \
	rutil/env.cc \
	rutil/env_posix.cc \
	rutil/io_posix.cc \
	rutil/random.cc \
	rutil/status.cc \
	rutil/status_message.cc \
	rutil/string_util.cc \
	rutil/thread_local.cc \
	rutil/threadpool_imp.cc \
	rutil/build_version.cc \
	rutil/auto_roll_logger.cc \
	rutil/sharded_cache.cc \
	rutil/lru_cache.cc \
	rutil/hash.cc \
	rutil/file_reader_writer.cc \
	rutil/crc32c.cc \
	rutil/log_writer.cc \
	rutil/log_reader.cc \
	rport/port_posix.cc

EXAMPLE_SOURCE = \
	examples/log_example.cc \
	examples/thread_local_example.cc \
	examples/mutexlock_example.cc \
	examples/thread_pool_example.cc

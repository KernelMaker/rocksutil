CXX = g++

CXXFLAGS = -DROCKSDB_PLATFORM_POSIX -DROCKSDB_LIB_IO_POSIX  -DOS_LINUX -O0 -g -gstabs+ -pg -pipe -fPIC -D__XDEBUG__ -W -Wwrite-strings -Wpointer-arith -Wreorder -Wswitch -Wsign-promo -Wredundant-decls -Wformat -Wall -Wconversion -Wno-unused-parameter -D_GNU_SOURCE -std=c++11

PORT_DIR = ./port
UTIL_DIR = ./util
OUTPUT = ./output

EXAMPLE_PATH = ./example

INCLUDE_PATH = -I. \
			   			 -I./include 

LIBRARY = librocksutil.a

.PHONY: all clean


BASE_OBJS := $(wildcard $(PORT_DIR)/*.cc)
BASE_OBJS += $(wildcard $(PORT_DIR)/*.c)
BASE_OBJS += $(wildcard $(PORT_DIR)/*.cpp)
BASE_OBJS += $(wildcard $(UTIL_DIR)/*.cc)
BASE_OBJS += $(wildcard $(UTIL_DIR)/*.c)
BASE_OBJS += $(wildcard $(UTIL_DIR)/*.cpp)
OBJS = $(patsubst %.cc,%.o,$(BASE_OBJS))

all: $(LIBRARY)
	@echo "Success, go, go, go..."


$(LIBRARY): $(OBJS)
	rm -rf $(OUTPUT)
	mkdir $(OUTPUT)
	mkdir $(OUTPUT)/lib
	rm -rf $@
	ar -rcs $@ $(OBJS)
	mv $(LIBRARY) $(OUTPUT)/lib/
	cp -r ./include $(OUTPUT)
	make -C $(EXAMPLE_PATH)

$(OBJS): %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH) 

$(TOBJS): %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH) 

clean: 
	rm -rf $(PORT_DIR)/*.o
	rm -rf $(UTIL_DIR)/*.o
	rm -rf $(OUTPUT)
	rm -rf $(LIBRARY)
	make clean -C $(EXAMPLE_PATH)


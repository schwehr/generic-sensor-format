# Copyright 2015 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TESTS := index_test
TESTS += read_gsf_2_9_test
TESTS += read_gsf_3_6_test
TESTS += write_test

all: test
	@echo "Done"

clean:
	rm -f $(TESTS) *.a *.o

test: $(TESTS)
	@echo $(TESTS)
	result=0; \
	for test in $(TESTS); do \
	  echo; ./$$test || result=1; \
        done; \
        exit $$result

GTEST_DIR = ../third_party/gtest
GMOCK_DIR = ../third_party/gmock

# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
CPPFLAGS += -isystem $(GMOCK_DIR)/include
CPPFLAGS +=  -I../src
CPPFLAGS += -I$(GTEST_DIR) -I$(GMOCK_DIR)

CXXFLAGS += -std=c++11

CXXFLAGS_DBG += -g -Wall -Wextra 
# CXXFLAGS_DBG += -pthread

SAN +=
SAN := -g -O1
# SAN += -faddress-sanitizer
SAN += -fsanitize=address
# SAN += -fsanitize=memory
# SAN += -fsanitize-memory-track-origins
SAN += -fno-optimize-sibling-calls
SAN += -fno-omit-frame-pointer

ifdef SANITIZE
  # CC:=clang
  # LD:=clang
  # CC := gcc-5
  # CXX := gcc-5
  # LD := gcc-5 ${SAN}
  CXXFLAGS += ${SAN}
endif



GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
		$(GTEST_DIR)/include/gtest/internal/*.h

GMOCK_HEADERS = $(GMOCK_DIR)/include/gmock/*.h \
		$(GMOCK_DIR)/include/gmock/internal/*.h \
		$(GTEST_HEADERS)

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
GMOCK_SRCS_ = $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)


gtest_main.o: $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c \
	    $(GTEST_DIR)/src/gtest_main.cc

gtest.a: gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c \
	    $(GTEST_DIR)/src/gtest-all.cc


gmock-all.o : $(GMOCK_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	    -c $(GMOCK_DIR)/src/gmock-all.cc

gmock_main.o : $(GMOCK_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	    -c $(GMOCK_DIR)/src/gmock_main.cc

gmock.a : gmock-all.o gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gmock_main.a : gmock-all.o gtest-all.o gmock_main.o
	$(AR) $(ARFLAGS) $@ $^

libgsftest.a: gsf_test_util.o
	ar rv $@ $?
	ranlib $@

LIBGSF := ../src/gsf.o ../src/gsf_dec.o ../src/gsf_enc.o ../src/gsf_geo.o
LIBGSF += ../src/gsf_indx.o ../src/gsf_info.o

gsf_test_util.o: gsf_test_util.cc gsf_test_util.h
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

index_test: index_test.o gtest_main.a libgsftest.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ ${LIBGSF} -lpthread

read_gsf_2_9_test: read_gsf_2_9_test.o gsf_test_util.o gtest-all.o gtest_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ ${LIBGSF} -lpthread

read_gsf_3_6_test: read_gsf_3_6_test.o gsf_test_util.o gtest-all.o gtest_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ ${LIBGSF} -lpthread

write_test: write_test.o gsf_test_util.o gtest-all.o gtest_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ ${LIBGSF} -lpthread

gsf_info: gsf_info.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ ../src/libgsf.a

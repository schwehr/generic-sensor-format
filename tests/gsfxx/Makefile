# Copyright 2015 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include ../../Makefile.inc

TESTS :=
TESTS += records_test
TESTS += util_test

all: test
	echo "Done with gsfxx testing."

clean:
	rm -rf $(TESTS) *.a *.o *.gsf *.dSYM

test: $(TESTS)
	@echo $(TESTS)
	result=0; \
	for test in $(TESTS); do \
	  echo; ./$$test || result=1; \
        done; \
        exit $$result

GTEST_DIR = ../../third_party/gtest
GMOCK_DIR = ../../third_party/gmock

# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include
CPPFLAGS += -isystem $(GMOCK_DIR)/include
CPPFLAGS += -I../../src/gsfxx
CPPFLAGS += -I$(GTEST_DIR) -I$(GMOCK_DIR)

LDFLAGS := -L../../src/gsfxx -lgsfxx
# TODO(schwehr): Move the gtest build stuff to its own directory.
LDFLAGS += -L. 
# LDFLAGS += -lgtest_main
LDFLAGS += -lpthread

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
		$(GTEST_DIR)/include/gtest/internal/*.h

GMOCK_HEADERS = $(GMOCK_DIR)/include/gmock/*.h \
		$(GMOCK_DIR)/include/gmock/internal/*.h \
		$(GTEST_HEADERS)

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
GMOCK_SRCS_ = $(GMOCK_DIR)/src/*.cc $(GMOCK_HEADERS)


gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c \
	    $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

libgtest_main.a : gtest-all.o gtest_main.o
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

util_test: util_test.o libgtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

records_test: records_test.o libgtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)


SRCS := ${wildcard *.cc}
OBJS := ${SRCS:.cc=.o}

obj: ${OBJS}

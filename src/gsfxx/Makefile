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

SRCS := $(wildcard *.cc)
OBJS := ${SRCS:.cc=.o}
LIB := libgsfxx.a

all: ${LIB}

obj: ${OBJS}

clean:
	-rm -f *.o
	-rm -f ${LIB}

${LIB}: ${OBJS}
	ar rv $@ $?
	ranlib $@

records.o: records.cc gsfxx.h
util.o: util.cc gsfxx.h

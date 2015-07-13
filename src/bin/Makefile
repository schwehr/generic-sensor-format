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

BIN := gsf_index

WARN := -Wall
WARN += -Wextra
# WARN += -Werror

DBG := -g3 -DDEBUG=1
DBG += -O0

OPT := -O3
OPT += -funroll-loops
OPT += -fexpensive-optimizations
OPT += -ffast-math
OPT += -DNDEBUG

CXXFLAGS := ${WARN}
CXXFLAGS += -std=c++11

CPPFLAGS := -I../gsf -I../gsfxx
LDFLAGS := -L../gsfxx

ifdef OPTIMIZE
  CXXFLAGS += ${OPT}
else
  CXXFLAGS += ${DBG}
endif


all: ${BIN}
	@echo "Done"

clean:
	rm -f $(BIN) *.a *.o

gsf_index: gsf_index.o ../gsf/libgsf.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ 

gsf_info: gsf_info.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ ${LDFLAGS} -lgsfxx

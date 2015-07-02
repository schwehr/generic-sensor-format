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

WARN := -Wall
WARN += -Wextra
# WARN += -Werror

DBG := -g3 -DDEBUG=1
DBG += -O0

OPT := -O3
OPT += -funroll-loops -fexpensive-optimizations -ffast-math
OPT += -DNDEBUG

# asan and msan
SAN := -g -O1
# SAN += -faddress-sanitizer
SAN += -fsanitize=address
# SAN += -fsanitize=memory
# SAN += -fsanitize-memory-track-origins
SAN += -fno-optimize-sibling-calls
SAN += -fno-omit-frame-pointer

ifdef DEBUG
  CFLAGS += ${DBG}
endif
ifdef OPTIMIZE
  CFLAGS += ${OPT}
endif
ifdef SANITIZE
  # CC:=clang
  # LD:=clang
  # CC := gcc-5
  # LD := gcc-5
  CFLAGS += ${SAN}
endif

CFLAGS += ${WARN}

SRCS := $(wildcard *.c)
OBJS := ${SRCS:.c=.o}
LIB := libgsf.a

${LIB}: ${OBJS}
	ar rv $@ $?
	ranlib $@

all: ${LIB}

clean:
	-rm -f *.o
	-rm -f ${LIB}

HDRS := gsf.h gsf_ft.h gsf_dec.h gsf_enc.h gsf_indx.h
gsf.o: gsf.c ${HDRS}
gsf_dec.o: gsf_dec.c ${HDRS}
gsf_enc.o: gsf_enc.c ${HDRS}
gsf_geo.o: gsf_geo.c ${HDRS}
gsf_indx.o: gsf_indx.c ${HDRS}
gsf_info.o: gsf_info.c ${HDRS}

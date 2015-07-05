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

# CC := gcc-5

WARN := -Wall
WARN += -Wextra
# WARN += -Werror

DBG := -g3
# DBG += -DDEBUG
DBG += -O0

OPT := -O3
OPT += -funroll-loops
OPT += -fexpensive-optimizations
OPT += -ffast-math
OPT += -DNDEBUG

CFLAGS := ${WARN}

ifdef OPTIMIZE
  CFLAGS += ${OPT}
else
  CFLAGS += ${DBG}
endif


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

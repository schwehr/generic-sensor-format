# -*- makefile -*-

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

DEBUG := -g3 -DDEBUG=1
DEBUG += -O0

OPT := -O3
OPT += -funroll-loops -fexpensive-optimizations -ffast-math
OPT += -DNDEBUG

ifdef OPTIMIZE
  CCFLAGS += ${OPT}
else
  CCFLAGS += ${DEBUG}
endif

CCFLAGS += ${WARN}

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

gsf.o: gsf.c gsf.h gsf_ft.h gsf_dec.h gsf_enc.h gsf_indx.h
gsf_dec.c: gsf.h gsf_dec.h gsf_enc.h
gsf_enc.c: gsf.h gsf_enc.h
gsf_geo.o: gsf.h
gsf_indx.c: gsf.h gsf_ft.h gsf_indx.h
gsf_info.c: gsf.h

# -*- makefile -*-

# Copyright 2015 Google Inc. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

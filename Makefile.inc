# -*- makefile -*-

ifdef USE_GCC
CC := gcc-5
CXX := g++-5
else
CC := clang
CXX := clang++
endif

WARN :=
WARN := -Wall
WARN += -Wextra

ifdef USE_GCC
WARN += -Wswitch-default
WARN += -Wswitch-enum
endif

ifdef ERROR
  WARN += -Werror
endif

CFLAGS := ${WARN}

CXXFLAGS += -std=c++11
# CXXFLAGS += -pthread
CXXFLAGS += ${WARN}

DBG := -g
DBG += -O

OPT := -O3
OPT += -funroll-loops
# Not supposed by clang.
# OPT += -fexpensive-optimizations
OPT += -ffast-math
OPT += -DNDEBUG

ifdef OPTIMIZE
  CFLAGS += ${OPT}
  CXXFLAGS += ${OPT}
else
  CFLAGS += ${DBG}
  CXXFLAGS += ${DBG}
endif


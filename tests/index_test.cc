// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Tests for writing out the basic record types.

#include <cstdio>
#include <sys/stat.h>

#include <memory>
#include <string>

#include "gsf.h"
#include "gsf_ft.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

using std::string;
using std::unique_ptr;

namespace generic_sensor_format {
namespace test {
namespace {

void CheckFileEqual(const string &filename_old,
                    const string &filename_new) {
  struct stat stat_old;
  ASSERT_EQ(0, stat(filename_old.c_str(), &stat_old));
  struct stat stat_new;
  ASSERT_EQ(0, stat(filename_new.c_str(), &stat_new));
  ASSERT_EQ(stat_old.st_size, stat_new.st_size);

  const size_t size = stat_new.st_size;

  // TODO(schwehr): Switch to make_unique with C++14.
  unique_ptr<unsigned char[]> buf_old(new unsigned char[size]());
  FILE *file_old = fopen(filename_old.c_str(), "r");
  ASSERT_NE(nullptr, file_old);
  ASSERT_EQ(size, fread(buf_old.get(), 1, size, file_old));

  unique_ptr<unsigned char[]> buf_new(new unsigned char[size]());
  FILE *file_new = fopen(filename_new.c_str(), "r");
  ASSERT_NE(nullptr, file_new);
  ASSERT_EQ(size, fread(buf_new.get(), 1, size, file_new));

  for (size_t i=0; i < size; ++i) {
    ASSERT_EQ(buf_old[i], buf_new[i]);
  }
}

TEST(GsfIndex, Comment) {
  // TempDir
  // cp gsf to tempdir
  int handle;
  string filename = "../data/single-record/comment.gsf";
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_UPDATE_INDEX, &handle));
  ASSERT_EQ(0, gsfClose(handle));

  string filename_old_index = "../data/single-record/comment.orig.nsf";
  string filename_new_index = "../data/single-record/comment.nsf";

  CheckFileEqual(filename_old_index, filename_new_index);
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

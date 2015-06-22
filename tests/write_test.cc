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

#include <string>

#include "gsf.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

namespace generic_sensor_format {
namespace test {
namespace {

TEST(GsfWriteSimple, HeaderOnly) {
  int handle;
  char filename[] = "sample01-header-only.gsf";
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));
  ASSERT_EQ(0, gsfClose(handle));
  struct stat buf;
  ASSERT_EQ(0, stat(filename, &buf));
  ASSERT_EQ(24, buf.st_size);
}

TEST(GsfWriteSimple, CommentEmpty) {
  int handle;
  ASSERT_EQ(0, gsfOpen("sample-comment-empty.gsf", GSF_CREATE, &handle));

  gsfDataID data_id = {false, 0, GSF_RECORD_COMMENT, 0};
  gsfRecords record;
  record.comment.comment_time.tv_sec = 1;
  record.comment.comment_time.tv_nsec = 2;
  char comment[] = "";
  record.comment.comment_length = strlen(comment);
  record.comment.comment = comment;
  ASSERT_EQ(20, gsfWrite(handle, &data_id, &record));

  ASSERT_EQ(0, gsfClose(handle));
}

TEST(GsfWriteSimple, CommentEmptyWithChecksum) {
  int handle;
  ASSERT_EQ(0, gsfOpen("sample-comment-empty.gsf", GSF_CREATE, &handle));

  gsfDataID data_id = {true, 0, GSF_RECORD_COMMENT, 0};
  gsfRecords record;
  record.comment.comment_time.tv_sec = 1;
  record.comment.comment_time.tv_nsec = 2;
  char comment[] = "";
  record.comment.comment_length = strlen(comment);
  record.comment.comment = comment;
  ASSERT_EQ(24, gsfWrite(handle, &data_id, &record));

  ASSERT_EQ(0, gsfClose(handle));
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

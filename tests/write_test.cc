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

#include <sys/stat.h>
#include <string>

#include "gsf.h"
#include "gsf_ft.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

namespace generic_sensor_format {
namespace test {
namespace {

TEST(GsfWriteSimple, HeaderOnly) {
  int handle;
  char filename[] = "sample-header-only.gsf";
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));
  ASSERT_EQ(0, gsfClose(handle));
  struct stat buf;
  ASSERT_EQ(0, stat(filename, &buf));
  ASSERT_EQ(24, buf.st_size);
}

void ValidateWriteComment(const char *filename,
                          bool checksum,
                          int expected_write_size,
                          const char *comment,
                          int expected_file_size) {
  ASSERT_NE(nullptr, filename);
  ASSERT_GE(expected_write_size, 20);
  ASSERT_NE(nullptr, comment);
  ASSERT_GE(expected_file_size, 44);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_COMMENT, 0};
  gsfRecords record;
  record.comment.comment_time.tv_sec = 1;
  record.comment.comment_time.tv_nsec = 2;
  record.comment.comment_length = strlen(comment);
  record.comment.comment = const_cast<char *>(comment);
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));

  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename, &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename, GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes
      = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_COMMENT, data_id.recordID);
  VerifyComment(record.comment, read_record.comment);
}

TEST(GsfWriteSimple, CommentEmpty) {
  char filename[] = "sample-comment-empty.gsf";
  char comment[] = "";
  ValidateWriteComment(filename, false, 20, comment, 44);
}

TEST(GsfWriteSimple, CommentEmptyChecksum) {
  char filename[] = "sample-comment-empty-checksum.gsf";
  char comment[] = "";
  ValidateWriteComment(filename, true, 24, comment, 48);
}

TEST(GsfWriteSimple, CommentUpTo5) {
  char filename1[] = "sample-comment-1.gsf";
  char comment1[] = "a";
  ValidateWriteComment(filename1, false, 24, comment1, 48);
  char filename2[] = "sample-comment-2.gsf";
  char comment2[] = "ab";
  ValidateWriteComment(filename2, false, 24, comment2, 48);
  char filename3[] = "sample-comment-3.gsf";
  char comment3[] = "abc";
  ValidateWriteComment(filename3, false, 24, comment3, 48);
  char filename4[] = "sample-comment-4.gsf";
  char comment4[] = "abcd";
  ValidateWriteComment(filename4, false, 24, comment4, 48);
  char filename5[] = "sample-comment-5.gsf";
  char comment5[] = "abcde";
  ValidateWriteComment(filename5, false, 28, comment5, 52);
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

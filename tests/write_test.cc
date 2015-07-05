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

// TODO(schwehr): GSF_RECORD_SWATH_BATHYMETRY_PING.
// TODO(schwehr): GSF_RECORD_SOUND_VELOCITY_PROFILE
// TODO(schwehr): GSF_RECORD_PROCESSING_PARAMETERS
// TODO(schwehr): GSF_RECORD_SENSOR_PARAMETERS

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
  record.comment = GsfComment({1, 2}, comment);
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

TEST(GsfWriteSimple, CommentLarge) {
  char filename[] = "sample-comment-large.gsf";
  char comment[] =
    "ab"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
    "de";
  ValidateWriteComment(filename, false, 656, comment, 680);
}

void ValidateWriteHistory(const char *filename,
                          bool checksum,
                          int expected_write_size,
                          const char *host_name,
                          const char *operator_name,
                          const char *command_line,
                          const char *comment,
                          int expected_file_size) {
  ASSERT_NE(nullptr, filename);
  ASSERT_GE(expected_write_size, 28);
  ASSERT_NE(nullptr, host_name);
  ASSERT_LE(strlen(host_name), GSF_HOST_NAME_LENGTH);
  ASSERT_NE(nullptr, operator_name);
  ASSERT_LE(strlen(operator_name), GSF_OPERATOR_LENGTH);
  ASSERT_NE(nullptr, command_line);
  ASSERT_NE(nullptr, comment);
  ASSERT_GE(expected_file_size, 52);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_HISTORY, 0};
  gsfRecords record;
  record.history
      = GsfHistory({1, 2}, host_name, operator_name, command_line, comment);
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
  ASSERT_EQ(GSF_RECORD_HISTORY, data_id.recordID);
  VerifyHistory(record.history, read_record.history);
}

TEST(GsfWriteSimple, History) {
  ValidateWriteHistory(
      "sample-history-empty.gsf", false, 28, "", "", "", "", 52);
  ValidateWriteHistory(
      "sample-history-empty-checksum.gsf", true, 32, "", "", "", "", 56);
  ValidateWriteHistory(
      "sample-history-1.gsf", false, 32, "a", "b", "c", "d", 56);
  ValidateWriteHistory(
      "sample-history-longer.gsf", false, 40, "ab", "cde", "fghi", "jklm", 64);
}

// TODO(schwehr): GSF_RECORD_NAVIGATION_ERROR
// TODO(schwehr): GSF_RECORD_SWATH_BATHY_SUMMARY
// TODO(schwehr): GSF_RECORD_SINGLE_BEAM_PING
// TODO(schwehr): GSF_RECORD_HV_NAVIGATION_ERROR
// TODO(schwehr): GSF_RECORD_ATTITUDE

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

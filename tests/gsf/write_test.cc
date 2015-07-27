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
//
// TODO(schwehr): Does this have to free what was read?

#include <sys/stat.h>

#include <memory>
#include <string>

#include "gsf.h"
#include "gsf_ft.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

// #include <iostream>  // TODO(schwehr): Remove.
// using namespace std;  // TODO(schwehr): Remove.

using std::unique_ptr;

namespace generic_sensor_format {
namespace test {
namespace {

// TODO(schwehr): Switch to make_unique when C++14 is available on Travis-CI.
template <typename T, typename... Args>
std::unique_ptr<T> MakeUnique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

TEST(GsfWriteSimple, HeaderOnly) {
  int handle;
  char filename[] = "header-only.gsf";
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));
  ASSERT_EQ(0, gsfClose(handle));
  struct stat buf;
  ASSERT_EQ(0, stat(filename, &buf));
  ASSERT_EQ(20, buf.st_size);
}

// TODO(schwehr): GSF_RECORD_SWATH_BATHYMETRY_PING.
// TODO(schwehr): GSF_RECORD_SOUND_VELOCITY_PROFILE
// TODO(schwehr): GSF_RECORD_PROCESSING_PARAMETERS
// TODO(schwehr): GSF_RECORD_SENSOR_PARAMETERS

void ValidateWriteComment(const char *filename, bool checksum,
                          int expected_write_size, const char *comment,
                          int expected_file_size) {
  ASSERT_NE(nullptr, filename);
  ASSERT_GE(expected_write_size, 20);
  ASSERT_NE(nullptr, comment);
  ASSERT_GE(expected_file_size, 40);

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
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_COMMENT, data_id.recordID);
  VerifyComment(record.comment, read_record.comment);
}

TEST(GsfWriteSimple, CommentEmpty) {
  char filename[] = "comment-empty.gsf";
  char comment[] = "";
  ValidateWriteComment(filename, false, 20, comment, 40);
}

TEST(GsfWriteSimple, CommentEmptyChecksum) {
  char filename[] = "comment-empty-checksum.gsf";
  char comment[] = "";
  ValidateWriteComment(filename, true, 24, comment, 44);
}

TEST(GsfWriteSimple, CommentUpTo5) {
  char filename1[] = "comment-1.gsf";
  char comment1[] = "a";
  ValidateWriteComment(filename1, false, 24, comment1, 44);
  char filename2[] = "comment-2.gsf";
  char comment2[] = "ab";
  ValidateWriteComment(filename2, false, 24, comment2, 44);
  char filename3[] = "comment-3.gsf";
  char comment3[] = "abc";
  ValidateWriteComment(filename3, false, 24, comment3, 44);
  char filename4[] = "comment-4.gsf";
  char comment4[] = "abcd";
  ValidateWriteComment(filename4, false, 24, comment4, 44);
  char filename5[] = "comment-5.gsf";
  char comment5[] = "abcde";
  ValidateWriteComment(filename5, false, 28, comment5, 48);
}

TEST(GsfWriteSimple, CommentLarge) {
  char filename[] = "comment-large.gsf";
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
  ValidateWriteComment(filename, false, 656, comment, 676);
}

void ValidateWriteHistory(const char *filename, bool checksum,
                          int expected_write_size, const char *host_name,
                          const char *operator_name, const char *command_line,
                          const char *comment, int expected_file_size) {
  ASSERT_NE(nullptr, filename);
  ASSERT_GE(expected_write_size, 28);
  ASSERT_NE(nullptr, host_name);
  ASSERT_LE(strlen(host_name), GSF_HOST_NAME_LENGTH);
  ASSERT_NE(nullptr, operator_name);
  ASSERT_LE(strlen(operator_name), GSF_OPERATOR_LENGTH);
  ASSERT_NE(nullptr, command_line);
  ASSERT_NE(nullptr, comment);
  ASSERT_GE(expected_file_size, 48);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_HISTORY, 0};
  gsfRecords record;
  record.history =
      GsfHistory({1, 2}, host_name, operator_name, command_line, comment);
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename, &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename, GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_HISTORY, data_id.recordID);
  VerifyHistory(record.history, read_record.history);
}

TEST(GsfWriteSimple, History) {
  ValidateWriteHistory("history-empty.gsf", false, 28, "", "", "", "", 48);
  ValidateWriteHistory("history-empty-checksum.gsf", true, 32, "", "", "", "",
                       52);
  ValidateWriteHistory("history-1.gsf", false, 32, "a", "b", "c", "d", 52);
  ValidateWriteHistory("history-longer.gsf", false, 40, "ab", "cde", "fghi",
                       "jklm", 60);
}

// TODO(schwehr): GSF_RECORD_NAVIGATION_ERROR
// TODO(schwehr): GSF_RECORD_SWATH_BATHY_SUMMARY
// TODO(schwehr): GSF_RECORD_SINGLE_BEAM_PING
// TODO(schwehr): GSF_RECORD_HV_NAVIGATION_ERROR

void ValidateWriteAttitude(const char *filename, bool checksum,
                           int expected_write_size, const gsfAttitude &attitude,
                           int expected_file_size) {
  ASSERT_NE(nullptr, filename);
  ASSERT_GE(expected_write_size, 20);
  ASSERT_GE(expected_file_size, 40);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename, GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_ATTITUDE, 0};
  gsfRecords record;
  record.attitude = attitude;
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename, &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename, GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_ATTITUDE, data_id.recordID);
  VerifyAttitude(record.attitude, read_record.attitude);
}

unique_ptr<gsfAttitude> WriteAttitudeAndReturnRead(const char *filename,
                                                   const gsfAttitude &attitude,
                                                   int *file_size,
                                                   int *num_bytes) {
  // TODO(schwehr): Add error handling.
  int handle;
  if (0 != gsfOpen(filename, GSF_CREATE, &handle)) {
    return nullptr;
  }

  gsfDataID data_id = {false, 0, GSF_RECORD_ATTITUDE, 0};
  gsfRecords record;
  record.attitude = attitude;
  if (gsfWrite(handle, &data_id, &record) < 20) {
    // Do not check return since we are already in trouble.
    gsfClose(handle);
    return nullptr;
  }
  if (0 != gsfClose(handle)) {
    return nullptr;
  }

  struct stat buf;
  if (0 != stat(filename, &buf)) {
    return nullptr;
  }
  *file_size = buf.st_size;

  if (0 != gsfOpen(filename, GSF_READONLY, &handle)) {
    return nullptr;
  }

  gsfRecords read_record;
  *num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  if (*num_bytes < 24) {
    return nullptr;
  }

  return MakeUnique<gsfAttitude>(read_record.attitude);
}

TEST(GsfWriteSimple, AttitudeEmpty) {
  const struct timespec times[] = {{3, 4}};
  const gsfAttitude attitude =
      GsfAttitude(0, times, nullptr, nullptr, nullptr, nullptr);
  ValidateWriteAttitude("attitude-empty.gsf", false, 20, attitude, 40);
}

TEST(GsfWriteSimple, AttitudeLength1Zeros) {
  const struct timespec times0[] = {{5, 6}};
  const double data0[] = {0.0};
  const gsfAttitude attitude0 =
      GsfAttitude(1, times0, data0, data0, data0, data0);
  ValidateWriteAttitude("attitude-length1-0.gsf", false, 28, attitude0, 48);
}

TEST(GsfWriteSimple, AttitudeLength1) {
  const struct timespec times[] = {{7, 8}};
  const double pitch[] = {1.2};
  const double roll[] = {2.2};
  const double heave[] = {3.3};
  const double heading[] = {4.4};
  const gsfAttitude attitude =
      GsfAttitude(1, times, pitch, roll, heave, heading);
  ValidateWriteAttitude("attitude-length1-1-checksum.gsf", true, 32, attitude,
                        52);
}

TEST(GsfWriteSimple, AttitudeLength1Negative) {
  const struct timespec times[] = {{9, 10}};
  const double pitch[] = {-1.2};
  const double roll[] = {-3.4};
  const double heave[] = {-5.6};
  // Stored as a int16 with value * 100, so cannot test negative.
  const double heading[] = {7.8};
  const gsfAttitude attitude =
      GsfAttitude(1, times, pitch, roll, heave, heading);
  ValidateWriteAttitude("attitude-length1-neg.gsf", false, 28, attitude, 48);
}

TEST(GsfWriteSimple, AttitudeRounding) {
  const struct timespec times[] = {{11, 12}};
  const double pitch[] = {-1.002};
  const double roll[] = {-2.0052};
  const double heave[] = {3.0045};
  const double heading[] = {4.0052};
  const gsfAttitude src = GsfAttitude(1, times, pitch, roll, heave, heading);
  int file_size;
  int num_bytes;
  const unique_ptr<gsfAttitude> dst = WriteAttitudeAndReturnRead(
      "attitude-round.gsf", src, &file_size, &num_bytes);
  ASSERT_NE(nullptr, dst);
  ASSERT_EQ(48, file_size);
  ASSERT_EQ(28, num_bytes);

  const double pitch2[] = {-1.00};
  const double roll2[] = {-2.01};
  const double heave2[] = {3.00};
  const double heading2[] = {4.01};
  const gsfAttitude expected =
      GsfAttitude(1, times, pitch2, roll2, heave2, heading2);
  VerifyAttitude(expected, *dst);
}

TEST(GsfWriteSimple, AttitudeLength2) {
  // TODO(schwehr): Why do the nanoseconds not match?
  const struct timespec times[] = {{1438016822, 0},
                                   {1438016823, 0}};
  const double pitch[] = {1.2, -3.4};
  const double roll[] = {-100.1, 100.2};
  const double heave[] = {-200.2, -200.3};
  // Stored as a int16 with value * 100, so cannot test negative.
  const double heading[] = {181, 359};
  const gsfAttitude attitude =
      GsfAttitude(2, times, pitch, roll, heave, heading);
  ValidateWriteAttitude("attitude-length2.gsf", false, 40, attitude, 60);
}


}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

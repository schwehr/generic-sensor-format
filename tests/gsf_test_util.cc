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

#include <cstring>

#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gsf.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

// using ::testing::ElementsAre;
// using ::testing::ElementsAreArray;
using std::string;
using std::vector;

#include <iostream>


namespace generic_sensor_format {
namespace test {

string RecordTypeStr(unsigned int record_id) {
  switch (record_id) {
  case GSF_RECORD_HEADER:
    return "GSF_RECORD_HEADER";
  case GSF_RECORD_SWATH_BATHYMETRY_PING:
    return "GSF_RECORD_SWATH_BATHYMETRY_PING";
  case GSF_RECORD_SOUND_VELOCITY_PROFILE:
    return "GSF_RECORD_SOUND_VELOCITY_PROFILE";
  case GSF_RECORD_PROCESSING_PARAMETERS:
    return "GSF_RECORD_PROCESSING_PARAMETERS";
  case GSF_RECORD_SENSOR_PARAMETERS:
    return "GSF_RECORD_SENSOR_PARAMETERS";
  case GSF_RECORD_COMMENT:
    return "GSF_RECORD_COMMENT";
  case GSF_RECORD_HISTORY:
    return "GSF_RECORD_HISTORY";
  case GSF_RECORD_NAVIGATION_ERROR:
    return "GSF_RECORD_NAVIGATION_ERROR";
  case GSF_RECORD_SWATH_BATHY_SUMMARY:
    return "GSF_RECORD_SWATH_BATHY_SUMMARY";
  case GSF_RECORD_SINGLE_BEAM_PING:
    return "GSF_RECORD_SINGLE_BEAM_PING";
  case GSF_RECORD_HV_NAVIGATION_ERROR:
    return "GSF_RECORD_HV_NAVIGATION_ERROR";
  case GSF_RECORD_ATTITUDE:
    return "GSF_RECORD_ATTITUDE";

  }
  return "UNKNOWN";
}

gsfComment GsfComment(const struct timespec &when, const char *comment) {
  gsfComment ret = {
    { when.tv_sec, when.tv_nsec },
    int(strlen(comment)),
    const_cast<char *>(comment)
  };
  return ret;
}

gsfHistory GsfHistory(const struct timespec &when,
                      const char *host_name,
                      const char *operator_name,
                      const char *command_line,
                      const char *comment) {
  gsfHistory history;
  history.history_time = { when.tv_sec, when.tv_nsec };
  strncpy(history.host_name, host_name, GSF_HOST_NAME_LENGTH);
  strncpy(history.operator_name, operator_name,GSF_OPERATOR_LENGTH);
  history.command_line = const_cast<char *>(command_line);
  history.comment = const_cast<char *>(comment);
  return history;
}

void PacketCounts::Verify(const vector<int> &expected) {
  ASSERT_EQ(NUM_REC_TYPES, expected.size());
  for (int i=0; i < NUM_REC_TYPES; ++i) {
    EXPECT_EQ(expected[i], counts_[i])
        << "Mismatch. packet #" << i << " expected: " << expected[i]
        << "  found: " << counts_[i] << "\n";
  }
}

void VerifyDataId(const gsfDataID &expected, const gsfDataID &actual) {
  EXPECT_EQ(expected.checksumFlag, actual.checksumFlag);
  EXPECT_EQ(expected.reserved, actual.reserved);
  EXPECT_EQ(expected.recordID, actual.recordID);
  // TODO(schwehr): When should this number match?
  // EXPECT_EQ(expected.record_number, actual.record_number);
}

void VerifySwathBathySummary(const gsfSwathBathySummary &expected,
                             const gsfSwathBathySummary &actual) {

  EXPECT_EQ(expected.start_time.tv_sec, actual.start_time.tv_sec);
  EXPECT_EQ(expected.start_time.tv_nsec, actual.start_time.tv_nsec);
  EXPECT_EQ(expected.end_time.tv_sec, actual.end_time.tv_sec);
  EXPECT_EQ(expected.end_time.tv_nsec, actual.end_time.tv_nsec);
  EXPECT_DOUBLE_EQ(expected.min_latitude, actual.min_latitude);
  EXPECT_DOUBLE_EQ(expected.min_longitude, actual.min_longitude);
  EXPECT_DOUBLE_EQ(expected.max_latitude, actual.max_latitude);
  EXPECT_DOUBLE_EQ(expected.max_longitude, actual.max_longitude);
  EXPECT_DOUBLE_EQ(expected.min_depth, actual.min_depth);
  EXPECT_DOUBLE_EQ(expected.max_depth, actual.max_depth);
}

void VerifyComment(const gsfComment &expected,
                   const gsfComment &actual) {
  EXPECT_EQ(expected.comment_time.tv_sec, actual.comment_time.tv_sec);
  EXPECT_EQ(expected.comment_time.tv_nsec, actual.comment_time.tv_nsec);
  EXPECT_EQ(expected.comment_length, actual.comment_length);
  EXPECT_STREQ(expected.comment, actual.comment);
}

void VerifyHistory(const gsfHistory &expected,
                   const gsfHistory &actual) {
  EXPECT_EQ(expected.history_time.tv_sec, actual.history_time.tv_sec);
  EXPECT_EQ(expected.history_time.tv_nsec, actual.history_time.tv_nsec);
  EXPECT_STREQ(expected.host_name, actual.host_name);
  EXPECT_STREQ(expected.operator_name, actual.operator_name);
  EXPECT_STREQ(expected.command_line, actual.command_line);
  EXPECT_STREQ(expected.comment, actual.comment);
}

}  // namespace test
}  // namespace generic_sensor_format

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

#include <string>
#include <vector>

#include "gsf.h"
#include "gsf_test_util.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using std::string;
using std::vector;

// TODO(schwehr): Remove iostream.
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

void PacketCounts::Verify(const vector<int> &expected) {
  ASSERT_EQ(NUM_REC_TYPES, expected.size());
  for (int i=0; i < NUM_REC_TYPES; ++i) {
    EXPECT_EQ(expected[i], counts_[i])
        << "Mismatch. packet #" << i << " expected: " << expected[i]
        << "  found: " << counts_[i] << "\n";
  }
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

}  // namespace test
}  // namespace generic_sensor_format

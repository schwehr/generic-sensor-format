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

#include "gmock/gmock.h"
#include "gsf.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using std::string;
// using std::vector;

// TODO(schwehr): Remove iostream.
#include <iostream>

// GSF error value.
// TODO(schwehr):  Why is this not in the header?
extern int gsfError;

namespace generic_sensor_format {
namespace test {
namespace {

char kComment[] = "Bathy converted from HIPS file: M:\\CCOM_Processing\\CARIS_v8\\HIPS\\81\\HDCS_Data\\EX1502L2\\Okeanos_March_2011\\2015-081\\0175_20150322_232639_EX1502L2_MB";

#if 0
class PacketCounts {
 public:
  PacketCounts() : counts_(NUM_REC_TYPES, 0) {}
  void add(int packet_num) { ++counts_[packet_num]; }
  vector<int> counts_;
  void Verify(const vector<int> &expected) {
    ASSERT_EQ(NUM_REC_TYPES, expected.size());
    for (int i=0; i < NUM_REC_TYPES; ++i) {
      EXPECT_EQ(expected[i], counts_[i])
        << "Mismatch. packet #" << i << " expected: " << expected[i]
        << "  found: " << counts_[i] << "\n";
    }
  }
};
#endif

#if 0
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
#endif

TEST(GsfRead2_9Test, CountPackets) {
  int handle;
  ASSERT_EQ(0, gsfOpen("../data/0175_20150322_232639_EX1502L2_MB.gsf.mb121",
                       GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);

  gsfDataID data_id;
  gsfRecords records;

  int num_bytes;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  PacketCounts counts;
  while (num_bytes >= 0) {
    counts.add(data_id.recordID);
    num_bytes
        = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  }
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfError);
  counts.Verify({0, 0, 4, 1, 1, 0, 2, 1, 0, 1, 0, 0, 34});
}

#if 0
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
#endif

TEST(GsfReadTest, ReadVersion2_9) {
  int handle;
  ASSERT_EQ(0, gsfOpen("../data/0175_20150322_232639_EX1502L2_MB.gsf.mb121",
                       GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);

  gsfDataID data_id;
  gsfRecords records;

  int num_bytes;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(48, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(GSF_RECORD_SWATH_BATHY_SUMMARY, data_id.recordID);

  const gsfSwathBathySummary expected =
      {
        {1427066789, 653000116},
        {1427066822, 154999971},
        18.7402008, -67.6184058,
        18.7412846, -67.6178461,
        1459.2, 1709.59};
  VerifySwathBathySummary(expected, records.summary);

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(168, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(6, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_COMMENT, data_id.recordID);

  const gsfComment expected_comment =
    {{1427066789, 653000116}, 147, kComment};
  VerifyComment(expected_comment, records.comment);

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(2236, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(4, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_PROCESSING_PARAMETERS, data_id.recordID);

  // TODO(schwehr): VerifyProcessingParameters().

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(1388, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(3, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_SOUND_VELOCITY_PROFILE, data_id.recordID);

  // TODO(schwehr): VerifySoundVelocityProfile().

  for (int count=0; count < 288; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    // ASSERT_GE(num_bytes, 0);
    if (num_bytes < 1)
      break;
#if 1
    std::cout << "rec: count: " << count
         << " #" << data_id.record_number
         << " id: " << data_id.recordID
         << " size: " << num_bytes
         << "   -->   " << RecordTypeStr(data_id.recordID)
         << "\n";
#endif
  }

}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

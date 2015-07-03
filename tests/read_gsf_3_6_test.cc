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

namespace generic_sensor_format {
namespace test {
namespace {

char kComment[] =
    "Bathy converted from HIPS file: M:\\CCOM_Processing\\CARIS_v8\\HIPS\\81"
    "\\HDCS_Data\\EX1502L2\\Okeanos_March_2011\\2015-081"
    "\\0175_20150322_232639_EX1502L2_MB";

TEST(GsfRead3_6Test, CountPackets) {
  int handle;
  ASSERT_EQ(
      0, gsfOpen("../data/surveys/0175_20150322_232639_EX1502L2_MB.gsf.mb121",
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
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfIntError());
  counts.Verify({0, 0, 4, 1, 1, 0, 2, 1, 0, 1, 0, 0, 34});
}

TEST(GsfReadTest, ReadVersion3_6) {
  int handle;
  ASSERT_EQ(0,
      gsfOpen("../data/surveys/0175_20150322_232639_EX1502L2_MB.gsf.mb121",
              GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);

  gsfDataID data_id;
  gsfRecords records;

  int num_bytes;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(48, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_SWATH_BATHY_SUMMARY, 0}, data_id);

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
  VerifyDataId({false, 0, GSF_RECORD_COMMENT, 0}, data_id);

  const gsfComment expected_comment =
    {{1427066789, 653000116}, 147, kComment};
  VerifyComment(expected_comment, records.comment);

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(2236, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_PROCESSING_PARAMETERS, 0}, data_id);

  // TODO(schwehr): VerifyProcessingParameters().

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(1388, num_bytes);
  ASSERT_EQ(GSF_RECORD_SOUND_VELOCITY_PROFILE, data_id.recordID);
  VerifyDataId({false, 0, GSF_RECORD_SOUND_VELOCITY_PROFILE, 0}, data_id);

  // TODO(schwehr): VerifySoundVelocityProfile().

  for (int count=0; count < 288; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    // ASSERT_GE(num_bytes, 0);
    if (num_bytes < 1)
      break;
#if 0
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

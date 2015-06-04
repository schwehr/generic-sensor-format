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

#include "gsf.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using std::string;
using std::vector;

// TODO(schwehr): Remove iostream.
#include <iostream>

// GSF error value.
// TODO(schwehr):  Why is this not in the header?
extern int gsfError;

namespace generic_sensor_format {
namespace {

class PacketCounts {
 public:
  PacketCounts() : counts_(NUM_REC_TYPES, 0) {}
  void add(int packet_num) { ++counts_[packet_num]; }
  vector<int> counts_;
  void Verify(const vector<int> &expected) {
    ASSERT_EQ(NUM_REC_TYPES, expected.size());
    for (int i=0; i < NUM_REC_TYPES; ++i) {
      EXPECT_EQ(expected[i], counts_[i]);
    }
  }
};

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
  ASSERT_EQ(0, gsfOpen("../data/6_184_1440.mb121", GSF_READONLY, &handle));
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
  counts.Verify({0, 0, 132, 21, 1, 0, 2, 1, 0, 1, 0, 0, 132});
}

TEST(GsfReadTest, ReadVersion2_9) {
  int handle;
  ASSERT_EQ(0, gsfOpen("../data/6_184_1440.mb121", GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);

  gsfDataID data_id;
  gsfRecords records;

  int num_bytes;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(48, num_bytes);

  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(9, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_SWATH_BATHY_SUMMARY, data_id.recordID);
  // TODO(schwehr): Why does this sometimes fail?
  // ASSERT_EQ(0, data_id.record_number);

  ASSERT_EQ(947169642, records.summary.start_time.tv_sec);
  ASSERT_EQ(799999952, records.summary.start_time.tv_nsec);
  ASSERT_EQ(947171370, records.summary.end_time.tv_sec);
  ASSERT_EQ(700000047, records.summary.end_time.tv_nsec);
  ASSERT_DOUBLE_EQ(37.963955, records.summary.min_latitude);
  ASSERT_DOUBLE_EQ(-76.3502001, records.summary.min_longitude);
  ASSERT_DOUBLE_EQ(38.0030681, records.summary.max_latitude);
  ASSERT_DOUBLE_EQ(-76.2895285, records.summary.max_longitude);
  ASSERT_DOUBLE_EQ(12.12, records.summary.min_depth);
  ASSERT_DOUBLE_EQ(13.12, records.summary.max_depth);

  int count = 0;

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(84, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(6, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_COMMENT, data_id.recordID);
  // ASSERT_EQ(-2, data_id.record_number);

  ASSERT_EQ(947169642, records.comment.comment_time.tv_sec);
  ASSERT_EQ(799999952, records.comment.comment_time.tv_nsec);
  ASSERT_STREQ("Bathy converted from HIPS file: B_SB BH_SB 2000-001 6_184_1440",
               records.comment.comment);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(988, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(4, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_PROCESSING_PARAMETERS, data_id.recordID);
  // ASSERT_EQ(-2, data_id.record_number);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(212, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(3, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_SOUND_VELOCITY_PROFILE, data_id.recordID);
  // ASSERT_EQ(-2, data_id.record_number);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(68, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(6, data_id.recordID);
  ASSERT_EQ(GSF_RECORD_COMMENT, data_id.recordID);
  // ASSERT_EQ(-2, data_id.record_number);

  ASSERT_EQ(947169629, records.comment.comment_time.tv_sec);
  ASSERT_EQ(799999952, records.comment.comment_time.tv_nsec);
  ASSERT_STREQ("SVP_FILE_NAME: D:\\hips\\Svp\\SheetB\\all_Bsheet.svp",
               records.comment.comment);

  for (; count < 40; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    ASSERT_GE(num_bytes, 0);
  }

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(28, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(GSF_RECORD_ATTITUDE, data_id.recordID);
  ASSERT_EQ(1, records.attitude.num_measurements);
  ASSERT_EQ(947169765, records.attitude.attitude_time[0].tv_sec);
  ASSERT_EQ(600000023, records.attitude.attitude_time[0].tv_nsec);
  ASSERT_DOUBLE_EQ(-1.0, records.attitude.pitch[0]);
  ASSERT_DOUBLE_EQ(-1.0, records.attitude.roll[0]);
  ASSERT_DOUBLE_EQ(0.08, records.attitude.heave[0]);
  ASSERT_DOUBLE_EQ(127.0, records.attitude.heading[0]);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(412, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(GSF_RECORD_SWATH_BATHYMETRY_PING, data_id.recordID);
  ASSERT_EQ(947169765, records.mb_ping.ping_time.tv_sec);
  ASSERT_EQ(600000023, records.mb_ping.ping_time.tv_nsec);
  EXPECT_DOUBLE_EQ(38.0007082, records.mb_ping.latitude);
  EXPECT_DOUBLE_EQ(-76.3465419, records.mb_ping.longitude);
  EXPECT_DOUBLE_EQ(9999.9899999999998, records.mb_ping.height);
  EXPECT_DOUBLE_EQ(9999.9899999999998, records.mb_ping.sep);
  EXPECT_EQ(1, records.mb_ping.number_beams);
  EXPECT_EQ(1, records.mb_ping.center_beam);
  EXPECT_EQ(0, records.mb_ping.ping_flags);
  EXPECT_EQ(0, records.mb_ping.reserved);
  EXPECT_DOUBLE_EQ(-0.1, records.mb_ping.tide_corrector);
  EXPECT_DOUBLE_EQ(99.989999999999995, records.mb_ping.gps_tide_corrector);
  EXPECT_DOUBLE_EQ(99.989999999999995, records.mb_ping.depth_corrector);
  EXPECT_DOUBLE_EQ(127, records.mb_ping.heading);
  EXPECT_DOUBLE_EQ(-1.0, records.mb_ping.pitch);
  EXPECT_DOUBLE_EQ(-1.0, records.mb_ping.roll);
  EXPECT_DOUBLE_EQ(0.08, records.mb_ping.heave);
  EXPECT_DOUBLE_EQ(131.41, records.mb_ping.course);
  EXPECT_DOUBLE_EQ(6.04, records.mb_ping.speed);
  ASSERT_EQ(26, records.mb_ping.scaleFactors.numArraySubrecords);
  EXPECT_EQ(32, records.mb_ping.scaleFactors.scaleTable[0].compressionFlag);
  EXPECT_DOUBLE_EQ(
      10000, records.mb_ping.scaleFactors.scaleTable[0].multiplier);
  EXPECT_DOUBLE_EQ(-13, records.mb_ping.scaleFactors.scaleTable[0].offset);
  // double            *depth;
  // double            *nominal_depth;
  // double            *across_track;
  // double            *along_track;
  // double            *travel_time;
  // double            *beam_angle;
  // double            *mc_amplitude;
  // double            *mr_amplitude;
  // double            *echo_width;
  // double            *quality_factor;
  // double            *receive_heave;
  // double            *depth_error;
  // double            *across_track_error;
  // double            *along_track_error;
  // unsigned char     *quality_flags;
  // unsigned char     *beam_flags;
  // double            *signal_to_noise;
  // double            *beam_angle_forward;
  // double            *vertical_error;
  // double            *horizontal_error;
  // unsigned short    *sector_number;
  // unsigned short    *detection_info;
  // double            *incident_beam_adj;
  // unsigned short    *system_cleaning;
  // double            *doppler_corr;
  // double            *sonar_vert_uncert;
  // int                sensor_id;
  // gsfSensorSpecific  sensor_data;
  // gsfBRBIntensity   *brb_inten;

  for (; count < 288; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    ASSERT_GE(num_bytes, 0);
#if 0
    std::cout << "rec: count: " << count
         << " #" << data_id.record_number
         << " id: " << data_id.recordID
         << " size: " << num_bytes
         << "   -->   " << RecordTypeStr(data_id.recordID)
         << "\n";
#endif
  }

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(60, num_bytes);
  ASSERT_FALSE(data_id.checksumFlag);
  ASSERT_EQ(0, data_id.reserved);
  ASSERT_EQ(GSF_RECORD_HISTORY, data_id.recordID);
  ASSERT_EQ(1253181992, records.history.history_time.tv_sec);
  ASSERT_EQ(0, records.history.history_time.tv_nsec);
  ASSERT_STREQ("PIXAR", records.history.host_name);
  ASSERT_STREQ("ltyson", records.history.operator_name);
  ASSERT_STREQ("HIPStoGSF", records.history.command_line);
  ASSERT_STREQ("version 6.1", records.history.comment);


  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(-1, num_bytes);
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfError);
}

}  // namespace
}  // namespace generic_sensor_format

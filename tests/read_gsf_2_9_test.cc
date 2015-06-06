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

using std::string;
using std::vector;

// GSF error value.
// TODO(schwehr):  Why is this not in the header?
extern int gsfError;

namespace generic_sensor_format {
namespace test {
namespace {

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

  VerifyDataId({false, 0, GSF_RECORD_SWATH_BATHY_SUMMARY, 0}, data_id);

  const gsfSwathBathySummary expected =
      {
        {947169642, 799999952},
        {947171370, 700000047},
        37.963955, -76.3502001,
        38.0030681, -76.2895285,
        12.12, 13.12 };
  VerifySwathBathySummary(expected, records.summary);

  int count = 0;

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(84, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_COMMENT, 0}, data_id);

  VerifyComment(GsfComment({947169642, 799999952},
                           "Bathy converted from HIPS file: "
                           "B_SB BH_SB 2000-001 6_184_1440"),
                records.comment);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(988, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_PROCESSING_PARAMETERS, 0}, data_id);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(212, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_SOUND_VELOCITY_PROFILE, 0}, data_id);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(68, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_COMMENT, 0}, data_id);

  VerifyComment(GsfComment({947169629, 799999952},
                           "SVP_FILE_NAME: D:\\hips\\Svp\\"
                           "SheetB\\all_Bsheet.svp"),
                records.comment);

  for (; count < 40; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    ASSERT_GE(num_bytes, 0);
  }

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(28, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_ATTITUDE, 0}, data_id);
  // TODO(schwehr): VerifyAttitude(expected_attitude, records.attitude);
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
  VerifyDataId({false, 0, GSF_RECORD_SWATH_BATHYMETRY_PING, 0}, data_id);
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
  EXPECT_EQ(0, records.mb_ping.scaleFactors.scaleTable[25].compressionFlag);
  EXPECT_DOUBLE_EQ(
      1, records.mb_ping.scaleFactors.scaleTable[25].multiplier);
  EXPECT_DOUBLE_EQ(0, records.mb_ping.scaleFactors.scaleTable[25].offset);

  EXPECT_DOUBLE_EQ(13.116, records.mb_ping.depth[0]);
  ASSERT_EQ(nullptr, records.mb_ping.nominal_depth);
  EXPECT_DOUBLE_EQ(0.21715, records.mb_ping.across_track[0]);
  EXPECT_DOUBLE_EQ(-0.2142, records.mb_ping.along_track[0]);
  EXPECT_DOUBLE_EQ(0.0168, records.mb_ping.travel_time[0]);
  EXPECT_DOUBLE_EQ(0, records.mb_ping.beam_angle[0]);
  EXPECT_EQ(nullptr, records.mb_ping.mc_amplitude);
  EXPECT_EQ(nullptr, records.mb_ping.mr_amplitude);
  EXPECT_EQ(nullptr, records.mb_ping.echo_width);
  EXPECT_EQ(nullptr, records.mb_ping.quality_factor);
  EXPECT_EQ(nullptr, records.mb_ping.receive_heave);
  EXPECT_EQ(nullptr, records.mb_ping.depth_error);
  EXPECT_EQ(nullptr, records.mb_ping.across_track_error);
  EXPECT_EQ(nullptr, records.mb_ping.along_track_error);
  EXPECT_EQ(nullptr, records.mb_ping.quality_flags);
  EXPECT_EQ(0, records.mb_ping.beam_flags[0]);
  EXPECT_EQ(nullptr, records.mb_ping.signal_to_noise);
  EXPECT_EQ(nullptr, records.mb_ping.beam_angle_forward);
  EXPECT_EQ(nullptr, records.mb_ping.vertical_error);
  EXPECT_EQ(nullptr, records.mb_ping.horizontal_error);
  EXPECT_EQ(nullptr, records.mb_ping.sector_number);
  EXPECT_EQ(nullptr, records.mb_ping.detection_info);
  EXPECT_EQ(nullptr, records.mb_ping.incident_beam_adj);
  EXPECT_EQ(nullptr, records.mb_ping.system_cleaning);
  EXPECT_EQ(nullptr, records.mb_ping.doppler_corr);
  EXPECT_EQ(nullptr, records.mb_ping.sonar_vert_uncert);
  EXPECT_EQ(0, records.mb_ping.sensor_id);
  // TODO(schwehr): gsfSensorSpecific  sensor_data.
  EXPECT_EQ(nullptr, records.mb_ping.brb_inten);

  for (; count < 288; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    ASSERT_GE(num_bytes, 0);
  }

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(60, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_HISTORY, 0}, data_id);
  VerifyHistory(GsfHistory({1253181992, 0},
                           "PIXAR",
                           "ltyson",
                           "HIPStoGSF",
                           "version 6.1"),
                records.history);

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(-1, num_bytes);
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfError);
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

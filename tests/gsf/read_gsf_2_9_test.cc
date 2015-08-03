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

#include <array>
#include <string>

#include "gmock/gmock.h"
#include "gsf.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

using std::string;
using std::vector;

namespace generic_sensor_format {
namespace test {
namespace {

TEST(GsfRead2_9Test, CountPackets) {
  int handle;
  ASSERT_EQ(
      0, gsfOpen("../../data/surveys/6_184_1440.mb121", GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);

  gsfDataID data_id;
  gsfRecords records;

  int num_bytes;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  PacketCounts counts;
  while (num_bytes >= 0) {
    counts.add(data_id.recordID);
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  }
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfIntError());
  counts.Verify({0, 0, 132, 21, 1, 0, 2, 1, 0, 1, 0, 0, 132});
}

TEST(GsfReadTest, ReadVersion2_9) {
  int handle;
  ASSERT_EQ(
      0, gsfOpen("../../data/surveys/6_184_1440.mb121", GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);

  gsfDataID data_id;
  gsfRecords records;

  int num_bytes;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(48, num_bytes);

  VerifyDataId({false, 0, GSF_RECORD_SWATH_BATHY_SUMMARY, 0}, data_id);

  const gsfSwathBathySummary expected = {{947169642, 799999952},
                                         {947171370, 700000047},
                                         37.963955,
                                         -76.3502001,
                                         38.0030681,
                                         -76.2895285,
                                         12.12,
                                         13.12};
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

  const struct timespec times[] = {{947169765, 600000023}};
  const double pitch[] = {-1.0};
  const double roll[] = {-1.0};
  const double heave[] = {0.08};
  const double heading[] = {127.0};
  VerifyAttitude(GsfAttitude(1, times, pitch, roll, heave, heading),
                 records.attitude);

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(412, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_SWATH_BATHYMETRY_PING, 0}, data_id);

  gsfScaleFactors scale_factors = GsfScaleFactors(
      {gsfScaleInfo({32, 10000, -13}), gsfScaleInfo({32, 100000, 0}),
       gsfScaleInfo({32, 100000, 0}), gsfScaleInfo({32, 100000, 0}),
       gsfScaleInfo({32, 100000, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({16, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0}),
       gsfScaleInfo({0, 1, 0}), gsfScaleInfo({0, 1, 0})});

  const std::array<double, 1> depth = {{13.116}};
  const std::array<double, 1> across_track = {{0.21715}};
  const std::array<double, 1> along_track = {{-0.2142}};
  const std::array<double, 1> travel_time = {{0.0168}};
  const std::array<double, 1> beam_angle = {{0}};
  const std::array<unsigned char, 1> beam_flags = {{0}};

  const gsfSwathBathyPing expected_ping =
      GsfSwathBathyPing({947169765, 600000023},  //
                        38.0007082,              // latitude
                        -76.3465419,             // longitude
                        9999.99,                 // height
                        9999.99,                 // sep
                        1,                       //
                        1,                       //
                        0,                       //
                        0,                       //
                        -0.1,                    //
                        99.99,                   //
                        99.99,                   //
                        127,                     //
                        -1,                      //
                        -1,                      //
                        0.08,                    //
                        131.41,                  //
                        6.04,                    //
                        scale_factors, depth.data(),
                        nullptr,  // nominal_depth.
                        across_track.data(), along_track.data(),
                        travel_time.data(), beam_angle.data(),
                        nullptr,  // mc_amplitude.
                        nullptr,  // mr_amplitude.
                        nullptr,  // echo_width.
                        nullptr,  // quality_factor.
                        nullptr,  // receive_heave.
                        nullptr,  // depth_error.
                        nullptr,  // across_track_error.
                        nullptr,  // along_track_error.
                        nullptr,  // quality_flags.
                        beam_flags.data(),
                        nullptr,  // signal_to_noise.
                        nullptr,  // beam_angle_forward.
                        nullptr,  // vertical_error.
                        nullptr,  // horizontal_error.
                        nullptr,  // sector_number.
                        nullptr,  // detection_info.
                        nullptr,  // incident_beam_adj.
                        nullptr,  // system_cleaning.
                        nullptr,  // doppler_corr.
                        nullptr,  // sonar_vert_uncert.
                        0         // sensor_id.
                        );

  // std::cout << records.mb_ping;
  // std::cout << "\nnominal_depth ===> "
  // << records.mb_ping.nominal_depth << "\n";

  VerifySwathBathyPing(expected_ping, records.mb_ping);

  for (; count < 288; ++count) {
    num_bytes =
        gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
    ASSERT_GE(num_bytes, 0);
  }

  ++count;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(60, num_bytes);
  VerifyDataId({false, 0, GSF_RECORD_HISTORY, 0}, data_id);
  VerifyHistory(GsfHistory({1253181992, 0}, "PIXAR", "ltyson", "HIPStoGSF",
                           "version 6.1"),
                records.history);

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(-1, num_bytes);
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfIntError());
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

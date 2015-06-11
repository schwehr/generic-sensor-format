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

gsfAttitude GsfAttitude(short num_measurements,
                        const struct timespec *attitude_time,
                        const double *pitch,
                        const double *roll,
                        const double *heave,
                        const double *heading) {
  gsfAttitude attitude;
  attitude.num_measurements = num_measurements;
  attitude.attitude_time = const_cast<struct timespec *>(attitude_time);
  attitude.pitch = const_cast<double *>(pitch);
  attitude.roll = const_cast<double *>(roll);
  attitude.heave = const_cast<double *>(heave);
  attitude.heading = const_cast<double *>(heading);
  return attitude;
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

void VerifyAttitude(const gsfAttitude &expected, const gsfAttitude &actual) {
  EXPECT_EQ(expected.num_measurements, actual.num_measurements);
  for (int i=0; i < expected.num_measurements; ++i) {
    EXPECT_EQ(expected.attitude_time[i].tv_sec, actual.attitude_time[i].tv_sec);
    EXPECT_EQ(expected.attitude_time[i].tv_nsec,
              actual.attitude_time[i].tv_nsec);
    EXPECT_DOUBLE_EQ(expected.pitch[i], actual.pitch[i]);
    EXPECT_DOUBLE_EQ(expected.roll[i], actual.roll[i]);
    EXPECT_DOUBLE_EQ(expected.heave[i], actual.heave[i]);
    EXPECT_DOUBLE_EQ(expected.heading[i], actual.heading[i]);
  }
}

void VerifyDataId(const gsfDataID &expected, const gsfDataID &actual) {
  EXPECT_EQ(expected.checksumFlag, actual.checksumFlag);
  EXPECT_EQ(expected.reserved, actual.reserved);
  EXPECT_EQ(expected.recordID, actual.recordID);
  // TODO(schwehr): When should this number match?
  // EXPECT_EQ(expected.record_number, actual.record_number);
}

void VerifySwathBathyPing(const gsfSwathBathyPing &expected,
                          const gsfSwathBathyPing &actual) {
  EXPECT_EQ(expected.ping_time.tv_sec, actual.ping_time.tv_sec);
  EXPECT_EQ(expected.ping_time.tv_nsec, actual.ping_time.tv_nsec);
  EXPECT_DOUBLE_EQ(expected.latitude, actual.latitude);
  EXPECT_DOUBLE_EQ(expected.longitude, actual.longitude);
  EXPECT_DOUBLE_EQ(expected.height, actual.height);
  EXPECT_DOUBLE_EQ(expected.sep, actual.sep);
  EXPECT_EQ(expected.number_beams, actual.number_beams);
  EXPECT_EQ(expected.center_beam, actual.center_beam);
  EXPECT_EQ(expected.ping_flags, actual.ping_flags);
  EXPECT_EQ(expected.reserved, actual.reserved);
  EXPECT_DOUBLE_EQ(expected.tide_corrector, actual.tide_corrector);
  EXPECT_DOUBLE_EQ(expected.gps_tide_corrector, actual.gps_tide_corrector);
  EXPECT_DOUBLE_EQ(expected.depth_corrector, actual.depth_corrector);
  EXPECT_DOUBLE_EQ(expected.heading, actual.heading);
  EXPECT_DOUBLE_EQ(expected.pitch, actual.pitch);
  EXPECT_DOUBLE_EQ(expected.roll, actual.roll);
  EXPECT_DOUBLE_EQ(expected.heave, actual.heave);
  EXPECT_DOUBLE_EQ(expected.course, actual.course);
  EXPECT_DOUBLE_EQ(expected.speed, actual.speed);
  ASSERT_EQ(expected.scaleFactors.numArraySubrecords,
            actual.scaleFactors.numArraySubrecords);
  if (expected.scaleFactors.scaleTable == nullptr ||
      actual.scaleFactors.scaleTable == nullptr) {
    EXPECT_EQ(expected.scaleFactors.scaleTable, actual.scaleFactors.scaleTable);
  } else {
    for (int i=0; i < expected.scaleFactors.numArraySubrecords; ++i) {
      EXPECT_EQ(expected.scaleFactors.scaleTable[i].compressionFlag,
                actual.scaleFactors.scaleTable[i].compressionFlag);
      EXPECT_DOUBLE_EQ(expected.scaleFactors.scaleTable[i].multiplier,
                       actual.scaleFactors.scaleTable[i].multiplier);
      EXPECT_DOUBLE_EQ(expected.scaleFactors.scaleTable[i].offset,
                       actual.scaleFactors.scaleTable[i].offset);
    }
  }
  // EXPECT_DOUBLE_EQ(, actual.depth[0]);
  // EXPECT_EQ(nullptr, actual.nominal_depth);
  // EXPECT_DOUBLE_EQ(, actual.across_track[0]);
  // EXPECT_DOUBLE_EQ(, actual.along_track[0]);
  // EXPECT_DOUBLE_EQ(, actual.travel_time[0]);
  // EXPECT_DOUBLE_EQ(, actual.beam_angle[0]);
  // EXPECT_EQ(, actual.mc_amplitude);
  // EXPECT_EQ(, actual.mr_amplitude);
  // EXPECT_EQ(, actual.echo_width);
  // EXPECT_EQ(, actual.quality_factor);
  // EXPECT_EQ(, actual.receive_heave);
  // EXPECT_EQ(, actual.depth_error);
  // EXPECT_EQ(, actual.across_track_error);
  // EXPECT_EQ(, actual.along_track_error);
  // EXPECT_EQ(, actual.quality_flags);
  // EXPECT_EQ(, actual.beam_flags[0]);
  // EXPECT_EQ(, actual.signal_to_noise);
  // EXPECT_EQ(, actual.beam_angle_forward);
  // EXPECT_EQ(, actual.vertical_error);
  // EXPECT_EQ(, actual.horizontal_error);
  // EXPECT_EQ(, actual.sector_number);
  // EXPECT_EQ(, actual.detection_info);
  // EXPECT_EQ(, actual.incident_beam_adj);
  // EXPECT_EQ(, actual.system_cleaning);
  // EXPECT_EQ(, actual.doppler_corr);
  // EXPECT_EQ(, actual.sonar_vert_uncert);
  // EXPECT_EQ(0, actual.sensor_id);
  // TODO(schwehr): gsfSensorSpecific  sensor_data.
  // EXPECT_EQ(, actual.brb_inten);
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

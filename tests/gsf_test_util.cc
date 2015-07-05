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

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

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

TempDir::TempDir(bool preserve) : preserve_(preserve) {
  char _template[] = "tmp_testdirXXXXXX";
  tmp_dir_ = mkdtemp(_template);
}

TempDir::~TempDir() {
  if (preserve_) {
    return;
  }
  DIR* dir = opendir(tmp_dir_.c_str());
  struct dirent *entry;
  while ((entry = readdir(dir))) {
    if (entry->d_type != DT_REG) {
      continue;
    }
    unlink(entry->d_name);
  }
  rmdir(tmp_dir_.c_str());
}

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

const gsfAttitude GsfAttitude(short num_measurements,
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

const gsfComment GsfComment(const struct timespec &when, const char *comment) {
  gsfComment ret = {
    { when.tv_sec, when.tv_nsec },
    int(strlen(comment)),
    const_cast<char *>(comment)
  };
  return ret;
}

const gsfHistory GsfHistory(const struct timespec &when,
                      const char *host_name,
                      const char *operator_name,
                      const char *command_line,
                      const char *comment) {
  gsfHistory history;
  history.history_time = { when.tv_sec, when.tv_nsec };
  strncpy(history.host_name, host_name, GSF_HOST_NAME_LENGTH);
  history.host_name[GSF_HOST_NAME_LENGTH] = '\0';
  strncpy(history.operator_name, operator_name, GSF_OPERATOR_LENGTH);
  history.operator_name[GSF_OPERATOR_LENGTH] = '\0';
  history.command_line = const_cast<char *>(command_line);
  history.comment = const_cast<char *>(comment);
  return history;
}

const gsfScaleFactors GsfScaleFactors(const vector<gsfScaleInfo> &scale_info) {
  assert(scale_info.size() <= GSF_MAX_PING_ARRAY_SUBRECORDS);
  gsfScaleFactors scale_factors;
  scale_factors.numArraySubrecords = scale_info.size();
  for (vector<gsfScaleInfo>::size_type i=0; i < scale_info.size(); ++i) {
    scale_factors.scaleTable[i] = scale_info[i];
  }
  for (int i=scale_info.size(); i < GSF_MAX_PING_ARRAY_SUBRECORDS; ++i) {
    scale_factors.scaleTable[i] = {0, 0.0, 0.0};
  }
  return scale_factors;
}

const gsfSwathBathyPing GsfSwathBathyPing(
    struct timespec ping_time,
    double latitude,
    double longitude,
    double height,
    double sep,
    short number_beams,
    short center_beam,
    unsigned short ping_flags,
    short reserved,
    double tide_corrector,
    double gps_tide_corrector,
    double depth_corrector,
    double heading,
    double pitch,
    double roll,
    double heave,
    double course,
    double speed,
    gsfScaleFactors &scaleFactors,
    const double *depth,
    const double *nominal_depth,
    const double *across_track,
    const double *along_track,
    const double *travel_time,
    const double *beam_angle,
    const double *mc_amplitude,
    const double *mr_amplitude,
    const double *echo_width,
    const double *quality_factor,
    const double *receive_heave,
    const double *depth_error,
    const double *across_track_error,
    const double *along_track_error,
    const unsigned char *quality_flags,
    const unsigned char *beam_flags,
    const double *signal_to_noise,
    const double *beam_angle_forward,
    const double *vertical_error,
    const double *horizontal_error,
    const unsigned short *sector_number,
    const unsigned short *detection_info,
    const double *incident_beam_adj,
    const unsigned short *system_cleaning,
    const double *doppler_corr,
    const double *sonar_vert_uncert,
    int sensor_id // ,
    // gsfSensorSpecific sensor_data,
    // const gsfBRBIntensity *brb_inten
  ) {
  gsfSwathBathyPing ping;
  ping.ping_time = ping_time;
  ping.latitude = latitude;
  ping.longitude = longitude;
  ping.height = height;
  ping.sep = sep;
  ping.number_beams = number_beams;
  ping.center_beam = center_beam;
  ping.ping_flags = ping_flags;
  ping.reserved = reserved;
  ping.tide_corrector = tide_corrector;
  ping.gps_tide_corrector = gps_tide_corrector;
  ping.depth_corrector = depth_corrector;
  ping.heading = heading;
  ping.pitch = pitch;
  ping.roll = roll;
  ping.heave = heave;
  ping.course = course;
  ping.speed = speed;
  ping.scaleFactors = scaleFactors;
  ping.depth = const_cast<double *>(depth);
  ping.nominal_depth = const_cast<double *>(nominal_depth);
  ping.across_track = const_cast<double *>(across_track);
  ping.along_track = const_cast<double *>(along_track);
  ping.travel_time = const_cast<double *>(travel_time);
  ping.beam_angle = const_cast<double *>(beam_angle);
  ping.mc_amplitude = const_cast<double *>(mc_amplitude);
  ping.mr_amplitude = const_cast<double *>(mr_amplitude);
  ping.echo_width = const_cast<double *>(echo_width);
  ping.quality_factor = const_cast<double *>(quality_factor);
  ping.receive_heave = const_cast<double *>(receive_heave);
  ping.depth_error = const_cast<double *>(depth_error);
  ping.across_track_error = const_cast<double *>(across_track_error);
  ping.along_track_error = const_cast<double *>(along_track_error);
  ping.quality_flags = const_cast<unsigned char *>(quality_flags);
  ping.beam_flags = const_cast<unsigned char *>(beam_flags);
  ping.signal_to_noise = const_cast<double *>(signal_to_noise);
  ping.beam_angle_forward = const_cast<double *>(beam_angle_forward);
  ping.vertical_error = const_cast<double *>(vertical_error);
  ping.horizontal_error = const_cast<double *>(horizontal_error);
  ping.sector_number = const_cast<unsigned short *>(sector_number);
  ping.detection_info = const_cast<unsigned short *>(detection_info);
  ping.incident_beam_adj = const_cast<double *>(incident_beam_adj);
  ping.system_cleaning = const_cast<unsigned short *>(system_cleaning);
  ping.doppler_corr = const_cast<double *>(doppler_corr);
  ping.sonar_vert_uncert = const_cast<double *>(sonar_vert_uncert);
  ping.sensor_id = sensor_id;
  // ping.sensor_data = sensor_data;
  // ping.brb_inten = const_cast<gsfBRBIntensity *>(brb_inten);

  return ping;
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

void VerifyArray(const unsigned char *expected, const unsigned char *actual,
                 int size) {
  if (size == 0) {
    EXPECT_EQ(nullptr, expected);
    EXPECT_EQ(nullptr, actual);
    return;
  }
  if (expected == nullptr) {
    ASSERT_EQ(nullptr, actual);
    return;
  }
  for (int i=0; i < size; ++i) {
    EXPECT_EQ(actual[i], expected[i]);
  }
}

void VerifyArray(const unsigned short *expected, const unsigned short *actual,
                 int size) {
  if (size == 0) {
    EXPECT_EQ(nullptr, expected);
    EXPECT_EQ(nullptr, actual);
    return;
  }
  if (expected == nullptr) {
    ASSERT_EQ(nullptr, actual);
    return;
  }
  for (int i=0; i < size; ++i) {
    EXPECT_EQ(actual[i], expected[i]);
  }
}

void VerifyArray(const double *expected, const double *actual,
                 int size) {
  // std::cout << "VA: " << expected << " " << actual << "\n";
  if (size == 0) {
    ASSERT_EQ(nullptr, expected);
    ASSERT_EQ(nullptr, actual);
    return;
  }
  if (expected == nullptr) {
    ASSERT_EQ(nullptr, actual);
    return;
  }
  for (int i=0; i < size; ++i) {
    EXPECT_EQ(actual[i], expected[i]);
  }
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
#if 0
  if (expected.scaleFactors.scaleTable == nullptr ||
      actual.scaleFactors.scaleTable == nullptr) {
    EXPECT_EQ(expected.scaleFactors.scaleTable, actual.scaleFactors.scaleTable);
  } else
#endif
  {
    for (int i=0; i < expected.scaleFactors.numArraySubrecords; ++i) {
      EXPECT_EQ(expected.scaleFactors.scaleTable[i].compressionFlag,
                actual.scaleFactors.scaleTable[i].compressionFlag);
      EXPECT_DOUBLE_EQ(expected.scaleFactors.scaleTable[i].multiplier,
                       actual.scaleFactors.scaleTable[i].multiplier);
      EXPECT_DOUBLE_EQ(expected.scaleFactors.scaleTable[i].offset,
                       actual.scaleFactors.scaleTable[i].offset);
    }
  }
#if 0
  if (expected.depth == nullptr || actual.depth == nullptr) {
    EXPECT_EQ(expected.depth, actual.depth);
  } else {
    for (int i=0; i < expected.number_beams; ++i) {
      EXPECT_EQ(actual.depth[i], expected.depth[i]);
    }
  }
#endif
  const auto size = expected.number_beams;
  VerifyArray(expected.depth, actual.depth, size);
  VerifyArray(expected.nominal_depth, actual.nominal_depth, size);
  VerifyArray(expected.across_track, actual.across_track, size);
  VerifyArray(expected.along_track, actual.along_track, size);
  VerifyArray(expected.travel_time, actual.travel_time, size);
  VerifyArray(expected.beam_angle, actual.beam_angle, size);
  VerifyArray(expected.mc_amplitude, actual.mc_amplitude, size);
  VerifyArray(expected.mr_amplitude, actual.mr_amplitude, size);
  VerifyArray(expected.echo_width, actual.echo_width, size);
  VerifyArray(expected.quality_factor, actual.quality_factor, size);
  VerifyArray(expected.receive_heave, actual.receive_heave, size);
  VerifyArray(expected.depth_error, actual.depth_error, size);
  VerifyArray(expected.across_track_error, actual.across_track_error, size);
  VerifyArray(expected.along_track_error, actual.along_track_error, size);
  VerifyArray(expected.quality_flags, actual.quality_flags, size);
  VerifyArray(expected.beam_flags, actual.beam_flags, size);
  VerifyArray(expected.signal_to_noise, actual.signal_to_noise, size);
  VerifyArray(expected.beam_angle_forward, actual.beam_angle_forward, size);
  VerifyArray(expected.vertical_error, actual.vertical_error, size);
  VerifyArray(expected.horizontal_error, actual.horizontal_error, size);
  VerifyArray(expected.sector_number, actual.sector_number, size);
  VerifyArray(expected.detection_info, actual.detection_info, size);
  VerifyArray(expected.incident_beam_adj, actual.incident_beam_adj, size);
  VerifyArray(expected.system_cleaning, actual.system_cleaning, size);
  VerifyArray(expected.doppler_corr, actual.doppler_corr, size);
  VerifyArray(expected.sonar_vert_uncert, actual.sonar_vert_uncert, size);
  // TODO(schwehr): actual.sensor_id.
  // TODO(schwehr): gsfSensorSpecific sensor_data.
  // TODO(schwehr): actual.brb_inten.
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

std::ostream& operator<< (std::ostream &o, const gsfBRBIntensity &intensity) {
  o << "GsfBrbIntensity(\n"
    << "  " << intensity.bits_per_sample << ",\n"
    << "  " << intensity.applied_corrections << ",\n"
    << "  {\n";
  for (int i=0; i < 15; ++i) {
    o << "    " << intensity.spare[i] << ",\n";
  }
  o << "    " << intensity.spare[15] << "\n"
    << "}\n";

  // TODO(schwehr): gsfSensorImagery sensor_imagery.
  if (intensity.time_series == nullptr) {
    o << "  nullptr  // time_series\n";
  } else {
    // TODO(schwehr): How do we know how long time_series is?
    // Does this need num_beams from the containing structure?
    o << "  // time_series here.\n";
  }
  o << ")";
  return o;
}

std::ostream& operator<< (std::ostream &o, const gsfScaleFactors &factors) {
  o << "GsfScaleFactors(\n"
    << "  " << factors.numArraySubrecords << ",\n"
    << "  {\n";
  for (int i=0; i < factors.numArraySubrecords - 1; ++i) {
    o << "    " << factors.scaleTable[i] << ",\n";
  }
  if (factors.numArraySubrecords > 0) {
    o << "    " << factors.scaleTable[factors.numArraySubrecords - 1] << "\n";
  }
  o << "  })";
  return o;
}

std::ostream& operator<< (std::ostream &o, const gsfScaleInfo &scale_info) {
  o << "GsfScaleInfo("
    << static_cast<int>(scale_info.compressionFlag) << ", "
    << scale_info.multiplier << ", "
    << scale_info.offset
    << ")";
  return o;
}

std::ostream& operator<< (std::ostream &o, const gsfSwathBathyPing &ping) {
  o << "GsfSwathBathyPing(\n"
    << "  {" << ping.ping_time.tv_sec << ", "
    << ping.ping_time.tv_nsec << "},\n"
    << "  " << ping.latitude << ",  // latitude\n"
    << "  " << ping.longitude << ",  // longitude\n"
    << "  " << ping.height << ",  // \n"
    << "  " << ping.sep << ",  // \n"
    << "  " << ping.number_beams << ",  // \n"
    << "  " << ping.center_beam << ",  // \n"
    << "  " << ping.ping_flags << ",  // \n"
    << "  " << ping.reserved << ",  // \n"
    << "  " << ping.tide_corrector << ",  // \n"
    << "  " << ping.gps_tide_corrector << ",  // \n"
    << "  " << ping.depth_corrector << ",  // \n"
    << "  " << ping.heading << ",  // \n"
    << "  " << ping.pitch << ",  // \n"
    << "  " << ping.roll << ",  // \n"
    << "  " << ping.heave << ",  // \n"
    << "  " << ping.course << ",  // \n"
    << "  " << ping.speed << ",  // \n"
    << ping.scaleFactors << ",\n";

  WriteArray(o, ping.number_beams, ping.depth, "depth");
  WriteArray(o, ping.number_beams, ping.nominal_depth, "nominal_depth");
  WriteArray(o, ping.number_beams, ping.across_track, "across_track");
  WriteArray(o, ping.number_beams, ping.along_track, "along_track");
  WriteArray(o, ping.number_beams, ping.travel_time, "travel_time");
  WriteArray(o, ping.number_beams, ping.beam_angle, "beam_angle");
  WriteArray(o, ping.number_beams, ping.mc_amplitude, "mc_amplitude");
  WriteArray(o, ping.number_beams, ping.mr_amplitude, "mr_amplitude");
  WriteArray(o, ping.number_beams, ping.echo_width, "echo_width");
  WriteArray(o, ping.number_beams, ping.quality_factor, "quality_factor");
  WriteArray(o, ping.number_beams, ping.receive_heave, "receive_heave");
  WriteArray(o, ping.number_beams, ping.depth_error, "depth_error");
  WriteArray(o, ping.number_beams, ping.across_track_error, "across_track_error");
  WriteArray(o, ping.number_beams, ping.along_track_error, "along_track_error");
  WriteArray(o, ping.number_beams, ping.quality_flags, "quality_flags");
  WriteArray(o, ping.number_beams, ping.beam_flags, "beam_flags");
  WriteArray(o, ping.number_beams, ping.signal_to_noise, "signal_to_noise");
  WriteArray(o, ping.number_beams, ping.beam_angle_forward, "beam_angle_forward");
  WriteArray(o, ping.number_beams, ping.vertical_error, "vertical_error");
  WriteArray(o, ping.number_beams, ping.horizontal_error, "horizontal_error");
  WriteArray(o, ping.number_beams, ping.sector_number, "sector_number");
  WriteArray(o, ping.number_beams, ping.detection_info, "detection_info");
  WriteArray(o, ping.number_beams, ping.incident_beam_adj, "incident_beam_adj");
  WriteArray(o, ping.number_beams, ping.system_cleaning, "system_cleaning");
  WriteArray(o, ping.number_beams, ping.doppler_corr, "doppler_corr");
  WriteArray(o, ping.number_beams, ping.sonar_vert_uncert, "sonar_vert_uncert");
  o << "  " << ping.sensor_id << ",  // sensor_id.\n";
  // TODO(schwehr) gsfSensorSpecific sensor_data.
  // WriteArray(o, ping.number_beams, ping.brb_inten, "brb_inten");  // gsfBRBIntensity

  o << ");\n\n";
  return o;
  // return o << " (" << position.lng_deg << ", " << position.lat_deg << ")";
}

void WriteArray(std::ostream &o, size_t size, const unsigned char *data,
                const string &name) {
  if (data == nullptr) {
    o << "  nullptr,  // " << name << ".\n";
  } else {
    assert(size > 0);

    o << "  {\n";
    for (size_t i=0; i < size - 1; ++i) {
      o << "  " << int(data[i]) << ",\n";
    }
    o << "    " << int(data[size - 1]) << "\n"
      << "  }," << "  // " << name << ".\n";
  }
}

void WriteArray(std::ostream &o, size_t size, const unsigned short *data,
                const string &name) {
  if (data == nullptr) {
    o << "  nullptr,  // " << name << ".\n";
  } else {
    assert(size > 0);

    o << "  {\n";
    for (size_t i=0; i < size - 1; ++i) {
      o << "  " << data[i] << ",\n";
    }
    o << "    " << data[size - 1] << "\n"
      << "  }," << "  // " << name << ".\n";
  }
}

void WriteArray(std::ostream &o, size_t size, const double *data,
                const string &name) {
  if (data == nullptr) {
    o << "  nullptr,  // " << name << ".\n";
  } else {
    assert(size > 0);

    o << "  // " << name << ".\n  {\n";
    for (size_t i=0; i < size - 1; ++i) {
      o << "  " << data[i] << ",\n";
    }
    o << "    " << data[size - 1] << "\n"
      << "  },\n";
  }
}

}  // namespace test
}  // namespace generic_sensor_format

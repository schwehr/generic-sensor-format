// -*- c++ -*-
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

#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace generic_sensor_format {
namespace test {

std::string RecordTypeStr(unsigned int record_id);

// Do not modify the string inside the resulting gsfComment or gsfHistory.
const gsfAttitude GsfAttitude(short num_measurements,
                        const struct timespec *attitude_time,
                        const double *pitch,
                        const double *roll,
                        const double *heave,
                        const double *heading);
const gsfComment GsfComment(const struct timespec &when, const char *comment);
const gsfHistory GsfHistory(const struct timespec &when,
                      const char *host_name,
                      const char *operator_name,
                      const char *command_line,
                      const char *comment);
const gsfScaleFactors GsfScaleFactors(
    const std::vector<gsfScaleInfo> &scale_info);
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
                                          );
class PacketCounts {
 public:
  PacketCounts() : counts_(NUM_REC_TYPES, 0) {}
  void add(int packet_num) { ++counts_[packet_num]; }
  std::vector<int> counts_;
  void Verify(const std::vector<int> &expected);
};

void VerifyAttitude(const gsfAttitude &expected, const gsfAttitude &actual);

void VerifyDataId(const gsfDataID &expected, const gsfDataID &actual);

void VerifyComment(const gsfComment &expected, const gsfComment &actual);

void VerifyHistory(const gsfHistory &expected, const gsfHistory &actual);

void VerifySwathBathyPing(const gsfSwathBathyPing &expected,
                          const gsfSwathBathyPing &actual);

void VerifySwathBathySummary(const gsfSwathBathySummary &expected,
                             const gsfSwathBathySummary &actual);

std::ostream& operator<< (std::ostream &o, const gsfBRBIntensity &intensity);
std::ostream& operator<< (std::ostream &o, const gsfScaleFactors &factors);
std::ostream& operator<< (std::ostream &o, const gsfScaleInfo &scale_info);
std::ostream& operator<< (std::ostream &o, const gsfSwathBathyPing &ping);

void WriteArray(std::ostream &o, size_t size, const unsigned char *data,
                const std::string &name);
void WriteArray(std::ostream &o, size_t size, const unsigned short *data,
                const std::string &name);
void WriteArray(std::ostream &o, size_t size, const double *data,
                const std::string &name);

}  // namespace test
}  // namespace generic_sensor_format

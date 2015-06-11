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

#include <cassert>

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
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfIntError());
  counts.Verify({0, 0, 132, 21, 1, 0, 2, 1, 0, 1, 0, 0, 132});
}

gsfScaleFactors GsfScaleFactors(const vector<gsfScaleInfo> &scale_info) {
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

  // gsfScaleInfo GsfScaleInfo() {
  //    scale_info;
  //   return scale_info;
  // }

std::ostream& operator<< (std::ostream &o, const gsfScaleInfo &scale_info) {
  o << "GsfScaleInfo("
    << static_cast<int>(scale_info.compressionFlag) << ", "
    << scale_info.multiplier << ", "
    << scale_info.offset
    << ")";
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

// Do not modify the data in the pointers.
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

  // std::cout << records.mb_ping;

  gsfScaleFactors scale_factors = GsfScaleFactors(
      {
        gsfScaleInfo({32, 10000, -13}),
          gsfScaleInfo({32, 100000, 0}),
          gsfScaleInfo({32, 100000, 0}),
          gsfScaleInfo({32, 100000, 0}),
          gsfScaleInfo({32, 100000, 0}),
          gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({16, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
    gsfScaleInfo({0, 1, 0}),
          gsfScaleInfo({0, 1, 0})
          });

  const std::array<double, 1> depth = {{13.116}};
  const std::array<double, 1> across_track = {{0.21715}};
  const std::array<double, 1> along_track = {{-0.2142}};
  const std::array<double, 1> travel_time = {{0.0168}};
  const std::array<double, 1> beam_angle = {{0}};
  const std::array<unsigned char, 1> beam_flags = {{0}};

  const gsfSwathBathyPing expected_ping = GsfSwathBathyPing(
  {947169765, 600000023},  //
  38.0007082,  // latitude
  -76.3465419,  // longitude
  9999.99,  // height
  9999.99,  // sep
  1,  //
  1,  //
  0,  //
  0,  //
  -0.1,  //
  99.99,  //
  99.99,  //
  127,  //
  -1,  //
  -1,  //
  0.08,  //
  131.41,  //
  6.04,  //
  scale_factors,
  depth.data(),
  nullptr,  // nominal_depth.
  across_track.data(),
  along_track.data(),
  travel_time.data(),
  beam_angle.data(),
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
  0  // sensor_id.
);


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
  VerifyHistory(GsfHistory({1253181992, 0},
                           "PIXAR",
                           "ltyson",
                           "HIPStoGSF",
                           "version 6.1"),
                records.history);

  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, nullptr, 0);
  ASSERT_EQ(-1, num_bytes);
  ASSERT_EQ(GSF_READ_TO_END_OF_FILE, gsfIntError());
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

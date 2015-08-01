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

// Tests for writing out the basic record types.
//
// TODO(schwehr): Does this have to free what was read?

#include <sys/stat.h>

#include <memory>
#include <string>

#include "gsf.h"
#include "gsf_ft.h"
#include "gsf_test_util.h"
#include "gtest/gtest.h"

#include <iostream>  // TODO(schwehr): Remove.
using std::cerr;
// using namespace std;  // TODO(schwehr): Remove.

using std::string;
using std::unique_ptr;

namespace generic_sensor_format {
namespace test {
namespace {

// TODO(schwehr): Switch to make_unique when C++14 is available on Travis-CI.
template <typename T, typename... Args>
std::unique_ptr<T> MakeUnique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class GsfFile {
 public:
  static std::unique_ptr<GsfFile> Open(const std::string filename, int mode) {
    if (mode > GSF_APPEND) {
      cerr << "ERROR: Invalid mode\n";
      return nullptr;
    }

    int handle;
    int result = gsfOpen(filename.c_str(), mode, &handle);
    if (result != 0) {
      cerr << "Error opening " << filename << ": " << gsfStringError() << "\n";
      return nullptr;
    }

    return MakeUnique<GsfFile>(filename, handle);
  }

  GsfFile(const string filename, int handle)
      : filename_(filename), handle_(handle) {}
  ~GsfFile() {
    const int result = gsfClose(handle_);
    if (0 != result) {
      cerr << "Error closing " << filename_ << ": " << gsfStringError() << "\n";
    }
  }

  int handle() const { return handle_; }
  string filename() const { return filename_; }

 private:
  const string filename_;
  const int handle_;
};

TEST(GsfWriteSimple, HeaderOnly) {
  string filename = "header-only.gsf";
  {
    unique_ptr<GsfFile> file = GsfFile::Open(filename, GSF_CREATE);
    ASSERT_NE(nullptr, file);
  }
  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(20, buf.st_size);
}

// TODO(schwehr): GSF_RECORD_SWATH_BATHYMETRY_PING.
#if 0
void ValidateWriteSwathBathyPing(const string filename, bool checksum,
                                 int expected_write_size,
                                 const gsfSwathBathyPing &ping,
                                 int expected_file_size) {
  ASSERT_GE(expected_write_size, 0);  // TODO(schwehr): Set to the correct min.
  ASSERT_GE(expected_file_size, 0);   // TODO(schwehr): Set to the correct min.

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_SWATH_BATHYMETRY_PING, 0};
  gsfRecords record;

  record.mb_ping = ping;
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_SWATH_BATHYMETRY_PING, data_id.recordID);
  VerifySwathBathyPing(record.mb_ping, read_record.mb_ping);
}

// TODO(schwehr): This is generating a -1 on the gsfWrite.
TEST(SwathBathyPing, Empty) {
  const struct timespec when = {0, 0};
  gsfScaleFactors scale_factors = GsfScaleFactors({});

  const gsfSwathBathyPing ping =
      GsfSwathBathyPing(when, 0.0, 0.0,  // latitude, longitude
                        0.0,             // height
                        0.0,             // sep
                        0,               // number_beams
                        0,               // center_beam
                        0,               // ping_flags
                        0,               // reserved
                        0.0,             // tide_corrector
                        0.0,             // gps_tide_corrector
                        0.0,             // depth_corrector
                        0.0,             // heading
                        0.0,             // pitch
                        0.0,             // roll
                        0.0,             // heave
                        0.0,             // course
                        0.0,             // speed
                        scale_factors,
                        nullptr,  // depth_data
                        nullptr,  // nominal_depth.
                        nullptr,  // across_track
                        nullptr,  // along_track
                        nullptr,  // travel_time
                        nullptr,  // beam_angle
                        nullptr,  // mc_amplitude.
                        nullptr,  // mr_amplitude.
                        nullptr,  // echo_width.
                        nullptr,  // quality_factor.
                        nullptr,  // receive_heave.
                        nullptr,  // depth_error.
                        nullptr,  // across_track_error.
                        nullptr,  // along_track_error.
                        nullptr,  // quality_flags.
                        nullptr,  // beam_flags
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
  ValidateWriteSwathBathyPing("swath-bathy-ping-empty.gsf", false, 1234, ping,
                              5677);
}
#endif

void ValidateWriteSvp(const string filename, bool checksum,
                      int expected_write_size, const gsfSVP &svp,
                      int expected_file_size) {
  ASSERT_GE(expected_write_size, 36);
  ASSERT_GE(expected_file_size, 56);

  gsfRecords record;
  record.svp = svp;

  {
    gsfDataID data_id = {checksum, 0, GSF_RECORD_SOUND_VELOCITY_PROFILE, 0};
    unique_ptr<GsfFile> file = GsfFile::Open(filename, GSF_CREATE);
    ASSERT_NE(nullptr, file);
    ASSERT_EQ(expected_write_size, gsfWrite(file->handle(), &data_id, &record));
  }

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  unique_ptr<GsfFile> file = GsfFile::Open(filename, GSF_READONLY);
  ASSERT_NE(nullptr, file);
  gsfRecords read_record;
  gsfDataID data_id;
  const int num_bytes = gsfRead(file->handle(), GSF_NEXT_RECORD, &data_id,
                                &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_SOUND_VELOCITY_PROFILE, data_id.recordID);
  VerifySvp(record.svp, read_record.svp);
}

TEST(GsfWriteSimple, SvpEmpty) {
  const struct timespec observation_time = {1, 20000000};
  const struct timespec application_time = {3, 40000000};
  const gsfSVP svp =
      GsfSvp(observation_time, application_time, 5.6, 7.8, 0, nullptr, nullptr);
  ValidateWriteSvp("svp-empty.gsf", false, 36, svp, 56);
}

TEST(GsfWriteSimple, SvpLength1Zero) {
  const struct timespec observation_time = {0, 0};
  const struct timespec application_time = {0, 0};
  const double depth[] = {0.0};
  const double sound_speed[] = {0.0};
  const gsfSVP svp = GsfSvp(observation_time, application_time, 0.0, 0.0, 1,
                            depth, sound_speed);
  ValidateWriteSvp("svp-length1-0.gsf", false, 44, svp, 64);
}

TEST(GsfWriteSimple, SvpLength1) {
  const struct timespec observation_time = {1438113652, 524180000};
  const struct timespec application_time = {1438113653, 123450000};
  const double depth[] = {1.2};
  const double sound_speed[] = {2.3};
  const gsfSVP svp = GsfSvp(observation_time, application_time, 12.3, 45.6, 1,
                            depth, sound_speed);
  ValidateWriteSvp("svp-length1.gsf", true, 48, svp, 68);
}

// Cannot do negative depth or sound speed.

TEST(GsfWriteSimple, SvpLength2) {
  const struct timespec observation_time = {1438113654, 524190000};
  const struct timespec application_time = {1438113655, 123460000};
  // Cannot do negative sound speed.
  const double depth[] = {1.2, 2.3};
  const double sound_speed[] = {4.5, 6.7};
  const gsfSVP svp = GsfSvp(observation_time, application_time, -12.3, -45.6, 2,
                            depth, sound_speed);
  ValidateWriteSvp("svp-length2.gsf", false, 52, svp, 72);
}

// TODO(schwehr): GSF_RECORD_PROCESSING_PARAMETERS

void ValidateWriteProcessingParameters(const string filename, bool checksum,
                                       int expected_write_size,
                                       const gsfProcessingParameters &params,
                                       int expected_file_size) {
  ASSERT_GE(expected_write_size, 20);
  ASSERT_GE(expected_file_size, 40);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_PROCESSING_PARAMETERS, 0};
  gsfRecords record;
  record.process_parameters = params;
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));

  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_PROCESSING_PARAMETERS, data_id.recordID);
  VerifyProcessingParameters(record.process_parameters,
                             read_record.process_parameters);
}

class GsfProcessingParametersWrapper {
 public:
  GsfProcessingParametersWrapper() { params_.number_parameters = 0; }
  ~GsfProcessingParametersWrapper() {
    for (int i = 0; i < params_.number_parameters; ++i) {
      free(params_.param[i]);
    }
  }

  bool Add(string key, string value) {
    if (params_.number_parameters + 1 >= GSF_MAX_PROCESSING_PARAMETERS) {
      cerr << "GsfProcessingParameters ERROR: To many params: " << key << "\n";
      return false;
    }
    string entry = key + "=" + value;
    int i = params_.number_parameters;
    // TODO(schwehr): Use a vector of unique_ptrs with a deleter using free.
    params_.param[i] = strdup(entry.c_str());
    params_.param_size[i] = entry.size() + 1;
    params_.number_parameters++;
    return true;
  }

  gsfProcessingParameters params_;
};

TEST(GsfProcessingParameter, Empty) {
  GsfProcessingParametersWrapper param_wrapper;
  param_wrapper.params_.param_time = {0, 0};
  ValidateWriteProcessingParameters("processing-params-empty.gsf", false, 20,
                                    param_wrapper.params_, 40);
}

TEST(GsfProcessingParameter, OneSmall) {
  GsfProcessingParametersWrapper param_wrapper;
  param_wrapper.params_.param_time = {1, 2};
  param_wrapper.Add("a", "b");
  ValidateWriteProcessingParameters("processing-params-1.gsf", true, 28,
                                    param_wrapper.params_, 48);
}

TEST(GsfProcessingParameter, TwoSmall) {
  GsfProcessingParametersWrapper param_wrapper;
  param_wrapper.params_.param_time = {1, 2};
  param_wrapper.Add("a", "b");
  param_wrapper.Add("cd", "ef");
  ValidateWriteProcessingParameters("processing-params-2.gsf", false, 32,
                                    param_wrapper.params_, 52);
}

TEST(GsfProcessingParameter, OneLarge) {
  GsfProcessingParametersWrapper param_wrapper;
  param_wrapper.params_.param_time = {1, 2};
  param_wrapper.Add("abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc",
                    "deeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeef");
  ValidateWriteProcessingParameters("processing-params-large.gsf", false, 108,
                                    param_wrapper.params_, 128);
}

TEST(GsfProcessingParameter, CharacterVariety) {
  GsfProcessingParametersWrapper param_wrapper;
  param_wrapper.params_.param_time = {1, 2};
  param_wrapper.Add("12345678902134567890-_+';:\",./<>?`~\\",
                    "\\~`?></.,\":;'+_-09876543120987654321");
  ValidateWriteProcessingParameters("processing-params-variety.gsf", false, 96,
                                    param_wrapper.params_, 116);
}

// TODO(schwehr): GSF_RECORD_SENSOR_PARAMETERS

void ValidateWriteComment(const string filename, bool checksum,
                          int expected_write_size, const char *comment,
                          int expected_file_size) {
  ASSERT_GE(expected_write_size, 20);
  ASSERT_NE(nullptr, comment);
  ASSERT_GE(expected_file_size, 40);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_COMMENT, 0};
  gsfRecords record;
  record.comment = GsfComment({1, 2}, comment);
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));

  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_COMMENT, data_id.recordID);
  VerifyComment(record.comment, read_record.comment);
}

TEST(GsfWriteSimple, CommentEmpty) {
  char comment[] = "";
  ValidateWriteComment("comment-empty.gsf", false, 20, comment, 40);
}

TEST(GsfWriteSimple, CommentEmptyChecksum) {
  char filename[] = "comment-empty-checksum.gsf";
  char comment[] = "";
  ValidateWriteComment(filename, true, 24, comment, 44);
}

TEST(GsfWriteSimple, CommentUpTo5) {
  char filename1[] = "comment-1.gsf";
  char comment1[] = "a";
  ValidateWriteComment(filename1, false, 24, comment1, 44);
  char filename2[] = "comment-2.gsf";
  char comment2[] = "ab";
  ValidateWriteComment(filename2, false, 24, comment2, 44);
  char filename3[] = "comment-3.gsf";
  char comment3[] = "abc";
  ValidateWriteComment(filename3, false, 24, comment3, 44);
  char filename4[] = "comment-4.gsf";
  char comment4[] = "abcd";
  ValidateWriteComment(filename4, false, 24, comment4, 44);
  char filename5[] = "comment-5.gsf";
  char comment5[] = "abcde";
  ValidateWriteComment(filename5, false, 28, comment5, 48);
}

TEST(GsfWriteSimple, CommentLarge) {
  char filename[] = "comment-large.gsf";
  char comment[] =
      "ab"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc"
      "de";
  ValidateWriteComment(filename, false, 656, comment, 676);
}

void ValidateWriteHistory(const string filename, bool checksum,
                          int expected_write_size, const char *host_name,
                          const char *operator_name, const char *command_line,
                          const char *comment, int expected_file_size) {
  ASSERT_GE(expected_write_size, 28);
  ASSERT_NE(nullptr, host_name);
  ASSERT_LE(strlen(host_name), GSF_HOST_NAME_LENGTH);
  ASSERT_NE(nullptr, operator_name);
  ASSERT_LE(strlen(operator_name), GSF_OPERATOR_LENGTH);
  ASSERT_NE(nullptr, command_line);
  ASSERT_NE(nullptr, comment);
  ASSERT_GE(expected_file_size, 48);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_HISTORY, 0};
  gsfRecords record;
  record.history =
      GsfHistory({1, 2}, host_name, operator_name, command_line, comment);
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_HISTORY, data_id.recordID);
  VerifyHistory(record.history, read_record.history);
}

TEST(GsfWriteSimple, History) {
  ValidateWriteHistory("history-empty.gsf", false, 28, "", "", "", "", 48);
  ValidateWriteHistory("history-empty-checksum.gsf", true, 32, "", "", "", "",
                       52);
  ValidateWriteHistory("history-1.gsf", false, 32, "a", "b", "c", "d", 52);
  ValidateWriteHistory("history-longer.gsf", false, 40, "ab", "cde", "fghi",
                       "jklm", 60);
}

void ValidateWriteNavigationError(string filename, bool checksum,
                                  int expected_write_size,
                                  const gsfNavigationError &nav_error,
                                  int expected_file_size) {
  ASSERT_GE(expected_write_size, 28);
  ASSERT_GE(expected_file_size, 48);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_NAVIGATION_ERROR, 0};
  gsfRecords record;
  record.nav_error = nav_error;
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_NAVIGATION_ERROR, data_id.recordID);
  VerifyNavigationError(record.nav_error, read_record.nav_error);
}

TEST(GsfWriteSimple, NavigationErrorZero) {
  const struct timespec time = {1438113659, 123450000};
  const gsfNavigationError nav_error = GsfNavigationError(time, 0, 0, 0);
  ValidateWriteNavigationError("nav-error-0.gsf", false, 28, nav_error, 48);
}

TEST(GsfWriteSimple, NavigationErrorPos) {
  const struct timespec time = {1438114000, 123460000};
  const gsfNavigationError nav_error =
      // TODO(schwehr): What is the largest positive value that should work?
      GsfNavigationError(time, 20000000, 0x00FFFFF0, 0x00FFFE0);
  ValidateWriteNavigationError("nav-error-pos.gsf", true, 32, nav_error, 52);
}

TEST(GsfWriteSimple, NavigationErrorNegative) {
  const struct timespec time = {1438115678, 123470000};
  const gsfNavigationError nav_error =
      // TODO(schwehr): What is the smallest negative value that should work?
      GsfNavigationError(time, -1, -1234.5, -678.9);
  ValidateWriteNavigationError("nav-error-neg.gsf", false, 28, nav_error, 48);
}

// TODO(schwehr): GSF_RECORD_SWATH_BATHY_SUMMARY
// TODO(schwehr): GSF_RECORD_SINGLE_BEAM_PING
void ValidateWriteHvNavigationError(string filename, bool checksum,
                                    int expected_write_size,
                                    const gsfHVNavigationError &hv_nav_error,
                                    int expected_file_size) {
  ASSERT_GE(expected_write_size, 0);
  ASSERT_GE(expected_file_size, 0);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_HV_NAVIGATION_ERROR, 0};
  gsfRecords record;
  record.hv_nav_error = hv_nav_error;
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_HV_NAVIGATION_ERROR, data_id.recordID);
  VerifyHvNavigationError(record.hv_nav_error, read_record.hv_nav_error);
}

TEST(GsfWriteSimple, HvNavigationErrorZero) {
  const struct timespec time = {0, 0};
  const char spare[2] = {'\0', '\0'};
  const char position_type[] = "";
  const gsfHVNavigationError hv_nav_error =
      GsfHvNavigationError(time, 0, 0.0, 0.0, 0.0, spare, position_type);
  ValidateWriteHvNavigationError("hv-nav-error-0.gsf", false, 36, hv_nav_error,
                                 56);
}

TEST(GsfWriteSimple, HvNavigationErrorPositive) {
  const struct timespec time = {1438113659, 123450000};
  const char spare[2] = {'a', '1'};
  const char position_type[] = GSF_POS_TYPE_LBLN;
  const gsfHVNavigationError hv_nav_error =
      GsfHvNavigationError(time, 2, 3.4, 5.6, 6.7, spare, position_type);
  ValidateWriteHvNavigationError("hv-nav-error-0.gsf", false, 40, hv_nav_error,
                                 60);
}

TEST(GsfWriteSimple, HvNavigationErrorPositiveLarge) {
  const struct timespec time = {1438113660, 123460000};
  const char spare[2] = {'a', '1'};
  const char position_type[] = "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc";
  const gsfHVNavigationError hv_nav_error =
      GsfHvNavigationError(time, 2, 349, 5610, 171, spare, position_type);
  ValidateWriteHvNavigationError("hv-nav-error-0.gsf", false, 76, hv_nav_error,
                                 96);
}

TEST(GsfWriteSimple, HvNavigationErrorNegative) {
  const struct timespec time = {1438113777, 123470000};
  const char spare[2] = {'a', '1'};
  const unsigned char position_type[6] = {127, 190, 255, 1, 3, 0};
  const gsfHVNavigationError hv_nav_error =
      GsfHvNavigationError(time, -42, -459, -6710, 211, spare,
                           reinterpret_cast<const char *>(position_type));
  ValidateWriteHvNavigationError("hv-nav-error-0.gsf", false, 40, hv_nav_error,
                                 60);
}

// TODO(schwehr): Make filename a string here and elsewhere.
void ValidateWriteAttitude(const string filename, bool checksum,
                           int expected_write_size, const gsfAttitude &attitude,
                           int expected_file_size) {
  ASSERT_GE(expected_write_size, 20);
  ASSERT_GE(expected_file_size, 40);

  int handle;
  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_CREATE, &handle));

  gsfDataID data_id = {checksum, 0, GSF_RECORD_ATTITUDE, 0};
  gsfRecords record;
  record.attitude = attitude;
  ASSERT_EQ(expected_write_size, gsfWrite(handle, &data_id, &record));
  ASSERT_EQ(0, gsfClose(handle));

  struct stat buf;
  ASSERT_EQ(0, stat(filename.c_str(), &buf));
  ASSERT_EQ(expected_file_size, buf.st_size);

  ASSERT_EQ(0, gsfOpen(filename.c_str(), GSF_READONLY, &handle));
  ASSERT_GE(handle, 0);
  gsfRecords read_record;
  const int num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  ASSERT_EQ(expected_write_size, num_bytes);
  ASSERT_EQ(GSF_RECORD_ATTITUDE, data_id.recordID);
  VerifyAttitude(record.attitude, read_record.attitude);
}

unique_ptr<gsfAttitude> WriteAttitudeAndReturnRead(const string filename,
                                                   const gsfAttitude &attitude,
                                                   int *file_size,
                                                   int *num_bytes) {
  // TODO(schwehr): Add error handling.
  int handle;
  if (0 != gsfOpen(filename.c_str(), GSF_CREATE, &handle)) {
    return nullptr;
  }

  gsfDataID data_id = {false, 0, GSF_RECORD_ATTITUDE, 0};
  gsfRecords record;
  record.attitude = attitude;
  if (gsfWrite(handle, &data_id, &record) < 20) {
    // Do not check return since we are already in trouble.
    gsfClose(handle);
    return nullptr;
  }
  if (0 != gsfClose(handle)) {
    return nullptr;
  }

  struct stat buf;
  if (0 != stat(filename.c_str(), &buf)) {
    return nullptr;
  }
  *file_size = buf.st_size;

  if (0 != gsfOpen(filename.c_str(), GSF_READONLY, &handle)) {
    return nullptr;
  }

  gsfRecords read_record;
  *num_bytes =
      gsfRead(handle, GSF_NEXT_RECORD, &data_id, &read_record, nullptr, 0);
  if (*num_bytes < 24) {
    return nullptr;
  }

  return MakeUnique<gsfAttitude>(read_record.attitude);
}

TEST(GsfWriteSimple, AttitudeEmpty) {
  const struct timespec times[] = {{3, 4}};
  const gsfAttitude attitude =
      GsfAttitude(0, times, nullptr, nullptr, nullptr, nullptr);
  ValidateWriteAttitude("attitude-empty.gsf", false, 20, attitude, 40);
}

TEST(GsfWriteSimple, AttitudeLength1Zeros) {
  const struct timespec times0[] = {{5, 6}};
  const double data0[] = {0.0};
  const gsfAttitude attitude0 =
      GsfAttitude(1, times0, data0, data0, data0, data0);
  ValidateWriteAttitude("attitude-length1-0.gsf", false, 28, attitude0, 48);
}

TEST(GsfWriteSimple, AttitudeLength1) {
  const struct timespec times[] = {{7, 8}};
  const double pitch[] = {1.2};
  const double roll[] = {2.2};
  const double heave[] = {3.3};
  const double heading[] = {4.4};
  const gsfAttitude attitude =
      GsfAttitude(1, times, pitch, roll, heave, heading);
  ValidateWriteAttitude("attitude-length1-1-checksum.gsf", true, 32, attitude,
                        52);
}

TEST(GsfWriteSimple, AttitudeLength1Negative) {
  const struct timespec times[] = {{9, 10}};
  const double pitch[] = {-1.2};
  const double roll[] = {-3.4};
  const double heave[] = {-5.6};
  // Stored as a int16 with value * 100, so cannot test negative.
  const double heading[] = {7.8};
  const gsfAttitude attitude =
      GsfAttitude(1, times, pitch, roll, heave, heading);
  ValidateWriteAttitude("attitude-length1-neg.gsf", false, 28, attitude, 48);
}

TEST(GsfWriteSimple, AttitudeRounding) {
  const struct timespec times[] = {{11, 12}};
  const double pitch[] = {-1.002};
  const double roll[] = {-2.0052};
  const double heave[] = {3.0045};
  const double heading[] = {4.0052};
  const gsfAttitude src = GsfAttitude(1, times, pitch, roll, heave, heading);
  int file_size;
  int num_bytes;
  const unique_ptr<gsfAttitude> dst = WriteAttitudeAndReturnRead(
      "attitude-round.gsf", src, &file_size, &num_bytes);
  ASSERT_NE(nullptr, dst);
  ASSERT_EQ(48, file_size);
  ASSERT_EQ(28, num_bytes);

  const double pitch2[] = {-1.00};
  const double roll2[] = {-2.01};
  const double heave2[] = {3.00};
  const double heading2[] = {4.01};
  const gsfAttitude expected =
      GsfAttitude(1, times, pitch2, roll2, heave2, heading2);
  VerifyAttitude(expected, *dst);
}

TEST(GsfWriteSimple, AttitudeLength2) {
  // TODO(schwehr): Why do the nanoseconds not match?
  const struct timespec times[] = {{1438016822, 80000000},
                                   {1438016823, 90000000}};
  const double pitch[] = {1.2, -3.4};
  const double roll[] = {-100.1, 100.2};
  const double heave[] = {-200.2, -200.3};
  // Stored as a int16 with value * 100, so cannot test negative.
  const double heading[] = {181, 359};
  const gsfAttitude attitude =
      GsfAttitude(2, times, pitch, roll, heave, heading);
  ValidateWriteAttitude("attitude-length2.gsf", false, 40, attitude, 60);
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

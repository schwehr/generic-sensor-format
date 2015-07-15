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

#include "gsfxx.h"

#include <cassert>

#include <chrono>
#include <memory>
#include <regex>
#include <string>

using std::chrono::system_clock;
using std::regex;
using std::smatch;
using std::string;
using std::unique_ptr;

namespace gsfxx {

const char *const RECORD_STRINGS[RECORD_NUM_TYPES] = {
    "INVALID",                "HEADER",                "SWATH_BATHYMETRY_PING",
    "SOUND_VELOCITY_PROFILE", "PROCESSING_PARAMETERS", "SENSOR_PARAMETERS",
    "COMMENT",                "HISTORY",               "NAVIGATION_ERROR",
    "SWATH_BATHY_SUMMARY",    "SINGLE_BEAM_PING",      "HV_NAVIGATION_ERROR",
};

string RecordBuffer::ToString(uint32_t start, uint32_t max_length) const {
  assert(start + max_length - 1 < size_);
  const char *buf = reinterpret_cast<const char *>(buf_) + start;
  return string(buf, max_length);
}

uint32_t RecordBuffer::ToUnsignedInt32(uint32_t start) const {
  assert(start <= size_ - 4);
  const uint32_t *raw = reinterpret_cast<const uint32_t *>(
      reinterpret_cast<const char *>(buf_) + start);
  return SwapEndian(*raw);
}

// static
unique_ptr<Header> Header::Decode(const RecordBuffer &buf) {
  if (buf.type() != RECORD_HEADER || buf.size() != 12) {
    return nullptr;
  }
  regex reg("GSF-v0*([[:digit:]])+[.]0*([[:digit:]]+)");
  smatch result;
  const string version = buf.ToString(0, 12);
  if (!regex_search(version, result, reg) || result.size() != 3) {
    return nullptr;
  }
  const int major = stoi(result[1]);
  const int minor = stoi(result[2]);
  return MakeUnique<Header>(major, minor);
}

// static
unique_ptr<Comment> Comment::Decode(const RecordBuffer &buf) {
  if (buf.type() != RECORD_COMMENT || buf.size() < 12) {
    return nullptr;
  }

  // TODO(schwehr): Deal with 2038 rollover somehow.
  uint32_t sec = buf.ToUnsignedInt32(0);
  uint32_t nsec = buf.ToUnsignedInt32(4);
  system_clock::time_point time_point = SecNsecToTimePoint(sec, nsec);

  const size_t size = buf.ToUnsignedInt32(8);
  if (buf.size() < 12 + size) {
    return nullptr;
  }
  const string comment = buf.ToString(12, size);

  return MakeUnique<Comment>(time_point, comment);
}

}  // namespace gsfxx

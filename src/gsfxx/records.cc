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

#include <memory>
#include <string>

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


// static
unique_ptr<Header> DecodeHeader(const RecordBuffer &buf) {
  assert(buf.type() == RECORD_HEADER);
  assert(buf.size() >= 12);
  // TODO(schwehr): Why is the payload of the header 16?
  const string version = buf.ToString(0, 12);
  return MakeUnique<Header>(3, 6);
}

} // namespace gsfxx

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
gsfComment GsfComment(const struct timespec &when, const char *comment);
gsfHistory GsfHistory(const struct timespec &when,
                      const char *host_name,
                      const char *operator_name,
                      const char *command_line,
                      const char *comment);

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

void VerifySwathBathySummary(const gsfSwathBathySummary &expected,
                             const gsfSwathBathySummary &actual);

}  // namespace test
}  // namespace generic_sensor_format

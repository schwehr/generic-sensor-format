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

#include "gtest/gtest.h"

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace gsfxx {
namespace test {
namespace {

TEST(GsfxxUtil, SwapEndianUint32) {
  EXPECT_EQ(0x04030201, SwapEndian(0x01020304));
}

TEST(GsfxxUtil, TimePoint) {
  auto time_point = SecNsecToTimePoint(1436901964, 550947000);
  seconds s = duration_cast<seconds>(time_point.time_since_epoch());
  ASSERT_EQ(1436901964, s.count());
  ASSERT_DOUBLE_EQ(1436901964.550947, TimePointToSeconds(time_point));
}

}  // namespace
}  // namespace test
}  // namespace gsfxx

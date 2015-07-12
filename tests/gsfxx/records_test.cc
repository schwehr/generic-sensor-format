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

#include <cstdint>

#include <array>
#include <memory>

#include "gtest/gtest.h"

using std::array;
using std::unique_ptr;

namespace gsfxx {
namespace test {
namespace {

TEST(GsfxxRecordHeader, Simple) {
  const uint32_t size = 20;
  array<uint8_t, size> data = {{
    0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x47, 0x53, 0x46, 0x2d,
    0x76, 0x30, 0x32, 0x2e, 0x30, 0x39, 0x00, 0x00
  }};
  RecordBuffer buf(reinterpret_cast<void *>(&data), size, RECORD_HEADER);
  unique_ptr<Header> header = Header::Decode(buf);
  EXPECT_EQ(3, header->version_major());
  EXPECT_EQ(6, header->version_minor());
}


}  // namespace
}  // namespace test
}  // namespace gsfxx

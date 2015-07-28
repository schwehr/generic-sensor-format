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

// Tests for time functions.

#include <sys/stat.h>

#include "gsf.h"
#include "gsf_priv.h"

#include "gtest/gtest.h"

namespace generic_sensor_format {
namespace test {
namespace {

TEST(GsfTime, Add) {
  struct timespec base_time = {0, 0};
  struct timespec sum_time = {1, 2};
  LocalAddTimes(&base_time, 0.0, &sum_time);
  EXPECT_EQ(0, sum_time.tv_sec);
  EXPECT_EQ(0, sum_time.tv_nsec);

  LocalAddTimes(&base_time, 0.1, &sum_time);
  EXPECT_EQ(0, sum_time.tv_sec);
  EXPECT_EQ(100000000, sum_time.tv_nsec);

  LocalAddTimes(&base_time, 3.2, &sum_time);
  EXPECT_EQ(3, sum_time.tv_sec);
  EXPECT_EQ(200000000, sum_time.tv_nsec);

  base_time = {10, 11};
  LocalAddTimes(&base_time, 4.5, &sum_time);
  EXPECT_EQ(14, sum_time.tv_sec);
  EXPECT_EQ(500000011, sum_time.tv_nsec);
}

TEST(GsfTime, Subtract) {
  struct timespec base_time = {0, 0};
  struct timespec second_time = {0, 0};
  double difference = 1.2;
  LocalSubtractTimes(&second_time, &base_time, &difference);
  EXPECT_DOUBLE_EQ(0.0, difference);

  second_time = {3, 400000000};
  LocalSubtractTimes(&second_time, &base_time, &difference);
  EXPECT_DOUBLE_EQ(3.4, difference);
  LocalSubtractTimes(&base_time, &second_time, &difference);
  EXPECT_DOUBLE_EQ(-3.4, difference);

  base_time = {1, 300000000};
  LocalSubtractTimes(&second_time, &base_time, &difference);
  EXPECT_DOUBLE_EQ(2.1, difference);
  second_time = {1, 300000000};
  LocalSubtractTimes(&second_time, &base_time, &difference);
  EXPECT_DOUBLE_EQ(0.0, difference);
}

}  // namespace
}  // namespace test
}  // namespace generic_sensor_format

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

#include <iostream>
#include <string>
// #include <vector>

#include "gsf.h"

using namespace std;

namespace generic_sensor_format {

int GsfIndex(const string &filename) {
  int handle;
  if (GSF_NORMAL != gsfOpen(filename.c_str(), GSF_UPDATE_INDEX, &handle)) {
    cout << "ERROR\n";
  }
  return 0;
}

}  // namespace generic_sensor_format

int main(int /* argc */, char *argv[]) {
  return generic_sensor_format::GsfIndex(argv[1]);
}

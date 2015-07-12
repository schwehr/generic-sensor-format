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

//  c++ -Wall -Wextra -std=c++11 -o gsf_mmap gsf_mmap.cc
//
// TODO(schwehr): Error reporting in the file reader.

#include "gsfxx.h"

#include <cassert>
#include <cstdint>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

namespace gsfxx {

const uint32_t kChecksumMask = 0x80000000;
const uint32_t kTypeMask = 0x003FFFFF;
// TODO(schwehr): Do we need to processed the reserved?
//   const uint32_t kReservedMask = 0x7FC00000;


unique_ptr<RecordBuffer> FileReaderMmap::NextBuffer() {
  // TODO(schwehr): Check for the record being too short.
  if (offset_ >= size_) {
    // Reached EOF.
    return nullptr;
  }

  const char *start = static_cast<const char *>(data_) + offset_;
  const uint32_t data_size =
      SwapEndian(*(reinterpret_cast<const uint32_t *>(start)));
  const uint32_t record_id =
      SwapEndian(*(reinterpret_cast<const uint32_t *>(start + 4)));

  uint32_t type_num = record_id & kTypeMask;
  if (type_num >= RECORD_NUM_TYPES) {
    type_num = RECORD_INVALID;
  }
  const RecordType type = static_cast<RecordType>(type_num);
  const bool have_checksum = record_id & kChecksumMask;
  const uint32_t header_size = have_checksum ? 12 : 8;
  const void *ptr = reinterpret_cast<const void *>(start + header_size);

  offset_ += data_size + header_size;
  return MakeUnique<RecordBuffer>(ptr, data_size, type);
}

// static
unique_ptr<FileReaderMmap> FileReaderMmap::Open(const string &filename) {
  const int fd = open(filename.c_str(), O_RDONLY, 0);
  if (fd == -1) {
    return nullptr;
  }
  // TODO(schwehr): File closer.

  struct stat stat_buf;
  if (0 != stat(filename.c_str(), &stat_buf)) {
    close(fd);
    return nullptr;
  }

  const void *data = (unsigned char *)mmap(0, stat_buf.st_size, PROT_READ,
                                           MAP_FILE | MAP_PRIVATE, fd, 0);
  close(fd);
  if (data == MAP_FAILED) {
    return nullptr;
  }

  return MakeUnique<FileReaderMmap>(data, stat_buf.st_size);
}


}  // namespace gsfxx

// TODO(schwehr): Remove main.
#if 0
using namespace gsfxx;

#include <iostream>
using std::cout;

int main(int /* argc */, char * /*argv*/ []) {
  auto file = FileReaderMmap::Open("../../data/single-record/comment.gsf");
  assert(file);
  auto buf = file->NextBuffer();
  cout << "record_type: " << buf->type() << "-" << RECORD_STRINGS[buf->type()]
       << " size: " << buf->size() << "\n";
  auto header = Header::Decode(*buf);
  cout << "header: " << header->version_major() << "."
       << header->version_minor() << "\n";
  while ((buf = file->NextBuffer())) {
    cout << "record_type: " << buf->type() << "-" << RECORD_STRINGS[buf->type()]
         << "\n";
  }
  return 0;
}
#endif

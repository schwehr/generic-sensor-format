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

#include <cassert>
#include <cstdint>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <utility>

using std::string;
using std::unique_ptr;

namespace gsfxx {

uint32_t SwapEndian(uint32_t src) {
  uint32_t dst = src;
  char *ptr = reinterpret_cast<char *>(&dst);
  std::swap(ptr[0], ptr[3]);
  std::swap(ptr[1], ptr[2]);
  return dst;
}

// TODO(schwehr): Switch to make_unique when C++14 is available on Travis-CI.
template <typename T, typename... Args>
std::unique_ptr<T> MakeUnique(Args &&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

enum RecordType {
  RECORD_INVALID = 0,
  RECORD_HEADER = 1,
  RECORD_SWATH_BATHYMETRY_PING = 2,
  RECORD_SOUND_VELOCITY_PROFILE = 3,
  RECORD_PROCESSING_PARAMETERS = 4,
  RECORD_SENSOR_PARAMETERS = 5,
  RECORD_COMMENT = 6,
  RECORD_HISTORY = 7,
  RECORD_NAVIGATION_ERROR = 8,
  RECORD_SWATH_BATHY_SUMMARY = 9,
  RECORD_SINGLE_BEAM_PING = 10,
  RECORD_HV_NAVIGATION_ERROR = 11,
  RECORD_ATTITUDE = 12,
  RECORD_NUM_TYPES,  // Sentinel.
};

extern const char *const RECORD_STRINGS[RECORD_NUM_TYPES];

const char *const RECORD_STRINGS[RECORD_NUM_TYPES] = {
    "INVALID",                "HEADER",                "SWATH_BATHYMETRY_PING",
    "SOUND_VELOCITY_PROFILE", "PROCESSING_PARAMETERS", "SENSOR_PARAMETERS",
    "COMMENT",                "HISTORY",               "NAVIGATION_ERROR",
    "SWATH_BATHY_SUMMARY",    "SINGLE_BEAM_PING",      "HV_NAVIGATION_ERROR",
};

const uint32_t kChecksumMask = 0x80000000;
const uint32_t kTypeMask = 0x003FFFFF;
// TODO(schwehr): Do we need to processed the reserved?
//   const uint32_t kReservedMask = 0x7FC00000;

class RecordBuffer {
 public:
  RecordBuffer(const void *buf, uint32_t size, RecordType type)
      : buf_(buf), size_(size), type_(type){};
  uint32_t size() const { return size_; }
  RecordType type() const { return type_; }

  string ToString(uint32_t start, uint32_t max_length) const;

 private:
  const void *buf_;
  const uint32_t size_;
  const RecordType type_;
  // TODO(schwehr): mutable Error error_;
};

string RecordBuffer::ToString(uint32_t start, uint32_t max_length) const {
  assert(start + max_length - 1 < size_);
  const char *buf = reinterpret_cast<const char *>(buf_) + start;
  return string(buf, max_length);
}

class FileReaderMmap {
 public:
  FileReaderMmap(const void *data, size_t size)
      : data_(data), offset_(0), size_(size) {}
  static unique_ptr<FileReaderMmap> Open(const string &filename);
  unique_ptr<RecordBuffer> NextBuffer();

 private:
  const void *data_;
  mutable size_t offset_;
  const size_t size_;
};

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

class Record {};

class Header : Record {
 public:
  Header(int version_major, int version_minor)
      : version_major_(version_major), version_minor_(version_minor) {}
  static unique_ptr<Header> DecodeHeader(const RecordBuffer buf);
  int version_major() { return version_major_; }
  int version_minor() { return version_minor_; }

 private:
  const int version_major_;
  const int version_minor_;
};

// static
unique_ptr<Header> DecodeHeader(const RecordBuffer &buf) {
  assert(buf.type() == RECORD_HEADER);
  assert(buf.size() >= 12);
  // TODO(schwehr): Why is the payload of the header 16?
  const string version = buf.ToString(0, 12);
  return MakeUnique<Header>(3, 6);
}


} // namespace gsfxx

// TODO(schwehr): Remove main.

using namespace gsfxx;

#include <iostream>
using std::cout;

int main(int /* argc */, char * /*argv*/ []) {
  auto file = FileReaderMmap::Open("../../data/single-record/comment.gsf");
  assert(file);
  auto buf = file->NextBuffer();
  cout << "record_type: " << buf->type() << "-" << RECORD_STRINGS[buf->type()]
       << " size: " << buf->size() << "\n";
  auto header = DecodeHeader(*buf);
  cout << "header: " << header->version_major() << "."
       << header->version_minor() << "\n";
  while ((buf = file->NextBuffer())) {
    cout << "record_type: " << buf->type() << "-" << RECORD_STRINGS[buf->type()]
         << "\n";
  }
  return 0;
}

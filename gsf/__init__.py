#!/usr/bin/env python

# Copyright 2015 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Read generic sensor format (GSF) sonar files.

These files have binary records that have the data in big engian."""

import os
import struct
import sys

__version__ = '0.1'

CHECKSUM_MASK = 0x80000000
RESERVED_MASK = 0x7FC00000
TYPE_MASK = 0x003FFFFF

GSF_HEADER = 1
GSF_SWATH_BATHYMETRY_PING = 2
GSF_SOUND_VELOCITY_PROFILE = 3
GSF_PROCESSING_PARAMETERS = 4
GSF_SENSOR_PARAMETERS = 5
GSF_COMMENT = 6
GSF_HISTORY = 7
GSF_NAVIGATION_ERROR = 8
GSF_SWATH_BATHY_SUMMARY = 9
GSF_SINGLE_BEAM_PING = 10
GSF_HV_NAVIGATION_ERROR = 11
GSF_ATTITUDE = 12

RECORD_TYPES = {
  1 : 'HEADER',
  2 : 'SWATH_BATHYMETRY_PING',
  3 : 'SOUND_VELOCITY_PROFILE',
  4 : 'PROCESSING_PARAMETERS',
  5 : 'SENSOR_PARAMETERS',
  6 : 'COMMENT',
  7 : 'HISTORY',
  8 : 'NAVIGATION_ERROR',
  9 : 'SWATH_BATHY_SUMMARY',
  10 : 'SINGLE_BEAM_PING',
  11 : 'HV_NAVIGATION_ERROR',
  12 : 'ATTITUDE',
}

def _GsfHeader(data):
  print 'GsfHeader:', len(data), data
  version = ''.join(struct.unpack('%dc' % len(data), data)).rstrip('\0')
  version_major, version_minor = version.split('v')[1].split('.')
  version_major = int(version_major.lstrip('0'))
  version_minor = int(version_minor.lstrip('0'))
  return {
    'record_type': GSF_HEADER,
    'version_str': version,
    'version_major': version_major,
    'version_minor': version_minor
  }


class GsfFile(object):
  """A simple GSF file reader."""

  def __init__(self, filename):
    self.filename = filename
    self.src = open(filename)
    self.size = os.path.getsize(filename)

  def __iter__(self):
    return GsfIterator(self)


class GsfIterator(object):

  def __init__(self, gsf_file):
    self.gsf_file = gsf_file

  def __iter__(self):
    return(self)

  def next(self):
    if self.gsf_file.src.tell() >= self.gsf_file.size:
      raise StopIteration

    record_header_text = self.gsf_file.src.read(8)
    data_size = struct.unpack('>I', record_header_text[:4])[0]
    record_id = struct.unpack('>I', record_header_text[4:])[0]
    record_type = record_id & TYPE_MASK
    reserved = record_id & RESERVED_MASK
    have_checksum = record_id & CHECKSUM_MASK
    checksum = None
    if have_checksum:
      checksum_text = self.gsf_file.src.read(4)
      checksum = struct.unpack('>I', checksum_text)

    header_data = record_header_text
    if have_checksum:
      header_data += checksum_text
    data = self.gsf_file.src.read(data_size)

    record = {
      'record_type': record_type,
      'record_type_str': RECORD_TYPES[record_type],
      'reserved': reserved,
      'checksum': checksum,
      'header_data': header_data,
      'data': data
    }

    if record_type == GSF_HEADER:
      record.update(_GsfHeader(data))

    return record


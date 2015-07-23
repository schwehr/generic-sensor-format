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

import argparse

import gsf


def Hex2(val):
  return '0x' + ('%02x' % ord(val)).upper()


def Pieces(data, max_size):
  """Yield max_size components from data."""
  for i in range(0, len(data), max_size):
    yield data[i:i + max_size]


def DumpHex(filename, include_cpp=True):
  gsf_file = gsf.GsfFile(filename)

  if include_cpp:
    print 'c++ setup:'
    print
    print '  #include <array>'
    print '  using std::array;'
    print

  for record_num, record in enumerate(gsf_file):
    if record_num:
      print

    header_data = record['header_data']
    data = record['data']

    type_str = record['record_type_str']
    header_hex = [Hex2(v) for v in header_data]
    data_hex = [Hex2(v) for v in data]
    print 'record:  ', record_num, type_str
    print 'sizes  = (%d, %d, %d)' % (record['size_total'], len(header_hex), len(data_hex))
    print 'header = (', ', '.join(header_hex), ')'
    print 'data   = (', ', '.join(data_hex), ')'

    if not include_cpp:
      continue

    print 'c++ data:'
    print
    print '  // Record type:', type_str
    print '  const uint32_t size_%d = %d;' % (record_num, len(data));
    print '  array<uint8_t, size_%d> data_%d = {{' % (record_num, record_num)
    for piece in Pieces(data, 11):
      print '    ' + ', '.join([Hex2(v) for v in piece]) + ','
    print '  }};'


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('filenames', metavar='N', type=str, nargs='+',
                 help='Files to get info about.')
  args = parser.parse_args()

  for filename in args.filenames:
    DumpHex(filename)

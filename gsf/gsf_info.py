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
import collections

import gsf


def GetInfo(filename):
  gsf_file = gsf.GsfFile(filename)
  counter = collections.Counter()
  version = None
  for record in gsf_file:
    if record['record_type'] == gsf.GSF_HEADER:
      print record
      version = record['version']
    counter[record['record_type']] += 1
  print 'file:', filename
  print '  version:', version
  print '  records:'
  for key in sorted(counter):
    print '    %2d: %6d' % (key, counter[key]), gsf.RECORD_TYPES[key]


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('filenames', metavar='N', type=str, nargs='+',
                 help='Files to get info about.')
  args = parser.parse_args()

  for filename in args.filenames:
    GetInfo(filename)

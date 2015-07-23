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

import unittest

import gsf


class HistoryTest(unittest.TestCase):

  def testAnything(self):
    data = (
        0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00,
        0x0A, 0x68, 0x6F, 0x73, 0x74, 0x20, 0x6E, 0x61, 0x6D,
        0x65, 0x00, 0x00, 0x0E, 0x6F, 0x70, 0x65, 0x72, 0x61,
        0x74, 0x6F, 0x72, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x00,
        0x00, 0x0D, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64,
        0x20, 0x6C, 0x69, 0x6E, 0x65, 0x00, 0x00, 0x07, 0x63,
        0x6F, 0x6D, 0x6D, 0x65, 0x6E, 0x74)

    history = gsf.GsfHistory(''.join(chr(v) for v in data))
    self.assertEqual(history['record_type'], gsf.GSF_HISTORY)
    self.assertEqual(history['sec'], 3)
    self.assertEqual(history['nsec'], 4)
    self.assertEqual(history['name'], 'host name')
    self.assertEqual(history['operator'], 'operator NAME')
    self.assertEqual(history['command'], 'command line')
    self.assertEqual(history['comment'], 'comment')


if __name__ == '__main__':
  unittest.main()

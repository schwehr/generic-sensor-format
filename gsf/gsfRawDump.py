#! Python 3.6.2
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

#   Program dumps gsf files into records
#   record length, record type, and hex representation of the record
import os
from struct import unpack
import argparse



def DumpGSF(filename):
    #   Open dump file as Comma Separated Values file
    #   for easy import into the spreadsheet of your choice
    dumpfileName = filename + ".csv"
    infile = open(filename,"rb")
    dumpfile = open(dumpfileName,"w")
    #   step through a record at a time
    while True:
        #   read the first two INT values
        read_Length_Type = infile.read(8)
        if len(read_Length_Type)==0:
            break # End of file reached.
        recordLength,recordType = unpack(">II",read_Length_Type)
        #   Human readable
        dumpfile.write("{},{}".format(recordLength,recordType))
        #   Read through each four byte word of the record
        #   Understand that some values are two byte words and characters
        for index in range(int(recordLength/4)):
            hexVal = infile.read(4).hex()
            dumpfile.write(",0x"+ hexVal)
        #   New line at the end of the record
        dumpfile.write("\n")
    dumpfile.close()

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('filenames', metavar='N', type=str, nargs='+',
                 help='Files to get info about.')
  args = parser.parse_args()
  print(args)
  print(args.filenames)
  for filename in args.filenames:
    DumpGSF(filename)

main()

#include <cstdio>
#include <iostream>
#include <string>

#include "gsf.h"

using std::string;
// using std::vector;

namespace generic_sensor_format {

string RecordTypeStr(unsigned int record_id) {
  switch (record_id) {
  case GSF_RECORD_HEADER:
    return "GSF_RECORD_HEADER";
  case GSF_RECORD_SWATH_BATHYMETRY_PING:
    return "GSF_RECORD_SWATH_BATHYMETRY_PING";
  case GSF_RECORD_SOUND_VELOCITY_PROFILE:
    return "GSF_RECORD_SOUND_VELOCITY_PROFILE";
  case GSF_RECORD_PROCESSING_PARAMETERS:
    return "GSF_RECORD_PROCESSING_PARAMETERS";
  case GSF_RECORD_SENSOR_PARAMETERS:
    return "GSF_RECORD_SENSOR_PARAMETERS";
  case GSF_RECORD_COMMENT:
    return "GSF_RECORD_COMMENT";
  case GSF_RECORD_HISTORY:
    return "GSF_RECORD_HISTORY";
  case GSF_RECORD_NAVIGATION_ERROR:
    return "GSF_RECORD_NAVIGATION_ERROR";
  case GSF_RECORD_SWATH_BATHY_SUMMARY:
    return "GSF_RECORD_SWATH_BATHY_SUMMARY";
  case GSF_RECORD_SINGLE_BEAM_PING:
    return "GSF_RECORD_SINGLE_BEAM_PING";
  case GSF_RECORD_HV_NAVIGATION_ERROR:
    return "GSF_RECORD_HV_NAVIGATION_ERROR";
  case GSF_RECORD_ATTITUDE:
    return "GSF_RECORD_ATTITUDE";
  }
  return "UNKNOWN";
}



bool GsfInfo(std::ostream& out, const string filename) {
  int handle;
  if (0 != gsfOpen(filename.c_str(), GSF_READONLY, &handle)) {
    return false;
  }

  const int max_size = 1000000;
  unsigned char stream[max_size];
  int record_num = 0;
  gsfDataID data_id;
  gsfRecords records;
  int num_bytes;
  int offset = 0;
  num_bytes = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, stream, max_size);
  while (num_bytes >= 0) {
    int start = offset;
    offset += num_bytes;
    char out_filename[1024];
    std::sprintf(out_filename, "%03d", record_num);
    FILE *file = std::fopen(out_filename, "w");
    fwrite(stream, 1, num_bytes, file);
    fclose(file);
    out << record_num << ": [" << start << ":" << offset << "] " << num_bytes
              << " " << RecordTypeStr(data_id.recordID) << "\n";
    ++record_num;
    out << "\n";
    num_bytes
        = gsfRead(handle, GSF_NEXT_RECORD, &data_id, &records, stream, max_size);
  }
  // ASSERT_EQ(-1, num_bytes);
  if (gsfIntError() != GSF_READ_TO_END_OF_FILE) {
    return false;
  }

  return true;
}

}  // namespace generic_sensor_format

int main(int argc, char *argv[]) {
  for (int i=1; i < argc; ++i) {
    generic_sensor_format::GsfInfo(std::cout, argv[i]);
  }
  return 0;
}

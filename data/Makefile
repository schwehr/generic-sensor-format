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

clean:
	for dir in ${DIRS}; do \
	  echo $$dir ; \
	  (cd $$dir && make $@) ; \
	done
	rm -f README.html

README.html: README.md
	markdown $< > $@

OCEAN_URL := http://surveys.ngdc.noaa.gov/mgg/MB/ocean

get-original-samples:
	rm -rf tmp
	mkdir tmp
	cd tmp && wget ${OCEAN_URL}/rainier/H11121/multibeam/data/version1/MB/R1SB_2002_2002-119_438_2024.mb121.gz
	cd tmp && wget ${OCEAN_URL}/whiting/H11077/multibeam/data/version1/MB/14VB_2001-248_244_2013.mb121.gz
	cd tmp && wget ${OCEAN_URL}/rainier/H10866/multibeam/data/version1/MB/RA01_99_1999-147_011_1723.mb121.gz
	cd tmp && wget ${OCEAN_URL}/rude/F00490/multibeam/data/version1/MB/RU00_SB_2003-112_845_2036.mb121.gz
	cd tmp && wget ${OCEAN_URL}/nancy_foster/NF0706/multibeam/data/version2/MB/518.gsf.mb121.gz
	cd tmp && wget ${OCEAN_URL}/bay_hydrographer/H10934/multibeam/data/version1/MB/6_184_1440.mb121.gz
	cd tmp && wget ${OCEAN_URL}/nancy_foster/NF0406/multibeam/data/version1/MB/BuckIsland_StCroix/052-1320.mb121.gz
	cd tmp && wget ${OCEAN_URL}/okeanos_explorer/EX1502L2/multibeam/data/version2/MB/em302/0175_20150322_232639_EX1502L2_MB.gsf.mb121.gz

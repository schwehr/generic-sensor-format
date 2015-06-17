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

VERSION:=${shell cat VERSION}

default:
	@echo
	@echo "        gsf ${VERSION}"
	@echo
	@echo "Build options:"
	@echo
	@echo "  all      - build everything"
	@echo "  clean    - remove all objects and executables"
	@echo "  tar      - create a release source tar using VERSION"
	@echo
	@echo "  test     - C++ unittests"

test: all
	(cd tests && make test)

all:
	(cd src && make all)

DIST:=gsf-${shell cat VERSION}
TAR:=${DIST}.tar
tar:
	rm -f ${TAR}.bz2 ${TAR}
	rm -rf ${DIST}
	mkdir ${DIST}
	cp -rp [A-Z]* src ${DIST}/
	tar cf ${TAR} ${DIST}
	xz -9 ${TAR}
	rm -rf ${DIST}

clean:
	(cd src && make clean)
	(cd tests && make clean)

real-clean:
	rm -rf ${DIST}
	rm -rf ${TAR}


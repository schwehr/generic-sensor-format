# -*- makefile -*-

# Copyright 2015 Google Inc. All Rights Reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

all:
	(cd src && make -f Makefile all)

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
	(cd src && make -f Makefile clean)

real-clean:
	rm -rf ${DIST}
	rm -rf ${TAR}


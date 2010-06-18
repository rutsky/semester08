#
#  This file is part of enconv---character encoding converter suite.
#
#  Copyright (C) 2010  Vladimir Rutsky
#
#  enconv is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  enconv is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with enconv.  If not, see <http://www.gnu.org/licenses/>.
#

# Based on Mozilla Weave build scripts.

objdir=dist
stage_dir=$(objdir)/stage
xpi_dir=$(objdir)/xpi

enconv_version := 0.1
storage_version := 2
enconv_id := enconv@enconv.sourceforge.net

buildid ?= ${WEAVE_BUILDID}
buildid_short := $(buildid)
ifeq ($(buildid),)
  date    := $(shell date -u +%Y%m%d%H%M)
  buildid := $(date)
  buildid_short := $(date)
endif
ifeq ($(buildid),)
  $(warning Could not determine build id)
  $(warning Install hg or set WEAVE_BUILDID the checkout id)
  $(error)
endif

ifeq ($(MAKECMDGOALS),xpi)
  unpacked =\#
  jar=
  chrometarget=xpi
else
  unpacked=
  jar=\#
  chrometarget=
endif

subst_names := \
  enconv_version \
  enconv_id \
  buildid \
  buildid_short
  
export $(subst_names)
export substitute = perl -pe 's/@([^@]+)@/defined $$$$ENV{$$$$1} ? $$$$ENV{$$$$1} : $$$$&/ge'

ifneq ($(findstring MINGW,$(shell uname -s)),)
  export NO_SYMLINK = 1
endif

all: build

.PHONY: setup chrome build buildall test xpi onlyxpi clean $(xpi_files)

setup:
	mkdir -p $(objdir)
	mkdir -p $(stage_dir)
	mkdir -p $(xpi_dir)

iconv: setup
	$(MAKE) -C enconv-iconv-component stage

# stage precompiled binaries
iconv-binaries: setup
	$(MAKE) -C enconv-iconv stage

# build from source
iconv-build: setup
	$(MAKE) -C enconv-iconv build

chrome: setup
	$(MAKE) -C src $(chrometarget)

build: iconv iconv-binaries chrome

xpi_name := enconv-$(enconv_version)-fx.xpi
xpi_files := chrome defaults components modules platform \
             install.rdf chrome.manifest icon.png COPYING.txt README.txt

xpi: build onlyxpi

onlyxpi:
	rm -f $(xpi_dir)/$(xpi_name)
	cd $(stage_dir);zip -9r $(xpi_name) $(xpi_files)
	mv $(stage_dir)/$(xpi_name) $(xpi_dir)/$(xpi_name)

clean:
	rm -rf $(objdir)
	$(MAKE) -C enconv-iconv clean
	$(MAKE) -C enconv-iconv-component clean

buildall: setup
	$(MAKE) -C enconv-iconv-component buildall
	$(MAKE) -C enconv-iconv buildall
	$(MAKE) -C src buildall

help:
	@echo TODO!

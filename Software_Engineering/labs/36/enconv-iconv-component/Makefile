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

stage_dir=../dist/stage

sdkdir ?= ${MOZSDKDIR}

idl = IEnconvIconv.idl
idl_typelib = $(idl:.idl=.xpt)
idl_header = $(idl:.idl=.h)

#
# The only thing to actually build here is the IDL's .xpt/.h form, which
# requires an SDK. So don't do that unless explicitly requested, and use
# the files checked into Mercurial instead.
#
all: build

buildall: build

build: $(idl_typelib) $(idl_header) stage

# No SDK is needed unless you're modifying the IDL interface, in which
# case we'll need to rebuild the .h and .xpt files.
xpidl = $(sdkdir)/bin/xpidl
ifdef CROSS_COMPILE
xpidl = $(sdkdir)/host/bin/host_xpidl
endif
$(idl_typelib): $(idl)
ifeq ($(sdkdir),)
	$(warning No 'sdkdir' variable given)
	$(warning It should point to the location of the Gecko SDK)
	$(warning For example: "make sdkdir=/foo/bar/baz")
	$(warning Or set the MOZSDKDIR environment variable to point to it)
	$(error)
else
	$(xpidl) -m typelib -I$(sdkdir)/idl $(@:.xpt=.idl)
endif

$(idl_header): $(idl)
ifeq ($(sdkdir),)
	$(warning No 'sdkdir' variable given)
	$(warning It should point to the location of the Gecko SDK)
	$(warning For example: "make sdkdir=/foo/bar/baz")
	$(warning Or set the MOZSDKDIR environment variable to point to it)
	$(error)
else
	$(xpidl) -m header -I$(sdkdir)/idl $(@:.h=.idl)
endif

stage:
	mkdir -p $(stage_dir)/components
ifdef NO_SYMLINK
	cp -v $(idl_typelib) $(stage_dir)/components
	cp -v $(idl_header)  $(TOPSRCDIR)/enconv-iconv/src
	#cp -v EnconvIconv.js $(stage_dir)/components
else
	ln -vsf `pwd`/$(idl_typelib) $(stage_dir)/components
	ln -vsf `pwd`/$(idl_header)  $(TOPSRCDIR)/enconv-iconv/src
	#ln -vsf `pwd`/EnconvIconv.js $(stage_dir)/components
endif

clean:
	rm -f $(TOPSRCDIR)/enconv-iconv/src/$(idl_header)
	rm -f $(idl_typelib)
	rm -f $(idl_header)
	# maybe hg revert the .xpt/.h?

.PHONY: all buildall build stage clean

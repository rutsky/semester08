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

destdir = ../dist/stage

bundle_dirs = chrome components defaults modules data

dotin_files := $(shell find . -type f -name \*.in)
dotin_files := $(dotin_files:.in=)

all: build

buildall: build

.PHONY: $(bundle_dirs) $(dotin_files) jar subst xpi buildall

build: $(bundle_dirs) subst

$(bundle_dirs):
ifdef NO_SYMLINK
	cp -R -v $@ $(destdir)
else
	find $@ -type d -exec mkdir -p $(destdir)/{} \;
	find $@ -type f -exec ln -sf `pwd`/{} $(destdir)/{} \;
endif

$(dotin_files): $(dotin_files:=.in)
	$(substitute) $@.in > $(destdir)/$@
	rm -f $(destdir)/$@.in

jar:
	#cd $(destdir)/chrome;rm -f enconv.jar;zip -0r enconv.jar *

subst: $(dotin_files)

xpi: build jar

#!/bin/bash

pushd enconv@enconv.sourceforge.net
FILE="../enconv-0.1-fx.xpi"
[ -f $FILE ] && rm $FILE
zip -r $FILE .
popd

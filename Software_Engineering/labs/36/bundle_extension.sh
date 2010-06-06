#!/bin/bash

pushd encodingconverter@enconv.sourceforge.net
FILE="../encodingconverter.xpi"
[ -f $FILE ] && rm $FILE
zip -r $FILE .
popd

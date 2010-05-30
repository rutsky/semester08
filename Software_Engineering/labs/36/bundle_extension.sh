#!/bin/bash

pushd encodingconverter@rutsky.ru
FILE="../encodingconverter.xpi"
[ -f $FILE ] && rm $FILE
zip -r $FILE .
popd

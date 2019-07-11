#!/bin/sh

mkdir ../temp
mkdir ../temp/bin
cp ../data --recursive ../temp
cp Salmon ../temp/bin
tar cfvz Archive.tar.gz -C ../temp/ bin data


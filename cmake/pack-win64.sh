#!/bin/sh

mkdir ../temp
mkdir ../temp/bin
cp ../data --recursive ../temp
cp Salmon.exe ../temp/bin
cp ../dependencies/win64/lib/*.dll ../temp/bin
cd ../temp/
zip -r ../build/Archive.zip bin data


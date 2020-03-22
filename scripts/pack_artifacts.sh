#!/bin/bash
set -e
usage="Usage: ./pack_artifacts.sh [PLATFORM] [BIT] [VERSION]
PLATFORM: \"linux\", \"windows\"
BIT: \"64\", \"32\"
VERSION: any string"
if [ "$1" ==  "--help" ] || [ "$1" == "-h" ]
then
    echo "$usage"
    exit
fi

if [ "$1" ]
then
    P=$1
else
    P=$PLATFORM
fi

if [ "$2" ]
then
    B=$2
else
    B=$BIT
fi

if [ "$3" ]
then
    TRAVIS_TAG=$4
fi

if [ "$P" != "linux" ] && [ "$P" != "windows" ]
then
    echo "PLATFORM parameter or env var is not properly set! Value is: \"${P}\"!"
    echo "Please check help page via \"./pack_artifacts -h\""
    exit 1
fi
if [ "$B" != "64" ] && [ "$B" != "32" ]
then
    echo "BIT parameter or env var is not properly set! Value is: \"${B}\"!"
    echo "Please check help page via \"./pack_artifacts -h\""
    exit 1
fi

cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../
mkdir bin
if [ "$P" = "linux" ]
then
    tar cfvz ./RawSalmonLib-${TRAVIS_TAG}-${P}${B}.tar.gz include -C ./scripts fetch_dependencies.sh -C ../lib libSalmon.so -C ../ libSalmon_objecttypes.xml

elif [ "$P" = "windows" ]
then
    cp ./dependencies/win${B}/lib/*.dll ./bin
    cp ./lib/libSalmon.dll ./bin
    cp ./lib/libSalmon.dll.a ./bin
    zip -r ./RawSalmonLib-${TRAVIS_TAG}-${P}${B}.zip ./bin ./include ./libSalmon_objecttypes.xml
else
    echo "Please define env var \"PLATFORM\" eiter with \"linux\" or \"windows\""
    exit 1
fi
rm -r bin
echo "Successfully packed artifacts at root level"

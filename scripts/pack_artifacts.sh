#!/bin/bash
usage="Usage: ./pack_artifacts.sh [PLATFORM] [BIT] [LIBRARY] [VERSION]
PLATFORM: \"linux\", \"windows\"
BIT: \"64\", \"32\"
LIBRARY: \"ON\", \"OFF\"
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
    L=$3
else
    L=$LIBRARY
fi

if [ "$4" ]
then
    TRAVIS_TAG=$4
fi

if [ "$P" != "linux" ] && [ "$P" != "windows" ]
then
    echo "PLATFORM parameter or env var is not properly set! Value is: \"${P}\"!"
    echo "Please check help page via \"./pack_artifacts -h\""
    exit
fi
if [ "$B" != "64" ] && [ "$B" != "32" ]
then
    echo "BIT parameter or env var is not properly set! Value is: \"${B}\"!"
    echo "Please check help page via \"./pack_artifacts -h\""
    exit
fi
if [ "$L" != "ON" ] && [ "$L" != "OFF" ]
then
    echo "LIBRARY parameter or env var is not properly set! Value is: \"${L}\"!"
    echo "Please check help page via \"./pack_artifacts -h\""
    exit
fi

cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../
mkdir bin
if [ "$P" = "linux" ]
then
    if [ "$L" = "ON" ]
    then
        tar cfvz ./RawSalmonLib-${TRAVIS_TAG}-${P}${B}.tar.gz -C ./src/ include -C ../cmake fetch_dependencies.sh -C ../build libSalmon.so -C ../data libSalmon_objecttypes.xml
    elif [ "$L" = "OFF" ]
    then
        cp ./build/Salmon ./bin
        tar cfvz ./RawSalmonEngine-${TRAVIS_TAG}-${P}${B}.tar.gz bin data -C ./cmake fetch_dependencies.sh startGame.sh
    else
        echo "Please define env var \"LIBRARY\" eiter with \"ON\" or \"OFF\""
        exit
    fi
elif [ "$P" = "windows" ]
then
    cp ./dependencies/win${B}/lib/*.dll ./bin
    if [ "$L" = "ON" ]
    then
        cp ./build/libSalmon.dll ./bin
        cp ./build/libSalmon.dll.a ./bin
        cp ./data/libSalmon_objecttypes.xml ./bin
        cp -r ./src/include ./
        zip -r ./RawSalmonLib-${TRAVIS_TAG}-${P}${B}.zip ./bin ./include
        rm -r ./include
    elif [ "$L" = "OFF" ]
    then
        cp ./build/Salmon.exe ./bin
        zip -r ./RawSalmonEngine-${TRAVIS_TAG}-${P}${B}.zip ./bin ./data
    else
        echo "Please define env var \"LIBRARY\" eiter with \"ON\" or \"OFF\""
        exit
    fi
else
    echo "Please define env var \"PLATFORM\" eiter with \"linux\" or \"windows\""
    exit
fi
rm -r bin
echo "Successfully packed artifacts at root level"
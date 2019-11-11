#!/bin/bash
usage="Usage: ./pack_artifact.sh [PLATFORM] [BIT] [LIBRARY] [VERSION]
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

cd "$(dirname "${BASH_SOURCE[0]}")"
cd ../
mkdir bin
if [ "$P" = "linux" ]
then
    if [ "$L" = "ON" ]
    then
        cp -r ./src/include ./
        tar cfvz ./RawSalmonLib-${TRAVIS_TAG}-${P}${B}.tar.gz include -C ./cmake fetch_dependencies.sh -C ../build libSalmon.so
        rm -r ./include
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
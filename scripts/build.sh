#!/bin/bash
set -e
usage="Usage: ./build.sh [CONFIG] [PLATFORM] [BIT] [VERSION]
CONFIG: \"Release\", \"Debug\", \"Profile\"
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
    export CONFIG="$1"
fi
if [ "$2" ]
then
    export PLATFORM="$2"
fi
if [ "$3" ]
then
    export BIT="$3"
fi
if [ "$4" ]
then
    export TRAVIS_TAG="$4"
fi

if [ "$CONFIG" != "Release" ] && [ "$CONFIG" != "Debug" ] && [ "$CONFIG" != "Profile" ]
then
    echo "CONFIG parameter or env var is not properly set! Value is: \"${CONFIG}\"!"
    echo "Please check help page via \"./build -h\""
    exit
fi
if [ "$PLATFORM" != "linux" ] && [ "$PLATFORM" != "windows" ]
then
    echo "PLATFORM parameter or env var is not properly set! Value is: \"${PLATFORM}\"!"
    echo "Please check help page via \"./build -h\""
    exit
fi
if [ "$BIT" != "64" ] && [ "$BIT" != "32" ]
then
    echo "BIT parameter or env var is not properly set! Value is: \"${BIT}\"!"
    echo "Please check help page via \"./build -h\""
    exit
fi

cd "$(dirname "${BASH_SOURCE[0]}")"
./fetch_dev_dependencies.sh
./compile.sh
./pack_artifacts.sh
./clean_up.sh
echo "Successfully built project!"

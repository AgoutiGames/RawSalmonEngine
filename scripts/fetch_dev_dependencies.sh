#!/bin/bash
set -e
usage="Usage: ./fetch_dependencies.sh [PLATFORM] [BIT]
PLATFORM: \"linux\", \"windows\"
BIT: \"64\", \"32\""
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

if [ "$P" != "linux" ] && [ "$P" != "windows" ]
then
    echo "PLATFORM parameter or env var is not properly set! Value is: \"${P}\"!"
    echo "Please check help page via \"./fetch_dependencies -h\""
    exit 1
fi
if [ "$B" != "64" ] && [ "$B" != "32" ]
then
    echo "BIT parameter or env var is not properly set! Value is: \"${B}\"!"
    echo "Please check help page via \"./fetch_dependencies -h\""
    exit 1
fi

if [ "$P" == "linux" ]
then
    if [ "$B" == "64" ]
    then
        sudo apt-get install libtinyxml2-dev zlib1g-dev libb64-dev libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
        exit
    elif [ "$B" == "32" ]
    then
        sudo apt-get install libglib2.0-dev:i386
        sudo apt-get install libpulse-dev:i386
        sudo apt-get install gcc-multilib g++-multilib zlib1g-dev:i386 libb64-dev:i386 libtinyxml2-dev:i386
        sudo apt-get install libsdl2-dev:i386 libsdl2-image-dev:i386 libsdl2-mixer-dev:i386 libsdl2-ttf-dev:i386
        exit
    else
        echo "Please define env var \"BIT\" eiter with \"64\" or \"32\""
        exit
    fi
elif [ "$P" == "windows" ]
then
    sudo apt-get install mingw-w64
    exit
else
    echo "Please define env var \"PLATFORM\" eiter with \"linux\" or \"windows\""
    exit
fi
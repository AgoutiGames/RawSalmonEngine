#!/bin/bash
set -e
usage="Usage: ./clean_up.sh
Simply erase the build folder at root"
if [ "$1" ==  "--help" ] || [ "$1" == "-h" ]
then
    echo "$usage"
    exit
fi

cd "$(dirname "${BASH_SOURCE[0]}")"
rm -r ../build
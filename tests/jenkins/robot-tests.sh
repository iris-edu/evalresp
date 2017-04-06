#!/bin/bash -ex

if [ ! -d src ]; then
    echo "run this script in the top level directory"
    exit 1
fi

# install robot in a virtualenv
# uncomment the rm below to save time by re-using if it already exists
rm -fr env
if [ ! -d env ]; then
    virtualenv env
    . ./env/bin/activate
    pip install --upgrade robotframework
    pushd tests/robot/lib
    python setup.py install
    popd
else
    echo "reusing existing robot virtualenv"
    . ./env/bin/activate
fi    

# under jenkins, WORKSPACE is set automatically, but otherwise
# this script will be run there anyway...
pwd=`pwd`; WORKSPACE=${WORKSPACE:-$pwd}
TMP=${TMP:-/tmp}
PATH="${PATH}:${WORKSPACE}/install/bin"

cd tests/robot
rm -fr run; mkdir run
WORKSPACE=${WORKSPACE} robot all
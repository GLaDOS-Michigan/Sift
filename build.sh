#!/bin/bash -x

# Make sure we exit if there is a failure
set -e

# Initialize submodules
git submodule update --init --recursive

# Building ic3po
cd ic3po

# Build and install Yices 2
echo "building yices2"
pushd .
sudo apt-get install -y libgmp-dev gperf
git clone https://github.com/aman-goel/yices2.git
cd yices2
autoconf
./configure
make
sudo make install
pip install yices
popd

echo "building pysmt"
pushd .
pip install $(pwd)/pysmt
cd pysmt
python2 install.py --force --z3 --confirm-agreement
popd

echo "building ivy"
pushd .
sudo apt-get install g++ cmake python-ply python-pygraphviz git python-tk tix pkg-config libssl-dev python-pydot python-pydot-ng
cd ivy
# Use a customized ivy with vmt translation
git pull https://github.com/GLaDOS-Michigan/ivy.git sift

python2 build_submodules.py
sudo python2 setup.py install
popd

RETURN="$?"
if [ "${RETURN}" != "0" ]; then
    echo "Building dependencies failed!"
    exit 1
fi

#!/bin/sh

PREFIX=/usr/local
SRC_PREFIX=/tmp
DARWIN_ARCH=$(uname -a |awk '{ print $15 }')
DARWIN_RELEASE=$(uname -a |awk '{ print $3 }')
export LDFLAGS="-L/opt/local/lib"
export CPPFLAGS="-I/opt/local/include"

set -e

# sudo port install autoconf
# sudo port install automake
# sudo port install libtool
# sudo port install git-core

mkdir -pv $SRC_PREFIX
cp -f $(dirname $0)/simavr.patch $SRC_PREFIX

if [ ! -d $SRC_PREFIX/simavr ]; then
	cd $SRC_PREFIX
	git clone git://github.com/buserror-uk/simavr
fi
cd $SRC_PREFIX/simavr

git reset --hard
git clean -xdf
git pull
git apply ../simavr.patch 

make clean
make all

obj_dir=simavr/obj-${DARWIN_ARCH}-apple-darwin${DARWIN_RELEASE}
mv $obj_dir/run_avr.elf $obj_dir/simavr
sudo install -m 755 -o root -g wheel $obj_dir/simavr $PREFIX/bin	
sudo install -m 644 -o root -g wheel $obj_dir/libsimavr.a $PREFIX/lib	
sudo install -m 644 -o root -g wheel simavr/sim/*.h $PREFIX/include	
sudo install -d -m 755 -o root -g wheel $PREFIX/include/avr
sudo install -m 644 -o root -g wheel simavr/sim/avr/*.h $PREFIX/include/avr

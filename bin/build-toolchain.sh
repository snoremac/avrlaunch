#!/bin/sh

PREFIX=/usr/local/avr
SRC_PREFIX=/usr/local/src/avr
DARWIN_ARCH=$(uname -a |awk '{ print $15 }')
DARWIN_RELEASE=$(uname -a |awk '{ print $3 }')
export PATH=$PREFIX/bin:$PATH
export LDFLAGS="-L/opt/local/lib"
export CPPFLAGS="-I/opt/local/include"

BINUTILS=2.19.1
GMP=4.2.4
MPFR=2.4.2
GCC=4.3.6
AVR_LIBC=1.7.1
AVRDUDE=5.10
GDB=6.8

set -e

clean() {
	sudo rm -rfv $PREFIX
	rm -rfv $SRC_PREFIX
}

deps() {
	sudo port install wget
	sudo port install autoconf
	sudo port install automake
	sudo port install libtool
	sudo port install git-core

	mkdir -pv $SRC_PREFIX
	sudo mkdir -pv $PREFIX
	cp -f $(dirname $0)/*.patch $SRC_PREFIX
}

binutils() {
	if [ ! -f /tmp/binutils-$BINUTILS.tar.bz2 ]; then
		wget -O /tmp/binutils-$BINUTILS.tar.bz2 http://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS}a.tar.bz2
	fi
	bunzip2 -ck /tmp/binutils-$BINUTILS.tar.bz2 | tar xvf - -C $SRC_PREFIX
	cd $SRC_PREFIX/binutils-$BINUTILS

	./configure --prefix=$PREFIX --target=avr --enable-install-libbfd --disable-nls --enable-werror=no
	make
	sudo make install
}

gmp() {
	if [ ! -f /tmp/gmp-$GMP.tar.bz2 ]; then
		wget -O /tmp/gmp-$GMP.tar.bz2 http://ftp.gnu.org/gnu/gmp/gmp-$GMP.tar.bz2
	fi
	bunzip2 -ck /tmp/gmp-$GMP.tar.bz2 | tar xvf - -C $SRC_PREFIX
	cd $SRC_PREFIX/gmp-$GMP

	./configure --prefix=$PREFIX
	make
	sudo make install
	make check
}

mpfr() {
	if [ ! -f /tmp/mpfr-$MPFR.tar.bz2 ]; then
		wget -O /tmp/mpfr-$MPFR.tar.bz2 http://ftp.gnu.org/gnu/mpfr/mpfr-$MPFR.tar.bz2
	fi
	bunzip2 -ck /tmp/mpfr-$MPFR.tar.bz2 | tar xvf - -C $SRC_PREFIX
	cd $SRC_PREFIX/mpfr-$MPFR

	./configure --prefix=$PREFIX --with-gmp=$PREFIX
	make
	sudo make install
}

gcc() {
	if [ ! -f /tmp/gcc-$GCC.tar.bz2 ]; then
		wget -O /tmp/gcc-$GCC.tar.bz2 http://ftp.gnu.org/gnu/gcc/gcc-$GCC/gcc-$GCC.tar.bz2
	fi
	bunzip2 -ck /tmp/gcc-$GCC.tar.bz2 | tar xvf - -C $SRC_PREFIX
	cd $SRC_PREFIX/gcc-$GCC

	patch -p0 < ../gcc-4.3.6.patch 
	mkdir -p build
	cd build
	../configure --prefix=$PREFIX --target=avr --enable-languages=c,c++ \
		--disable-dependency-tracking --disable-nls --disable-werror --disable-libssp \
		--with-gmp=$PREFIX --with-mpfr=$PREFIX --with-dwarf2
	make
	sudo make install
}

avrlibc() {	
	if [ ! -f /tmp/avr-libc-$AVR_LIBC.tar.bz2 ]; then
		wget -O /tmp/avr-libc-$AVR_LIBC.tar.bz2 http://download.savannah.gnu.org/releases/avr-libc/avr-libc-$AVR_LIBC.tar.bz2
	fi
	bunzip2 -ck /tmp/avr-libc-$AVR_LIBC.tar.bz2 | tar xvf - -C $SRC_PREFIX
	cd $SRC_PREFIX/avr-libc-$AVR_LIBC

	./bootstrap
	./configure --prefix=$PREFIX --host=avr
	make
	sudo make install
}

avrdude() {
	if [ ! -f /tmp/avrdude-$AVRDUDE.tar.gz ]; then
		wget -nc -O /tmp/avrdude-$AVRDUDE.tar.gz http://download.savannah.gnu.org/releases/avrdude/avrdude-$AVRDUDE.tar.gz
	fi
	tar xvzf /tmp/avrdude-$AVRDUDE.tar.gz -C $SRC_PREFIX
	cd $SRC_PREFIX/avrdude-$AVRDUDE

	mkdir -p build
	cd build
	../configure --prefix=$PREFIX
	make
	sudo make install
}

gdb() {
	if [ ! -f /tmp/gdb-$GDB.tar.bz2 ]; then
		wget -O /tmp/gdb-$GDB.tar.bz2 http://ftp.gnu.org/gnu/gdb/gdb-${GDB}a.tar.bz2
	fi
	bunzip2 -ck /tmp/gdb-$GDB.tar.bz2 | tar xvf - -C $SRC_PREFIX
	cd $SRC_PREFIX/gdb-$GDB

	mkdir -p build
	cd build
	../configure --prefix=$PREFIX --target=avr --enable-werror=no
	make
	sudo make install
}

simavr() {
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
  sudo install -m 644 -o root -g wheel simavr/sim/avr/*.h $PREFIX/include/avr}

clean
deps
binutils
gmp
mpfr
gcc
avrlibc
avrdude
gdb
simavr

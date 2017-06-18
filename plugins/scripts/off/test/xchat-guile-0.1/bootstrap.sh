#! /bin/sh

export ACLOCAL="aclocal -I ./m4"
autoreconf -fisv

echo "Running configure with no arguments"
./configure

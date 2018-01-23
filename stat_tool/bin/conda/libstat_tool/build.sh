set -ve

scons cpp --prefix=$PREFIX -j$CPU_COUNT --visibility=default

set +ve
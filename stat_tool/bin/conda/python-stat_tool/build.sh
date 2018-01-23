set -ve

scons py --prefix=$PREFIX -j$CPU_COUNT --visibility=default

set +ve
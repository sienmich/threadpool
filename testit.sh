tar -xf ms406340.tar.gz
rm -r build
mkdir build && cd build
cmake ../ms406340
make
make test
cd ..
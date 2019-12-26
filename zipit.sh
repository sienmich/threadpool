rm ms406340.tar.gz
rm -r ms406340
mkdir ms406340
cp *.h ms406340/
cp *.c ms406340/
cp CMakeLists.txt ms406340/CMakeLists.txt
cp -r test ms406340/test
tar -czf ms406340.tar.gz ms406340
rm -r ms406340

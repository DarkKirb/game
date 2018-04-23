set -ex
rm -rf build
rm -rf out
mkdir out
mkdir build
cd build
cmake ..
make
mv game ../out
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../DevkitArm3DS.cmake ..
make
mv game.cia ../out
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../DevkitPpcWii.cmake ..
make
mv game.dol ../out

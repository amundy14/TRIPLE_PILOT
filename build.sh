#x86_64-w64-mingw32-gcc service.c -municode -Wall -Werror -s -o iso/service.exe
#genisoimage iso > iso.iso

PROJECTNAME="TWIN_PILOT"

mkdir iso

cd $PROJECTNAME/build
cmake ..
make

rsync -r --info=progress2 ./* ../../iso/
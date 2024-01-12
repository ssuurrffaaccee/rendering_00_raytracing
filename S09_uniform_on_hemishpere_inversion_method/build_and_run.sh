mkdir -p ./out
g++ --std=c++17  -I. ./main.cpp   -o ./out/e.exe 
cd ./out
./e.exe
rm ./e.exe
# ppmtojpeg ./xxxx.ppm > xxx.jpg 
# rm xxxx.ppm
cd ..
rmdir out
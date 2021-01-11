在example目录下进行编译
编译命令
gcc   scan_image.c -o main  -L../lib -lzbar -lpng -lpthread


如果报 png.h 的错，则安装一下
sudo apt-get install libpng-dev
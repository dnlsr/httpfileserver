#!/bin/bash
echo "сборка проекта..."
cd ~/httpserver
rm -rf build
mkdir build
cd build
cmake ..
make
cd ..
echo "сборка завершена"
ls -la build/http-server
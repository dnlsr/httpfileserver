#!/bin/bash
echo "сборка для ARM..."
cd ~/httpserver
arm-linux-gnueabihf-g++ -std=c++17 -Wall -Wextra -pthread -I. \
    src/main.cpp -o http-server-arm -lpthread -static 2>/dev/null
echo "ARM версия создана"
tar -czf http-server-armv7.tar.gz http-server-arm
echo "архив создан"
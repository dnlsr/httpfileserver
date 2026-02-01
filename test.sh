#!/bin/bash
echo "тестирование endpoints..."
cd ~/httpserver

echo "проверка root:"
./build/http-server 2>&1
echo ""

sudo ./build/http-server &
PID=$!
sleep 2

echo ""
echo "проверка /info и /log:"
echo "wget -q -O - http://localhost:1616/info"
wget -q -O - http://localhost:1616/info
echo "wget -q -O - http://localhost:1616/log"
wget -q -O - http://localhost:1616/log

echo ""
echo "проверка /upload через wget:"
echo "тест" > t.txt
echo "wget --post-file=t.txt http://localhost:1616/upload -O"
wget --post-file=t.txt http://localhost:1616/upload -O - 2>/dev/null
ls -la /tmp

echo ""
echo "остановка сервера..."
sudo kill $PID > /dev/null 2>&1
rm -f t.txt
echo "тест завершен"
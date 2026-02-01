# http file server для armv7
http сервер для загрузки файлов через wget

## требования
компилятор: C++17 (g++ 7+)
для arm: кросс-компилятор arm-linux-gnueabihf
установка зависимостей: sudo apt install g++ cmake wget
веб-сервер: httplib.h

## возможности
- проверка прав root при запуске
- HTTP сервер на порту 1616
- /info проверка работы
- /log логи сервера
- /upload загрузка файлов через wget
- сохранение файлов в /tmp
- поддержка ARMv7

### сборка и тестирование и запуск сервера для Linux
	./build.sh
	./test.sh
	sudo ./http-server
	
### сборка и запуск для ARMv7
	./build_arm.sh
	sudo ./http-server-arm
	
### проверка работы
	wget -O - http://localhost:1616/info

### просмотр логов
	wget -O - http://localhost:1616/log

### загрузка файла
	wget --auth-no-challenge --no-check-certificate --post-file=test.txt http://localhost:1616/upload
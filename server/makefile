CC = g++

STATES = include/States.h

PACKAGE = include/Package.h

main_server: src/Storage.o src/PackageManager.o src/NetworkModule.o src/Server.o src/main.o
	$(CC) -o main_server src/main.o src/Server.o src/Storage.o src/PackageManager.o src/NetworkModule.o 
	make clean

main.o: src/main.cpp
	$(CC) -c src/main.o src/main.cpp

Server.o: src/Server.cpp include/Server.h
	$(CC) -c src/Server.cpp include/Server.h

Storage.o: src/Storage.cpp include/Storage.h
	$(CC) -c src/Storage.cpp include/Storage.h

PackageManager.o: src/PackageManager.cpp include/PackageManager.h
	$(CC) -c src/PackageManager.cpp include/PackageManager.h

NetworkModule.o: src/NetworkModule.cpp include/NetworkModule.h
	$(CC) -c src/NetworkModule.cpp include/NetworkModule.h

.PHONY clean:
	rm src/*.o
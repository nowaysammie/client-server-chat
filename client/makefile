CC = g++

STATES = include/States.h

PACKAGE = include/Package.h

main_client: src/ClientStorage.o UI.o src/PackageManager.o src/NetworkModule.o src/Client.o src/main.o
	$(CC) -o main_client src/main.o src/Client.o src/ClientStorage.o src/PackageManager.o src/NetworkModule.o UI.o
	make clean

main.o: src/main.cpp
	$(CC) -c src/main.o src/main.cpp

Client.o: src/Client.cpp include/Client.h
	$(CC) -c src/Client.cpp include/Client.h

ClientStorage.o: src/ClientStorage.cpp include/ClientStorage.h
	$(CC) -c src/ClientStorage.cpp include/ClientStorage.h

PackageManager.o: src/PackageManager.cpp include/PackageManager.h
	$(CC) -c src/PackageManager.cpp include/PackageManager.h

NetworkModule.o: src/NetworkModule.cpp include/NetworkModule.h
	$(CC) -c src/NetworkModule.cpp include/NetworkModule.h

UI.o: src/UI.cpp include/UI.h
	$(CC) -c src/UI.cpp include/UI.h

.PHONY clean:
	rm src/*.o
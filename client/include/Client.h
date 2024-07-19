#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkModule.h"
#include "ClientStorage.h"
#include "PackageManager.h"
#include "UI.h"
#include "Package.h"

class Client
{
private:
	NetworkModule network_module;
	ClientStorage client_storage;
	PackageManager package_manager;
	UI ui;
	pollfd *fds;
	uint32_t my_uid;					 // стандартный поток ввода и серверный сокет
	void disconnect();					 // закрывает сокет
	uint8_t getMessage(char *buffer);	 // получить сообщение
	uint8_t sendMessage(struct Package); // отправить сообщение
public:
	uint8_t init(char *server_ip);		// происходит инициализация, создаётся сокет
	uint8_t connectToServer();			// попытка присоединиться к серверу
	uint8_t toPoll();					// отслеживает стандартный поток ввода и серверный сокет
	void printStartScreen(char *login); // вывод Welcome и запрос логина
	uint8_t eventHandler(pollfd fd);	// обрабатывает либо данные с потока ввода, либо присланные данные, взаимодействует с PackageManager, ClientStorage и UI
};

#endif
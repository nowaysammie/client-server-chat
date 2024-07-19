#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <map>
#include <string>
#include "NetworkModule.h"
#include "Storage.h"
#include "PackageManager.h"
#include "Package.h"
#include "States.h"

#define LOGIN_SIZE_MAX 50

class Server
{
private:
	NetworkModule network_module;
	Storage storage;
	PackageManager package_manager;
	void shutdown();														   // закрывает сокет
	void sendErrorPackage(int32_t client_socket, uint8_t error_code);		   // отправить клиенту сообщение об ошибке
	uint8_t instruction();													   // обработка команды администратора сервера из потока ввода
	uint8_t authorization(Package package, int32_t client_socket);			   // обработка попытки авторизации клиента
	void sendUserList(Package package, int32_t client_socket);				   // обработка запроса списка подключённых пользователей от клиента
	uint8_t forwardMsg(Package package, uint32_t client_socket, char *buffer); // добавить отправителя в отслеживаемые(Storage.appendFriend), проверить на ошибки, отправить получателю
public:
	uint8_t init(char *server_ip); // запуск сервера, бинд к порту
	uint8_t toPoll();			   // запускает network_module.toPoll()
	uint8_t eventHandler();		   // делает reevent=0, обрабатывает событие, взаимодействует с PackageManager и Storage, возвращает статус
};

#endif
#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "NetworkModule.h"
#include "Storage.h"
#include "PackageManager.h"
#include "Package.h"

class Server
{
private:
	NetworkModule network_module;
	Storage storage;
	PackageManager package_manager;
	void shutdown();														// закрывает сокет
	void sendErrorPackage(int32_t client_socket, uint8_t error_code);		// отправить клиенту сообщение об ошибке
	uint8_t instruction();													// обработка команды администратора сервера из потока ввода
	void authorization(Package package, int32_t client_socket);				// обработка попытки авторизации клиента
	void sendUserList(Package package, int32_t client_socket);				// обработка запроса списка подключённых пользователей от клиента
	void forwardMsg(Package package, uint32_t client_socket, char *buffer); // добавить отправителя в отслеживаемые(Storage.appendFriend), проверить на ошибки, отправить получателю
	void notifyFriends(uint32_t client_uid, std::set<uint32_t> waiting_friends);

public:
	bool testIp(std::string ip);
	uint8_t init(char *server_ip); // запуск сервера, бинд к порту
	uint8_t toPoll();			   // запускает network_module.toPoll()
	uint8_t eventHandler();		   // делает reevent=0, обрабатывает событие, взаимодействует с PackageManager и Storage, возвращает статус
	void doOffilneUser(int32_t client_socket);
	void clientErrorHandler(Package package, int32_t client_socket);
};

#endif
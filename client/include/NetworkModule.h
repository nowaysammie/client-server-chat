#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include "States.h"
#define BUFFER_SIZE 804
//клиентский NetworkModule
class NetworkModule {
private:
	int32_t socket; //сокет клиента
	sockaddr_in serverAdress; //адрес сервера
	pollfd fds[2]; //массив структур (server + stdin)
public:
	uint8_t init(uint32_t server_ip, uint16_t port); //происходит инициализация, создаётся сокет, пытается соединиться с сервером
	uint8_t connectToServer(); //попытка соединиться с сервером
	void toPoll(); //опрос сервера и потока ввода
	uint8_t getMessage(char* buffer); //получить сообщение
	uint8_t sendMessage(const char* buffer); //отправить сообщение
	void closeSocket(); //закрыть сокет сервера
};

#endif
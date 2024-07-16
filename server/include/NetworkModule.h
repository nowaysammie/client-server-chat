#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include "States.h"
#define BUFFER_SIZE 804

//серверный NetworkModule
class NetworkModule {
private:
	int32_t socket; //сокет сервера
	sockaddr_in serverAddress; //адрес сервера
	nfds_t count_fds; //количество элементов в fds
	pollfd* fds; //массив структур (users + stdin)
public:
	uint8_t init(uint32_t server_ip, uint16_t port); //происходит инициализация, создаётся сокет, сокет биндится к порту
	void toPoll(); //запускает опрос всех сокетов
	uint8_t appendClient(int32_t client_socket); //добавляем клиента в отслеживаемый массив !узнать тип event
	uint8_t removeClient(); //убрать клиента из списка отслеживаемых
	uint8_t getMessage(int32_t client_socket, char* buffer); //получить сообщение
	uint8_t sendMessage(int32_t client_socket, const char* buffer); //отправить сообщение
	void closeSocket(); //закрыть сокет сервера
};

#endif
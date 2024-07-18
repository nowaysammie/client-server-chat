#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "States.h"

#define BUFFER_SIZE 804
#define CONNECTED_USERS_MAX 15

// серверный NetworkModule
class NetworkModule
{
private:
	int32_t serverSocket;	   // сокет сервера
	sockaddr_in serverAddress; // адрес сервера
	nfds_t count_fds;		   // количество элементов в fds
	pollfd *fds;			   // массив структур (users + stdin)

public:
	uint8_t init(char *server_ip, uint16_t port);						// происходит инициализация, создаётся сокет, сокет биндится к порту
	void toPoll();														// запускает опрос всех сокетов
	uint8_t appendClient();												// добавляем клиента в отслеживаемый массив !узнать тип event
	uint8_t removeClient(int32_t client_socket);						// убрать клиента из списка отслеживаемых
	uint8_t getMessage(int32_t client_socket, char *buffer);			// получить сообщение
	uint8_t sendMessage(int32_t client_socket, const char *buffer);		// отправить сообщение
	pollfd *readyFd();													// возвращает указатель на готовый к обработке файловый дескриптор
	uint8_t getClientSocket(uint32_t client_uid, int32_t *dest_socket); // РЕАЛИЗОВАТЬ!!!
	pollfd *getFd(unsigned int index);									// возвращает указатель на элемент fds[index]
	void closeSocket();													// закрыть сокет сервера
};

#endif
#ifndef NETWORK_MODULE_CLIENT_H
#define NETWORK_MODULE_CLIENT_H

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
// клиентский NetworkModule
class NetworkModule
{
private:
	int32_t clientSocket;	   // сокет клиента
	sockaddr_in serverAddress; // адрес сервера
	pollfd *fds;			   // массив структур (server socket + stdin)

public:
	void setPollCin();
	void unsetPollCin();
	uint8_t init(char *server_ip);			 // происходит инициализация, создаётся сокет, пытается соединиться с сервером
	uint8_t connectToServer();				 // попытка соединиться с сервером
	int8_t toPoll();						 // опрос сервера и потока ввода
	uint8_t getMessage(char *buffer);		 // получить сообщение
	uint8_t sendMessage(const char *buffer); // отправить сообщение
	pollfd *readyFd();						 // возвращает указатель на готовый к обработке файловый дескриптор
	pollfd *getFd(unsigned int index);		 // возвращает указатель на элемент fds[index]
	void closeSocket();						 // закрыть сокет сервера
};

#endif
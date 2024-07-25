#include "NetworkModuleClient.h"
#include "StatesClient.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>

// клиентский NetworkModule

in_addr_t *NetworkModule::getSockaddrPtr()
{
	return &(serverAddress.sin_addr.s_addr);
}

void NetworkModule::unsetPollCin()
{
	fds[0].events = 0;
}
void NetworkModule::setPollCin()
{
	fds[0].events = POLLIN;
}

uint8_t NetworkModule::init(char *server_ip)
{
	uint8_t state = E_CONNECT;
	// создание массива файловых дескрипторов, добавление ФД потока ввода в массив опрашиваемых ФД
	fds = new pollfd[2];
	fds[0].fd = fileno(stdin);
	fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	// настройка протокола, ip-адреса и порта
	serverAddress.sin_family = AF_INET;
	int status = inet_pton(AF_INET, server_ip, &serverAddress.sin_addr.s_addr);
	if (status > 0)
	{
		serverAddress.sin_port = htons(22280);
		// создание сокета
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (clientSocket >= 0)
		{
			// разблокировка сокета
			fcntl(clientSocket, F_SETFL, O_NONBLOCK);
			fds[1].fd = clientSocket;
			fds[1].events = POLLIN;
			fds[1].revents = 0;
			state = SUCCESS;
		}
	}
	return state;
}

int8_t NetworkModule::toPoll()
{
	// опрос сокетов неопределённое время
	uint8_t state = E_CONNECT;
	int status = poll(fds, 2, -1);
	if (status == 0)
	{
		state = C_HANDLE_END;
	}
	else if (status > 0)
	{
		state = C_OK;
	}
	return state;
}

uint8_t NetworkModule::connectToServer()
{
	uint8_t state = SUCCESS;
	// попытка присоединиться к серверу
	int status = connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (status < 0)
	{
		state = E_CONNECT;
	}
	return state;
}

uint8_t NetworkModule::getMessage(char *buffer)
{
	uint8_t state = SUCCESS;
	// получаем сообщение от клиента
	int status = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	if (status <= 0)
	{
		state = E_CONNECT;
	}
	return state;
}

uint8_t NetworkModule::sendMessage(const char *buffer)
{
	uint8_t state = SUCCESS;
	// отправляем сообщение серверу
	int status = send(clientSocket, buffer, BUFFER_SIZE, 0);
	if (status <= 0)
	{
		state = E_CONNECT;
	}
	return state;
}

// получение указателя на fds[index]
pollfd *NetworkModule::getFd(unsigned int index)
{
	pollfd *temp = NULL;
	if (index < 2)
	{
		temp = (fds + index);
	}
	return temp;
}

// вернуть указатель на pollfd, на котором случился event
pollfd *NetworkModule::readyFd()
{
	pollfd *temp = NULL;
	for (int i = 0; i < 2; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			temp = (fds + i);
			break;
		}
	}
	return temp;
}

void NetworkModule::closeSocket()
{
	// закрытие сокета клиента
	close(clientSocket);
}

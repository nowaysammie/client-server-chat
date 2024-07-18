#include "../include/NetworkModule.h"

// клиентский NetworkModule

uint8_t NetworkModule::init(char *server_ip)
{
	// создание массива файловых дескрипторов, добавление ФД потока ввода в массив опрашиваемых ФД
	fds = new pollfd[2];
	fds[0].fd = fileno(stdin);
	fds[0].events = POLLIN;
	// настройка протокола, ip-адреса и порта
	serverAddress.sin_family = AF_INET;
	int state = inet_pton(AF_INET, server_ip, &serverAddress.sin_addr.s_addr);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	serverAddress.sin_port = htons(12345); // работает?
	// создание сокета
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
	{
		return E_CONNECT;
	}
	// разблокировка сокета
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	return SUCCESS;
}

uint8_t NetworkModule::connectToServer()
{
	// попытка присоединиться к серверу
	int state = connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (state < 0)
	{
		return E_CONNECT;
	}
	fds[1].fd = clientSocket;
	fds[2].events = POLLIN;
	return SUCCESS;
}

void NetworkModule::toPoll()
{
	// опрос сокетов неопределённое время
	int state = poll(fds, 2, -1);
}

uint8_t NetworkModule::getMessage(char *buffer)
{
	// получаем сообщение от клиента
	int state = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	return SUCCESS;
}

uint8_t NetworkModule::sendMessage(const char *buffer)
{
	// отправляем сообщение серверу
	int state = send(clientSocket, buffer, BUFFER_SIZE, 0);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	return SUCCESS;
}

// получение указателя на fds[index]
pollfd *NetworkModule::getFd(unsigned int index)
{
	if (index < 2)
	{
		return (fds + index);
	}
	return NULL;
}

// вернуть указатель на pollfd, на котором случился event
pollfd *NetworkModule::readyFd()
{
	for (int i = 0; i < 2; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			return (fds + i);
		}
	}
	return NULL;
}

void NetworkModule::closeSocket()
{
	// закрытие сокета клиента
	close(clientSocket);
}

#include "NetworkModule.h"

// клиентский NetworkModule

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
	// создание массива файловых дескрипторов, добавление ФД потока ввода в массив опрашиваемых ФД
	fds = new pollfd[2];
	fds[0].fd = fileno(stdin);
	fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	// настройка протокола, ip-адреса и порта
	serverAddress.sin_family = AF_INET;
	int state = inet_pton(AF_INET, server_ip, &serverAddress.sin_addr.s_addr);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	serverAddress.sin_port = htons(22280);
	// создание сокета
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	int optval = 1;
	setsockopt(clientSocket, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval));
	if (clientSocket < 0)
	{
		return E_CONNECT;
	}
	// разблокировка сокета
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	fds[1].fd = clientSocket;
	fds[1].events = POLLIN;
	fds[1].revents = 0;
	return SUCCESS;
}

int8_t NetworkModule::toPoll()
{
	// опрос сокетов неопределённое время
	return poll(fds, 2, -1); // посмотреть, нужно ли возвращаемое значение
}

uint8_t NetworkModule::connectToServer()
{
	// попытка присоединиться к серверу
	int state = connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (state < 0)
	{
		return E_CONNECT;
	}
	return SUCCESS;
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

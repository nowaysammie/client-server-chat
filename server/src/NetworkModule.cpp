#include "../include/NetworkModule.h"
#include "../include/States.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>

// серверный NetworkModule

// инициализация сетевого модуля
uint8_t NetworkModule::init(char *server_ip)
{
	uint8_t state = E_CONNECT;
	count_fds = 2;
	fds = new pollfd[2];
	// добавляем в отслеживамые поток ввода
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
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket >= 0)
		{
			// разблокировка сокета
			fcntl(serverSocket, F_SETFL, O_NONBLOCK);
			// добавляем в отслеживаемые слушающий сокет
			fds[1].fd = serverSocket;
			fds[1].events = POLLIN;
			fds[1].revents = 0;
			// привязка сокета к порту
			status = bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
			if (status >= 0)
			{
				listen(serverSocket, 50);
				state = SUCCESS;
			}
		}
	}
	return state;
}
// опросить дескрипторы
uint8_t NetworkModule::toPoll()
{
	// опрос сокетов неопределённое время
	uint8_t state = E_CONNECT;
	int status = poll(fds, count_fds, -1);
	if (status == 0)
	{
		state = SRV_HANDLE_END;
	}
	else if (status > 0)
	{
		state = SRV_OK;
	}
	return state;
}
// добавить клиента в отслеживаемые
uint8_t NetworkModule::appendClient()
{
	uint8_t state = E_CONNECT;
	int32_t client_socket = accept(serverSocket, NULL, NULL);
	if (client_socket >= 0)
	{
		if (count_fds < CONNECTED_USERS_MAX)
		{
			// создание расширенного массива опрашиваемых ФД
			pollfd *temp = new pollfd[++count_fds];
			for (int i = 0; i < (count_fds - 1); i++)
			{
				temp[i] = fds[i];
			}
			// добавление нового элемента
			temp[count_fds - 1].fd = client_socket;
			temp[count_fds - 1].events = POLLIN;
			temp[count_fds - 1].revents = 0;
			// смена указателя на обновленный массив
			delete[] fds;
			fds = temp;
			state = SUCCESS;
		}
		else
		{
			close(client_socket);
			state = E_SRV_FULL;
		}
	}
	else
	{
		close(client_socket);
	}
	return state;
}
// убрать клиента из отслеживаемых и закрыть его сокет
uint8_t NetworkModule::removeClient(int32_t client_socket)
{
	// создание уменьшенного массива опрашиваемых ФД
	pollfd *temp = new pollfd[--count_fds];
	int j = 0;
	for (int i = 0; i < (count_fds + 1); i++)
	{
		if (fds[i].fd != client_socket)
		{
			temp[j] = fds[i];
			j++;
		}
	}
	// смена указателя на обновленный массив
	delete[] fds;
	fds = temp;
	close(client_socket);
	return SUCCESS; // добавить ошибки?
}
// получить буфер
uint8_t NetworkModule::getMessage(int32_t client_socket, char *buffer)
{
	uint8_t state = E_CONNECT;
	// получаем сообщение от клиента
	int status = recv(client_socket, buffer, BUFFER_SIZE, 0);
	if (status != 0)
	{
		if (status > 0)
		{
			state = SUCCESS;
		}
		else
		{
			state = E_DATA;
		}
	}
	return state;
}
// отправить буфер
uint8_t NetworkModule::sendMessage(int32_t client_socket, const char *buffer)
{
	uint8_t state = E_CONNECT;
	// отправляем сообщение клиенту
	int status = send(client_socket, buffer, BUFFER_SIZE, 0);
	if (status != 0)
	{
		if (status > 0)
		{
			state = SUCCESS;
		}
		else
		{
			state = E_DATA;
		}
	}
	return state;
}
// закрытие сокета сервера
void NetworkModule::closeSocket()
{
	// закрытие сокета сервера
	close(serverSocket);
}
// вернуть указатель на pollfd, на котором случился event
pollfd *NetworkModule::readyFd()
{
	pollfd *temp = NULL;
	for (int i = 0; i < count_fds; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			temp = (fds + i);
			break;
		}
	}
	return temp;
}
// получение указателя на fds[index]
pollfd *NetworkModule::getFd(unsigned int index)
{
	pollfd *temp = NULL;
	if (index < count_fds)
	{
		temp = (fds + index);
	}
	return temp;
}
// получение UID пользователя по логину
uint32_t NetworkModule::getClientUid(std::string login)
{
	uint32_t hash = 0;
	for (char c : login)
	{
		hash = hash * 31 + static_cast<uint32_t>(c);
	}
	return hash;
}